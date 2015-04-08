/*
 * (c) Domen Puncer, Visionect, d.o.o.
 * BSD License
 *
 * v0.2 add support for SDHC
 * 
 * Changes made by Fabien Royer on behalf of Nwazet, LLC. under the terms of the New BSD license | http://nwazet.com
 */

#include <stdio.h>
#include "stdint.h"
#include "stm32f0xx_spi.h"
#include "stm32f0xx_gpio.h"
#include "ffconf.h"
#include "diskio.h"
#include "rtc.h"
#include "peripherals.h"
#include "LedController.h"
#include "vt100.h"

typedef uint8_t u8;
typedef uint16_t u16;
typedef int32_t u32;

/*
 * Code is split into 3 parts:
 * - generic SPI code: adapt for your MCU
 * - sd card code, with crc7 and crc16 calculations
 *   there's lots of it, but it's simple
 * - fatfs interface. If you use anything else, look here for
 *   interface to SD card code
 */

struct hwif {
  int initialized;
  int sectors;
  int erase_sectors;
  int capabilities;
};

typedef struct hwif hwif;

#define CAP_VER2_00	(1<<0)
#define CAP_SDHC	(1<<1)

#define spi_cs_low() do { GPIO_WriteBit(SD_CS_GPIO_PORT, SD_CS_PIN, Bit_RESET); } while (0)
#define spi_cs_high() do { GPIO_WriteBit(SD_CS_GPIO_PORT, SD_CS_PIN, Bit_SET); } while (0)

enum sd_speed {
  SD_SPEED_INVALID = -1,
  SD_SPEED_LOW = SPI_BaudRatePrescaler_64,      // 16000kHz/64 = 250kHz < 400kHz
  SD_SPEED_HIGH = SPI_BaudRatePrescaler_2       // 16000kHz/2 = 8MHz
};

/*** spi functions ***/
void spi_set_speed(enum sd_speed speed);

void spi_init(){
  GPIO_InitTypeDef      GpioInitStruct;
  SPI_InitTypeDef       SPI_InitStructure;

  // Configure the GPIOs driving the SD card
  GpioInitStruct.GPIO_Pin = SD_SPI_SCK_PIN | SD_SPI_MOSI_PIN | SD_SPI_MISO_PIN;
  GpioInitStruct.GPIO_Mode = GPIO_Mode_AF;
  GpioInitStruct.GPIO_Speed = SD_GPIO_SPEED;
  GpioInitStruct.GPIO_OType = GPIO_OType_PP;
  GpioInitStruct.GPIO_PuPd  = GPIO_PuPd_NOPULL;
  GPIO_Init(SD_SPI_SCK_GPIO_PORT, &GpioInitStruct);

  // Remap the pins to their alternate functions
  GPIO_PinAFConfig(SD_SPI_SCK_GPIO_PORT, SD_SPI_SCK_SOURCE, SD_SPI_SCK_AF);
  GPIO_PinAFConfig(SD_SPI_MISO_GPIO_PORT, SD_SPI_MISO_SOURCE, SD_SPI_MISO_AF);
  GPIO_PinAFConfig(SD_SPI_MOSI_GPIO_PORT, SD_SPI_MOSI_SOURCE, SD_SPI_MOSI_AF);

  /* SPI configuration */
  SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
  SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
  SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
  SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;
  SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;
  SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
  SPI_InitStructure.SPI_BaudRatePrescaler = SD_SPEED_LOW;
  SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
  SPI_InitStructure.SPI_CRCPolynomial = 7;

  SPI_Init(SD_SPI, &SPI_InitStructure);
  SPI_CalculateCRC(SD_SPI, DISABLE);
  SPI_Cmd(SD_SPI, ENABLE);

  /* Configure I/O for Flash Chip select */
  GpioInitStruct.GPIO_Pin = SD_CS_PIN;
  GpioInitStruct.GPIO_Mode = GPIO_Mode_OUT;
  GpioInitStruct.GPIO_OType = GPIO_OType_PP;
  GpioInitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GpioInitStruct.GPIO_Speed = SD_GPIO_SPEED;
  GPIO_Init(SD_CS_GPIO_PORT, &GpioInitStruct);
}

void spi_set_speed(enum sd_speed speed){
  SPI_InitTypeDef SPI_InitStructure;

  SPI_Cmd(SD_SPI, DISABLE);

  SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
  SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
  SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
  SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;
  SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;
  SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
  SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
  SPI_InitStructure.SPI_CRCPolynomial = 7;
  SPI_InitStructure.SPI_BaudRatePrescaler = speed;

  SPI_Init(SD_SPI, &SPI_InitStructure);
  SPI_Cmd(SD_SPI, ENABLE);
}

u8 spi_txrx(u8 data){
  /* RXNE always happens after TXE, so if this function is used we don't need to check for TXE */
  SPI_SendData8(SD_SPI, data);
  while(SPI_I2S_GetFlagStatus(SD_SPI, SPI_I2S_FLAG_RXNE) == RESET);
  return SPI_ReceiveData8(SD_SPI);
}

void spi_tx(u8 data){
  /* RXNE always happens after TXE, so if this function is used we don't need to check for TXE */
  while(SPI_I2S_GetFlagStatus(SD_SPI, SPI_I2S_FLAG_TXE) == RESET);
  SPI_SendData8(SD_SPI, data);
  SPI_ReceiveData8(SD_SPI);
}

/* crc helpers */
u8 crc7_one(u8 t, u8 data){
  int i;
  const u8 g = 0x89;
  t ^= data;
  for (i=0; i<8; i++) {
    if (t & 0x80)
            t ^= g;
    t <<= 1;
  }
  return t;
}

u8 crc7(const u8 *p, int len){
  int j;
  u8 crc = 0;
  for (j=0; j<len; j++)
          crc = crc7_one(crc, p[j]);
  return crc>>1;
}

/* http://www.eagleairaust.com.au/code/crc16.htm */
u16 crc16_ccitt(u16 crc, u8 ser_data){
  crc  = (u8)(crc >> 8) | (crc << 8);
  crc ^= ser_data;
  crc ^= (u8)(crc & 0xff) >> 4;
  crc ^= (crc << 8) << 4;
  crc ^= ((crc & 0xff) << 4) << 1;
  return crc;
}

u16 crc16(const u8 *p, int len){
  int i;
  u16 crc = 0;
  for (i=0; i<len; i++)
          crc = crc16_ccitt(crc, p[i]);
  return crc;
}

/*** sd functions - on top of spi code ***/
void sd_cmd(u8 cmd, u32 arg){
  u8 crc = 0;
  spi_txrx(0x40 | cmd);
  crc = crc7_one(crc, 0x40 | cmd);
  spi_txrx(arg >> 24);
  crc = crc7_one(crc, arg >> 24);
  spi_txrx(arg >> 16);
  crc = crc7_one(crc, arg >> 16);
  spi_txrx(arg >> 8);
  crc = crc7_one(crc, arg >> 8);
  spi_txrx(arg);
  crc = crc7_one(crc, arg);
  //spi_txrx(0x95);	/* crc7, for cmd0 */
  spi_txrx(crc | 0x1);	/* crc7, for cmd0 */
}

u8 sd_get_r1(){
  int tries = 1000;
  u8 r;

  while (tries--) {
    r = spi_txrx(0xff);
    if ((r & 0x80) == 0)
            return r;
  }
  return 0xff;
}

u16 sd_get_r2(){
  int tries = 1000;
  u16 r;

  while (tries--) {
    r = spi_txrx(0xff);
    if ((r & 0x80) == 0)
            break;
  }
  if (tries < 0)
          return 0xff;
  r = r<<8 | spi_txrx(0xff);

  return r;
}

/*
 * r1, then 32-bit reply... same format as r3
 */
u8 sd_get_r7(u32 *r7){
  u32 r;
  r = sd_get_r1();
  if (r != 0x01)
          return r;

  r = spi_txrx(0xff) << 24;
  r |= spi_txrx(0xff) << 16;
  r |= spi_txrx(0xff) << 8;
  r |= spi_txrx(0xff);

  *r7 = r;
  return 0x01;
}

#define sd_get_r3 sd_get_r7

static const char *r1_strings[7] = {
  "in idle",
  "erase reset",
  "illegal command",
  "communication crc error",
  "erase sequence error",
  "address error",
  "parameter error"
};

void print_r1(u8 r){
  int i;
  printf("R1: %02x\r\n", r);
  for (i=0; i<7; i++){
    if (r & (1<<i)){
      vt100TextColor(VT100_Red, VT100_Black);
      printf("  %s\r\n", r1_strings[i]);
      vt100TextDefault();
    }
  }
}

static const char *r2_strings[15] = {
  "card is locked",
  "wp erase skip | lock/unlock cmd failed",
  "error",
  "CC error",
  "card ecc failed",
  "wp violation",
  "erase param",
  "out of range | csd overwrite",
  "in idle state",
  "erase reset",
  "illegal command",
  "com crc error",
  "erase sequence error",
  "address error",
  "parameter error"
};

void print_r2(u16 r){
  int i;
  printf("R2: %04x\r\n", r);
  for (i=0; i<15; i++){
    if (r & (1<<i)){
      vt100TextColor(VT100_Red, VT100_Black);
      printf("  %s\r\n", r2_strings[i]);
      vt100TextDefault();
    }
  }
}

void PrintFail(){
  vt100TextColor(VT100_Red, VT100_Black);
  printf("fail\r\n");
  vt100TextDefault();
}

void PrintSuccess(){
  vt100TextColor(VT100_Green, VT100_Black);
  printf("ok\r\n");
  vt100TextDefault();
}

/* Nec (=Ncr? which is limited to [0,8]) dummy bytes before lowering CS,
 * as described in sandisk doc, 5.4. */
void sd_nec(){
  int i;
  for (i=0; i<8; i++)
          spi_txrx(0xff);
}

int sd_init(hwif *hw){
  int i;
  int r;
  u32 r7;
  u32 r3;
  int tries;
  u32 hcs = 0;

  hw->capabilities = 0;

  /* start with 100-400 kHz clock */
  spi_set_speed(SD_SPEED_LOW);

  printf("cmd0 - reset: ");

  spi_cs_high();
  /* 74+ clocks with CS high */
  for (i=0; i<10; i++){
    spi_tx(0xff);
  }

  /* reset */
  spi_cs_low();
  sd_cmd(0, 0);
  r = sd_get_r1();
  sd_nec();
  spi_cs_high();
  if (r == 0xff)
    goto err_spi;
  if (r != 0x01) {
    PrintFail();
    print_r1(r);
    goto err;
  }
  PrintSuccess();

  printf("cmd8 - voltage: ");

  /* ask about voltage supply */
  spi_cs_low();
  sd_cmd(8, 0x1aa /* VHS = 1 */);
  r = sd_get_r7(&r7);
  sd_nec();
  spi_cs_high();
  hw->capabilities |= CAP_VER2_00;
  if (r == 0xff)
    goto err_spi;
  if (r == 0x01){
    PrintSuccessMessage("SD v2.x");
  }
  else if (r & 0x4) {
    hw->capabilities &= ~CAP_VER2_00;
    PrintSuccessMessage("not implemented, SD v1.x");
  } else {
    PrintFail();
    print_r1(r);
    return -2;
  }

  printf("cmd58 - OCR: ");

  /* ask about voltage supply */
  spi_cs_low();
  sd_cmd(58, 0);
  r = sd_get_r3(&r3);
  sd_nec();
  spi_cs_high();
  if (r == 0xff)
    goto err_spi;
  if (r != 0x01 && !(r & 0x4)) { /* allow it to not be implemented - old cards */
    PrintFail();
    print_r1(r);
    return -2;
  }
  else {
    int i;
    for (i=4; i<=23; i++){
      if (r3 & 1<<i)
        break;
    }
    printf("Vdd voltage window: %i.%i-", (12+i)/10, (12+i)%10);

    for (i=23; i>=4; i--){
      if (r3 & 1<<i)
        break;
    }
    /* CCS shouldn't be valid here yet */
    printf("%i.%iV, CCS:%li, power up status:%li, ", (13+i)/10, (13+i)%10, r3>>30 & 1, r3>>31);
    PrintSuccess();
  }

  printf("acmd41 - HCS: ");

  tries = 1000;
  /* say we support SDHC */
  if (hw->capabilities & CAP_VER2_00)
    hcs = 1<<30;

  /* needs to be polled until in_idle_state becomes 0 */
  do {
    /* send we don't support SDHC */
    spi_cs_low();
    /* next cmd is ACMD */
    sd_cmd(55, 0);
    r = sd_get_r1();
    sd_nec();
    spi_cs_high();
    if (r == 0xff)
      goto err_spi;
    /* well... it's probably not idle here, but specs aren't clear */
    if (r & 0xfe) {
      PrintFail();
      print_r1(r);
      goto err;
    }

    spi_cs_low();
    sd_cmd(41, hcs);
    r = sd_get_r1();
    sd_nec();
    spi_cs_high();
    if (r == 0xff)
      goto err_spi;
    if (r & 0xfe) {
      PrintFail();
      print_r1(r);
      goto err;
    }
  } while (r != 0 && tries--);
  if (tries == -1) {
    PrintErrorMessage("timeout");
    goto err;
  }

  PrintSuccess();

  /* Seems after this card is initialized which means bit 0 of R1 will be cleared. Not too sure. */

  if (hw->capabilities & CAP_VER2_00) {
    printf("cmd58 - OCR (2nd time): ");
    /* ask about voltage supply */
    spi_cs_low();
    sd_cmd(58, 0);
    r = sd_get_r3(&r3);
    sd_nec();
    spi_cs_high();
    if (r == 0xff)
            goto err_spi;
    if (r & 0xfe) {
            PrintFail();
            print_r1(r);
            return -2;
    } else {
#if 1
      int i;
      for (i=4; i<=23; i++){
        if (r3 & 1<<i)
          break;
      }
      printf("Vdd voltage window: %i.%i-", (12+i)/10, (12+i)%10);

      for (i=23; i>=4; i--){
        if (r3 & 1<<i)
          break;
      }
      /* CCS shouldn't be valid here yet */
      printf("%i.%iV, CCS:%li, power up status:%li, ", (13+i)/10, (13+i)%10, r3>>30 & 1, r3>>31);

      // XXX power up status should be 1 here, since we're finished initializing, but it's not. WHY?
      // that means CCS is invalid, so we'll set CAP_SDHC later
#endif
      if (r3>>30 & 1) {
        hw->capabilities |= CAP_SDHC;
      }
      PrintSuccess();
    }
  }

  /* with SDHC block length is fixed to 1024 */
  if ((hw->capabilities & CAP_SDHC) == 0) {
    printf("cmd16 - block length: ");

    spi_cs_low();
    sd_cmd(16, 512);
    r = sd_get_r1();
    sd_nec();
    spi_cs_high();
    if (r == 0xff)
      goto err_spi;
    if (r & 0xfe) {
      PrintFail();
      print_r1(r);
      goto err;
    }
    PrintSuccess();
  }

  printf("cmd59 - enable CRC: ");

  /* crc on */
  spi_cs_low();
  sd_cmd(59, 0);
  r = sd_get_r1();
  sd_nec();
  spi_cs_high();
  if (r == 0xff)
    goto err_spi;
  if (r & 0xfe) {
    PrintFail();
    print_r1(r);
    goto err;
  }
  PrintSuccess();

  /* now we can up the clock to <= 25 MHz */
  spi_set_speed(SD_SPEED_HIGH);

  return 0;
err_spi:
  PrintErrorMessage("fail spi");
  return -1;
err:
  return -2;
}

int sd_read_status(hwif *hw){
  u16 r2;
  spi_cs_low();
  sd_cmd(13, 0);
  r2 = sd_get_r2();
  sd_nec();
  spi_cs_high();
  if (r2 & 0x8000)
    return -1;
  if (r2)
    print_r2(r2);
  return 0;
}

/* 0xfe marks data start, then len bytes of data and crc16 */
int sd_get_data(hwif *hw, u8 *buf, int len){
  int tries = 20000;
  u8 r;
  u16 _crc16;
  u16 calc_crc;
  int i;

  while (tries--) {
    r = spi_txrx(0xff);
    if (r == 0xfe)
      break;
  }
  if (tries < 0)
    return -1;

  for (i=0; i<len; i++){
    buf[i] = spi_txrx(0xff);
  }
  _crc16 = spi_txrx(0xff) << 8;
  _crc16 |= spi_txrx(0xff);

  calc_crc = crc16(buf, len);
  if (_crc16 != calc_crc) {
    vt100TextColor(VT100_Red, VT100_Black);
    printf("%s, crcs differ: %04x vs. %04x, len:%i\r\n", __func__, _crc16, calc_crc, len);
    vt100TextDefault();
    return -1;
  }

  return 0;
}

int sd_put_data(hwif *hw, const u8 *buf, int len){
  u8 r;
  int tries = 10;
  u8 b[16];
  int bi = 0;
  u16 crc;
  b[0] = 0;

  spi_txrx(0xfe); /* data start */

  while (len--){
    spi_txrx(*buf++);
  }
  crc = crc16(buf, len);
  /* crc16 */
  spi_txrx(crc>>8);
  spi_txrx(crc);

  /* normally just one dummy read in between... specs don't say how many */
  while (tries--) {
    r = spi_txrx(0xff);
    b[bi++] = r;
    if (r != 0xff)
      break;
  }
  if (tries < 0)
    return -1;

  /* poll busy, about 300 reads for 256 MB card */
  tries = 100000;
  while (tries--) {
    if (spi_txrx(0xff) == 0xff)
      break;
  }
  if (tries < 0)
    return -2;

  /* data accepted, WIN */
  if ((r & 0x1f) == 0x05)
    return 0;

  return r;
}

int sd_read_csd(hwif *hw){
  u8 buf[16];
  int r;
  int capacity;

  spi_cs_low();
  sd_cmd(9, 0);
  r = sd_get_r1();
  if (r == 0xff) {
    spi_cs_high();
    return -1;
  }
  if (r & 0xfe) {
    spi_cs_high();
    PrintErrorMessage(__func__);
    print_r1(r);
    return -2;
  }

  r = sd_get_data(hw, buf, 16);
  sd_nec();
  spi_cs_high();
  if (r == -1) {
    PrintErrorMessage("failed to get CSD");
    return -3;
  }

  if ((buf[0] >> 6) + 1 == 1) {
    /* CSD v1 */
    printf("CSD: CSD v%i taac:%02x, nsac:%i, tran:%02x, classes:%02x%x, read_bl_len:%i,\r\n"
            "read_bl_part:%i, write_blk_misalign:%i, read_blk_misalign:%i, dsr_imp:%i,\r\n"
            "c_size:%i, vdd_rmin:%i, vdd_rmax:%i, vdd_wmin:%i, vdd_wmax:%i,\r\n"
            "c_size_mult:%i, erase_blk_en:%i, erase_s_size:%i,\r\n"
            "wp_grp_size:%i, wp_grp_enable:%i, r2w_factor:%i, write_bl_len:%i, write_bl_part:%i,\r\n"
            "filef_gpr:%i, copy:%i, perm_wr_prot:%i, tmp_wr_prot:%i, filef:%i\r\n",
                    (buf[0] >> 6) + 1,
                    buf[1], buf[2], buf[3],
                    buf[4], buf[5] >> 4, 1<<(buf[5] & 0xf), /* classes, read_bl_len */
                    buf[6]>>7, (buf[6]>>6)&1, (buf[6]>>5)&1, (buf[6]>>4)&1,
                    (buf[6]&0x3)<<10 | buf[7]<<2 | buf[8]>>6, /* c_size */
                    (buf[8]&0x38)>>3, buf[8]&0x07, buf[9]>>5, (buf[9]>>2)&0x7,
                    1<<(2+(((buf[9]&3) << 1) | buf[10]>>7)), /* c_size_mult */
                    (buf[10]>>6)&1,
                    ((buf[10]&0x3f)<<1 | buf[11]>>7) + 1, /* erase sector size */
                    (buf[11]&0x7f) + 1, /* write protect group size */
                    buf[12]>>7, 1<<((buf[12]>>2)&7),
                    1<<((buf[12]&3)<<2 | buf[13]>>6), /* write_bl_len */
                    (buf[13]>>5)&1,
                    buf[14]>>7, (buf[14]>>6)&1, (buf[14]>>5)&1, (buf[14]>>4)&1,
                    (buf[14]>>2)&3 /* file format */);
    /* ^ = (c_size+1) * 2**(c_size_mult+2) * 2**(read_bl_len-9) */
    capacity = (((buf[6]&0x3)<<10 | buf[7]<<2 | buf[8]>>6)+1) << (2+(((buf[9]&3) << 1) | buf[10]>>7)) << ((buf[5] & 0xf) - 9);
  } else {
    /* CSD v2 */
    /* this means the card is HC */
    hw->capabilities |= CAP_SDHC;

    printf("CSD: CSD v%i taac:%02x, nsac:%i, tran:%02x, classes:%02x%x, read_bl_len:%i,\r\n"
            "read_bl_part:%i, write_blk_misalign:%i, read_blk_misalign:%i, dsr_imp:%i,\r\n"
            "c_size:%i, erase_blk_en:%i, erase_s_size:%i,\r\n"
            "wp_grp_size:%i, wp_grp_enable:%i, r2w_factor:%i, write_bl_len:%i, write_bl_part:%i,\r\n"
            "filef_gpr:%i, copy:%i, perm_wr_prot:%i, tmp_wr_prot:%i, filef:%i\r\n",
                    (buf[0] >> 6) + 1,
                    buf[1], buf[2], buf[3],
                    buf[4], buf[5] >> 4, 1<<(buf[5] & 0xf), /* classes, read_bl_len */
                    buf[6]>>7, (buf[6]>>6)&1, (buf[6]>>5)&1, (buf[6]>>4)&1,
                    buf[7]<<16 | buf[8]<<8 | buf[9], /* c_size */
                    (buf[10]>>6)&1,
                    ((buf[10]&0x3f)<<1 | buf[11]>>7) + 1, /* erase sector size */
                    (buf[11]&0x7f) + 1, /* write protect group size */
                    buf[12]>>7, 1<<((buf[12]>>2)&7),
                    1<<((buf[12]&3)<<2 | buf[13]>>6), /* write_bl_len */
                    (buf[13]>>5)&1,
                    buf[14]>>7, (buf[14]>>6)&1, (buf[14]>>5)&1, (buf[14]>>4)&1,
                    (buf[14]>>2)&3 /* file format */);

    capacity = buf[7]<<16 | buf[8]<<8 | buf[9]; /* in 512 kB */
    capacity *= 1024; /* in 512 B sectors */
  }
  vt100TextColor(VT100_Green, VT100_Black);
  printf("SD card capacity = %i KB\r\n", capacity/2);
  vt100TextDefault();

  hw->sectors = capacity;

  /* if erase_blk_en = 0, then only this many sectors can be erased at once this is NOT yet tested */
  hw->erase_sectors = 1;
  if (((buf[10]>>6)&1) == 0)
          hw->erase_sectors = ((buf[10]&0x3f)<<1 | buf[11]>>7) + 1;

  return 0;
}

int sd_read_cid(hwif *hw){
  u8 buf[16];
  int r;

  spi_cs_low();
  sd_cmd(10, 0);
  r = sd_get_r1();
  if (r == 0xff) {
    spi_cs_high();
    return -1;
  }
  if (r & 0xfe) {
    spi_cs_high();
    PrintErrorMessage(__func__);
    print_r1(r);
    return -2;
  }

  r = sd_get_data(hw, buf, 16);
  sd_nec();
  spi_cs_high();
  if (r == -1) {
    PrintErrorMessage("failed to get CID");
    return -3;
  }
  vt100TextColor(VT100_White, VT100_Black);
  printf("CID: mid:%x, oid:%c%c, pnm:%c%c%c%c%c, prv:%i.%i, psn:%02x%02x%02x%02x, mdt:%i/%i\r\n",
        buf[0], buf[1], buf[2],			/* mid, oid */
        buf[3], buf[4], buf[5], buf[6], buf[7],	/* pnm */
        buf[8] >> 4, buf[8] & 0xf,		/* prv */
        buf[9], buf[10], buf[11], buf[12],	/* psn */
        2000 + (buf[13]<<4 | buf[14]>>4), 1 + (buf[14] & 0xf));
  vt100TextDefault();
  return 0;
}

int sd_readsector(hwif *hw, u32 address, u8 *buf){
  int r;

  spi_cs_low();
  if (hw->capabilities & CAP_SDHC){
    sd_cmd(17, address); /* read single block */
  } else {
    sd_cmd(17, address*512); /* read single block */
  }

  r = sd_get_r1();
  if (r == 0xff) {
    spi_cs_high();
    r = -1;
    goto fail;
  }
  if (r & 0xfe) {
    spi_cs_high();
    PrintErrorMessage(__func__);
    print_r1(r);
    r = -2;
    goto fail;
  }

  r = sd_get_data(hw, buf, 512);
  sd_nec();
  spi_cs_high();
  if (r == -1) {
    r = -3;
    goto fail;
  }

  return 0;
fail:
  vt100TextColor(VT100_Red, VT100_Black);
  printf("failed to read sector %li, err:%i\r\n", address, r);
  vt100TextDefault();
  return r;
}

int sd_writesector(hwif *hw, u32 address, const u8 *buf){
  int r;

  spi_cs_low();
  if (hw->capabilities & CAP_SDHC){
    sd_cmd(24, address); /* write block */
  } else {
    sd_cmd(24, address*512); /* write block */
  }

  r = sd_get_r1();
  if (r == 0xff) {
    spi_cs_high();
    r = -1;
    goto fail;
  }
  if (r & 0xfe) {
    spi_cs_high();
    PrintErrorMessage(__func__);
    print_r1(r);
    r = -2;
    goto fail;
  }

  spi_txrx(0xff); /* Nwr (>= 1) high bytes */
  r = sd_put_data(hw, buf, 512);
  sd_nec();
  spi_cs_high();
  if (r != 0) {
    vt100TextColor(VT100_Red, VT100_Black);
    printf("sd_put_data returned: %i\r\n", r);
    vt100TextDefault();
    r = -3;
    goto fail;
  }

  /* efsl code is weird shit, 0 is error in there?
   * not that it's properly handled or anything,
   * and the return type is char, fucking efsl */
  return 0;
fail:
  vt100TextColor(VT100_Red, VT100_Black);
  printf("failed to write sector %li, err:%i\r\n", address, r);
  vt100TextDefault();
  return r;
}
//
// public API - on top of sd/spi code
//
int hwif_init(hwif* hw){
  if (hw->initialized)
    return 0;

  printf("SD card init:\r\n");

  if (GPIO_ReadInputData(SD_DETECT_GPIO_PORT) & SD_DETECT_PIN){
    PrintErrorMessage("no media detected");
    return -1;
  }

  spi_init();

  int tries = 10;

  while (tries--) {
    if (sd_init(hw) == 0)
      break;
  }
  if (tries == -1)
    return -1;

  /* read status register */
  sd_read_status(hw);

  sd_read_cid(hw);
  if (sd_read_csd(hw) != 0)
    return -1;

  hw->initialized = 1;

  printf("SD card init: ");
  PrintSuccess();

  return 0;
}

int sd_read(hwif* hw, u32 address, u8 *buf){
  int r;
  int tries = 10;

  r = sd_readsector(hw, address, buf);

  while (r < 0 && tries--) {
    if (sd_init(hw) != 0)
      continue;
    /* read status register */
    sd_read_status(hw);
    r = sd_readsector(hw, address, buf);
  }
  if (tries == -1){
    vt100TextColor(VT100_Red, VT100_Black);
    printf("%s: couldn't read sector %li\r\n", __func__, address);
    vt100TextDefault();
  }
  return r;
}

int sd_write(hwif* hw, u32 address,const u8 *buf){
  int r;
  int tries = 10;

  r = sd_writesector(hw, address, buf);

  while (r < 0 && tries--) {
    if (sd_init(hw) != 0)
      continue;
    /* read status register */
    sd_read_status(hw);
    r = sd_writesector(hw, address, buf);
  }
  if (tries == -1){
    vt100TextColor(VT100_Red, VT100_Black);
    printf("%s: couldn't write sector %li\r\n", __func__, address);
    vt100TextDefault();
  }
  return r;
}
//
// fatfs code that uses the public API
//
static hwif hw;

void disk_reset(){
  hw.capabilities = 0;
  hw.erase_sectors = 0;
  hw.initialized = 0;
  hw.sectors = 0;
}

DSTATUS disk_initialize(BYTE drv){
  LedControllerSet(SD, 0, Bit_SET);
  if (hwif_init(&hw) == 0){
    LedControllerSet(SD, 0, Bit_RESET);
    return 0;
  }
  LedControllerSet(SD, 0, Bit_RESET);
  return STA_NOINIT;
}

DSTATUS disk_status(BYTE drv){
  DSTATUS status = 0;
  if (GPIO_ReadInputData(SD_DETECT_GPIO_PORT) & SD_DETECT_PIN){
    disk_reset();
    return STA_NODISK;
  }
  if (GPIO_ReadInputData(SD_WRITE_PROTECT_GPIO_PORT) & SD_WRITE_PROTECT_PIN){
    status |= STA_PROTECT;
  }
  if (hw.initialized)
    return status;

  return STA_NOINIT;
}

DRESULT disk_read(BYTE drv, BYTE *buff, DWORD sector, BYTE count){
  LedControllerSet(SD, 0, Bit_SET);
  for (int i=0; i<count; i++){
    if (sd_read(&hw, sector+i, buff+512*i) != 0){
      LedControllerSet(SD, 0, Bit_RESET);
      return RES_ERROR;
    }
  }
  LedControllerSet(SD, 0, Bit_RESET);
  return RES_OK;
}

DRESULT disk_write(BYTE drv, const BYTE *buff, DWORD sector, BYTE count){
  LedControllerSet(SD, 0, Bit_SET);
  for (int i=0; i<count; i++){
    if (sd_write(&hw, sector+i, buff+512*i) != 0){
      LedControllerSet(SD, 0, Bit_RESET);
      return RES_ERROR;
    }
  }
  LedControllerSet(SD, 0, Bit_RESET);
  return RES_OK;
}

DRESULT disk_ioctl(BYTE drv, BYTE ctrl, void *buff){
  switch (ctrl) {
  case CTRL_SYNC:
          return RES_OK;
  case GET_SECTOR_SIZE:
          *(WORD*)buff = 512;
          return RES_OK;
  case GET_SECTOR_COUNT:
          *(DWORD*)buff = hw.sectors;
          return RES_OK;
  case GET_BLOCK_SIZE:
          *(DWORD*)buff = hw.erase_sectors;
          return RES_OK;
  }
  return RES_PARERR;
}
/*
 * FAT filestamp format:
 * [31:25] - year - 1980
 * [24:21] - month 1..12
 * [20:16] - day 1..31
 * [15:11] - hour 0..23
 * [10:5]  - minute 0..59
 * [4:0]   - second/2 0..29
 * so... midnight 2009 is 0x3a000000
 */
DWORD get_fattime(){
  return GetRtcTimeFatFormat();
}
void disk_check(){
}

#include "ILI9341.h"
#include "systick.h"
#include "gpio.h"

enum {
  MemoryAccessControlNormalOrder,
  MemoryAccessControlReverseOrder
} MemoryAccessControlRefreshOrder;

enum {
  MemoryAccessControlColorOrderBGR,
  MemoryAccessControlColorOrderRGB
} MemoryAccessControlColorOrder;

enum {
    ColumnAddressSet = 0x2a,
    PageAddressSet = 0x2b,
    MemoryWrite = 0x2c,
    MemoryAccessControl = 0x36,
    WriteDisplayBrightness = 0x51
} ILI9341Register;

static lcdProperties_t  lcdProperties = { 240, 320, true, true, true };
static lcdOrientation_t lcdOrientation = LCD_ORIENTATION_PORTRAIT;

static unsigned char lcdPortraitConfig = 0;
static unsigned char lcdLandscapeConfig = 0;

void lcdInit(void)
{
  lcdPortraitConfig = lcdBuildMemoryAccessControlConfig(
                                                    MemoryAccessControlNormalOrder, // rowAddressOrder
                                                    MemoryAccessControlReverseOrder, // columnAddressOrder
                                                    MemoryAccessControlNormalOrder, // rowColumnExchange
                                                    MemoryAccessControlNormalOrder, // verticalRefreshOrder
                                                    MemoryAccessControlColorOrderRGB, // colorOrder
                                                    MemoryAccessControlNormalOrder); // horizontalRefreshOrder

  lcdLandscapeConfig = lcdBuildMemoryAccessControlConfig(
                                                    MemoryAccessControlNormalOrder, // rowAddressOrder
                                                    MemoryAccessControlNormalOrder, // columnAddressOrder
                                                    MemoryAccessControlReverseOrder, // rowColumnExchange
                                                    MemoryAccessControlReverseOrder, // verticalRefreshOrder
                                                    MemoryAccessControlColorOrderRGB, // colorOrder
                                                    MemoryAccessControlReverseOrder); // horizontalRefreshOrder
  delay(120);

  lcdReset();

  GPIO_WriteBit(TFT_LCD_CONTROL_PORT, TFT_LCD_CS_PIN, Bit_RESET);

  lcdWriteCommand(0x01);
  delay(10);
  lcdWriteCommand(0x28);

  lcdWriteCommand(0xcf);
  lcdWriteData(0x00);
  lcdWriteData(0x83);
  lcdWriteData(0x30);

  lcdWriteCommand(0xed);
  lcdWriteData(0x64);
  lcdWriteData(0x03);
  lcdWriteData(0x12);
  lcdWriteData(0x81);

  lcdWriteCommand(0xe8);
  lcdWriteData(0x85);
  lcdWriteData(0x01);
  lcdWriteData(0x79);

  lcdWriteCommand(0xcb);
  lcdWriteData(0x39);
  lcdWriteData(0x2c);
  lcdWriteData(0x00);
  lcdWriteData(0x34);
  lcdWriteData(0x02);

  lcdWriteCommand(0xf7);
  lcdWriteData(0x20);

  lcdWriteCommand(0xea);
  lcdWriteData(0x00);
  lcdWriteData(0x00);

  lcdWriteCommand(0xc0);
  lcdWriteData(0x26);

  lcdWriteCommand(0xc1);
  lcdWriteData(0x11);

  lcdWriteCommand(0xc5);
  lcdWriteData(0x35);
  lcdWriteData(0x3e);

  lcdWriteCommand(0xc7);
  lcdWriteData(0xbe);

  lcdWriteCommand(MemoryAccessControl);
  lcdWriteData(lcdPortraitConfig);

  lcdWriteCommand(0x3a);
  lcdWriteData(0x55);

  lcdWriteCommand(0xb1);
  lcdWriteData(0x00);
  lcdWriteData(0x1B);

  lcdWriteCommand(0xf2);
  lcdWriteData(0x08);

  lcdWriteCommand(0x26);
  lcdWriteData(0x01);

  lcdWriteCommand(0xe0);
  lcdWriteData(0x1f);
  lcdWriteData(0x1a);
  lcdWriteData(0x18);
  lcdWriteData(0x0a);
  lcdWriteData(0x0f);
  lcdWriteData(0x06);
  lcdWriteData(0x45);
  lcdWriteData(0x87);
  lcdWriteData(0x32);
  lcdWriteData(0x0a);
  lcdWriteData(0x07);
  lcdWriteData(0x02);
  lcdWriteData(0x07);
  lcdWriteData(0x05);
  lcdWriteData(0x00);

  lcdWriteCommand(0xe1);
  lcdWriteData(0x00);
  lcdWriteData(0x25);
  lcdWriteData(0x27);
  lcdWriteData(0x05);
  lcdWriteData(0x10);
  lcdWriteData(0x09);
  lcdWriteData(0x3a);
  lcdWriteData(0x78);
  lcdWriteData(0x4d);
  lcdWriteData(0x05);
  lcdWriteData(0x18);
  lcdWriteData(0x0d);
  lcdWriteData(0x38);
  lcdWriteData(0x3a);
  lcdWriteData(0x1f);

  lcdWriteCommand(0x2a);
  lcdWriteData(0x00);
  lcdWriteData(0x00);
  lcdWriteData(0x00);
  lcdWriteData(0xEF);

  lcdWriteCommand(0x2b);
  lcdWriteData(0x00);
  lcdWriteData(0x00);
  lcdWriteData(0x01);
  lcdWriteData(0x3F);

  lcdWriteCommand(0xb7);
  lcdWriteData(0x07);

  lcdWriteCommand(0xb6);
  lcdWriteData(0x0a);
  lcdWriteData(0x82);
  lcdWriteData(0x27);
  lcdWriteData(0x00);

  lcdWriteCommand(0x11);
  delay(100);
  lcdWriteCommand(0x29);
  delay(100);
  lcdWriteCommand(MemoryWrite);

}
void lcdSetCursor(unsigned short x, unsigned short y) {
  lcdSetWindow(x, x, y, y);
}
void lcdSetWindow(unsigned short x0, unsigned short y0, unsigned short x1, unsigned short y1) {
  lcdWriteCommand(ColumnAddressSet);
  lcdWriteData((x0 >> 8) & 0xFF);
  lcdWriteData(x0 & 0xFF);
  lcdWriteData((y0 >> 8) & 0xFF);
  lcdWriteData(y0 & 0xFF);
  lcdWriteCommand(PageAddressSet);
  lcdWriteData((x1 >> 8) & 0xFF);
  lcdWriteData(x1 & 0xFF);
  lcdWriteData((y1 >> 8) & 0xFF);
  lcdWriteData(y1 & 0xFF);
  lcdWriteCommand(MemoryWrite);
}
void lcdHome(void){
  lcdSetWindow(0, lcdProperties.width-1, 0, lcdProperties.height-1);
}
void lcdTest(void) {
  lcdSetOrientation(LCD_ORIENTATION_PORTRAIT);

  for (int y = 0; y < 320; y++) {
    for (int x = 0; x < 240; x++) {
      if (y > 279) lcdWriteData(COLOR_WHITE);
      else if (y > 239) lcdWriteData(COLOR_BLUE);
      else if (y > 199) lcdWriteData(COLOR_GREEN);
      else if (y > 159) lcdWriteData(COLOR_CYAN);
      else if (y > 119) lcdWriteData(COLOR_RED);
      else if (y > 79) lcdWriteData(COLOR_MAGENTA);
      else if (y > 39) lcdWriteData(COLOR_YELLOW);
      else lcdWriteData(COLOR_BLACK);
    }
  }
}
void lcdFillRGB(uint16_t color) {
  lcdSetWindow(0,lcdProperties.width-1,0,lcdProperties.height-1);
  int dimensions = lcdProperties.width * lcdProperties.height;
  while(dimensions--){
    lcdWriteData(color);
  }
}
void lcdDrawPixel(uint16_t x, uint16_t y, uint16_t color) {
  lcdSetWindow(x, x, y, y);
  lcdWriteData(color);
}
void lcdDrawPixels(uint16_t x, uint16_t y, uint16_t *data, uint32_t dataLength) {
  uint32_t i = 0;
  lcdSetWindow(x,lcdProperties.width-1,y,lcdProperties.height-1);
  do {
    lcdWriteData(data[i++]);
  } while (i<dataLength);
}
void lcdDrawHLine(uint16_t x0, uint16_t x1, uint16_t y, uint16_t color) {
  // Allows for slightly better performance than setting individual pixels
  uint16_t x, pixels;
  if (x1 < x0) {
    // Switch x1 and x0
    x = x1;
    x1 = x0;
    x0 = x;
  }
  // Check limits
  if (x1 >= lcdProperties.width) {
    x1 = lcdProperties.width - 1;
  }
  if (x0 >= lcdProperties.width) {
    x0 = lcdProperties.width - 1;
  }
  lcdSetWindow(x0, x1, y, y);
  for (pixels = 0; pixels < x1 - x0 + 1; pixels++) {
    lcdWriteData(color);
  }
}
void lcdDrawVLine(uint16_t x, uint16_t y0, uint16_t y1, uint16_t color) {
  if (y1 < y0) {
    uint16_t y = y1;
    y1 = y0;
    y0 = y;
  }

  if (x >= lcdProperties.width) {
    x = lcdProperties.width - 1;
  }

  if (y0 >= lcdProperties.height) {
    y0 = lcdProperties.height - 1;
  }
  if (y1 >= lcdProperties.height) {
    y1 = lcdProperties.height - 1;
  }

  for(int line = y0; line <= y1; line++){
    lcdDrawPixel(x,line,color);
  }
}
uint16_t lcdGetPixel(uint16_t x, uint16_t y) {
  return 0;
}
void lcdSetOrientation(lcdOrientation_t value) {
  lcdOrientation = value;
  lcdWriteCommand(MemoryAccessControl);
  if (lcdOrientation == LCD_ORIENTATION_PORTRAIT) {
      lcdWriteData(lcdPortraitConfig);
      lcdProperties.width = 240;
      lcdProperties.height = 320;
  } else {
      lcdWriteData(lcdLandscapeConfig);
      lcdProperties.width = 320;
      lcdProperties.height = 240;
  }
  lcdWriteCommand(MemoryWrite);
  lcdSetWindow(0, lcdProperties.width-1, 0, lcdProperties.height-1);
}
lcdOrientation_t lcdGetOrientation(void) {
  return lcdOrientation;
}
uint16_t lcdGetWidth(void) {
  return lcdProperties.width;
}
uint16_t lcdGetHeight(void) {
  return lcdProperties.height;
}
void lcdScroll(int16_t pixels, uint16_t fillColor) {
}
uint16_t lcdGetControllerID(void) {
  return 0x9341;
}
lcdProperties_t lcdGetProperties(void) {
  return lcdProperties;
}
void lcdReset(void){
  GPIO_WriteBit(TFT_LCD_CONTROL_PORT, TFT_LCD_RD_PIN, Bit_SET);
  GPIO_WriteBit(TFT_LCD_CONTROL_PORT, TFT_LCD_WR_PIN, Bit_SET);

  GPIO_WriteBit(TFT_LCD_CONTROL_PORT, TFT_LCD_CS_PIN, Bit_SET);

  GPIO_WriteBit(TFT_LCD_CONTROL_PORT, TFT_LCD_RESET_PIN, Bit_SET);
  delay(1);
  GPIO_WriteBit(TFT_LCD_CONTROL_PORT, TFT_LCD_RESET_PIN, Bit_RESET);
  delay(5);
  GPIO_WriteBit(TFT_LCD_CONTROL_PORT, TFT_LCD_RESET_PIN, Bit_SET);
  delay(120);
}

// Write an 8 bit command to the IC driver
void lcdWriteCommand(unsigned char command){
  GPIO_WriteBit(TFT_LCD_CONTROL_PORT, TFT_LCD_DC_PIN, Bit_RESET);
  lcdWrite(command);
}

// Write an 16 bit data word to the IC driver
void lcdWriteData(unsigned short data){
  GPIO_WriteBit(TFT_LCD_CONTROL_PORT, TFT_LCD_DC_PIN, Bit_SET);
  lcdWrite(data);
}

// Places a 16 bit word on the port connected to the IC driver and pulses the write pin
void lcdWrite(unsigned short data){
  GPIO_WriteBit(TFT_LCD_CONTROL_PORT, TFT_LCD_WR_PIN, Bit_SET);

  // Write bit 0-13 (low 13 bits) to the data port
  GPIO_Write(TFT_LCD_DATA_PORT_LOW, data);

  // Retrieve the content of the existing output register, masking out the highest 3 bits
  uint16_t dataPortHighBits = GPIO_ReadOutputData(TFT_LCD_DATA_PORT_HIGH) & 0x1FFF;
  // OR in the high color bits
  dataPortHighBits |= (data & 0xE000);
  // Write bit 14-16 (high 3 bits) to the data port
  GPIO_Write(TFT_LCD_DATA_PORT_HIGH, dataPortHighBits);

  // Strobe the WRITE pin
  GPIO_WriteBit(TFT_LCD_CONTROL_PORT, TFT_LCD_WR_PIN, Bit_RESET);
  GPIO_WriteBit(TFT_LCD_CONTROL_PORT, TFT_LCD_WR_PIN, Bit_SET);
}

unsigned char lcdBuildMemoryAccessControlConfig(
                        bool rowAddressOrder,
                        bool columnAddressOrder,
                        bool rowColumnExchange,
                        bool verticalRefreshOrder,
                        bool colorOrder,
                        bool horizontalRefreshOrder){
  unsigned char value = 0;
  if(horizontalRefreshOrder) value |= 0x0004;
  if(colorOrder) value |= 0x0008;
  if(verticalRefreshOrder) value |= 0x0010;
  if(rowColumnExchange) value |= 0x0020;
  if(columnAddressOrder) value |= 0x0040;
  if(rowAddressOrder) value |= 0x0080;
  return value;
}

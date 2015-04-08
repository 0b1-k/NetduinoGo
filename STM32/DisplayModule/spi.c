#include "spi.h"
#include "string.h"
#include "projectconfig.h"
#include "GoBusModuleID.h"

#define SPI1_NSS_PIN    GPIO_Pin_4
#define SPI1_SCK_PIN    GPIO_Pin_5
#define SPI1_MISO_PIN   GPIO_Pin_6
#define SPI1_MOSI_PIN   GPIO_Pin_7

#define SPI1_PORT       GPIOA

#define SPI1_NSS_PIN_SOURCE    GPIO_PinSource4
#define SPI1_SCK_PIN_SOURCE    GPIO_PinSource5
#define SPI1_MISO_PIN_SOURCE   GPIO_PinSource6
#define SPI1_MOSI_PIN_SOURCE   GPIO_PinSource7

#define GOBUS_GPIO_SPEED  GPIO_Speed_100MHz
#define GO_BUS_IRQ        GPIO_Pin_9
#define GO_BUS_IRQ_PORT   GPIOB

#define spiRxBufferSize 1024 * 4
#define spiTxBufferSize 1024 * 4

static unsigned char spiRxBuffer[spiRxBufferSize];
static unsigned char spiTxBuffer[spiTxBufferSize];

void AssertGoBusIRQ(){
  GPIO_WriteBit(GO_BUS_IRQ_PORT, GO_BUS_IRQ, Bit_RESET);
}
void ReleaseGoBusIRQ(){
  GPIO_WriteBit(GO_BUS_IRQ_PORT, GO_BUS_IRQ, Bit_SET);
}

void spiInit(void)
{
  InitializeModuleUUID(spiTxBuffer, spiTxBufferSize);

  GPIO_InitTypeDef  GPIO_InitStructure;
  SPI_InitTypeDef   SPI_InitStructure;
  DMA_InitTypeDef   DMA_InitStructure;
  NVIC_InitTypeDef  NVIC_InitStructure;

  GPIO_StructInit(&GPIO_InitStructure);
  SPI_StructInit(&SPI_InitStructure);
  DMA_StructInit(&DMA_InitStructure);

  // Enable the peripheral clocks
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1, ENABLE);
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA2, ENABLE);

  // Configure the /GOBUS_IRQ line used to synchronize with the SPI master
  GPIO_InitStructure.GPIO_Pin = GO_BUS_IRQ;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_Speed = GOBUS_GPIO_SPEED;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_Init(GO_BUS_IRQ_PORT, &GPIO_InitStructure);

  // Set the /GOBUS_IRQ line HIGH
  ReleaseGoBusIRQ();

  // Connect SPI pins to AF5
  GPIO_PinAFConfig(GPIOA, SPI1_NSS_PIN_SOURCE, GPIO_AF_SPI1);
  GPIO_PinAFConfig(GPIOA, SPI1_SCK_PIN_SOURCE, GPIO_AF_SPI1);
  GPIO_PinAFConfig(GPIOA, SPI1_MISO_PIN_SOURCE, GPIO_AF_SPI1);
  GPIO_PinAFConfig(GPIOA, SPI1_MOSI_PIN_SOURCE, GPIO_AF_SPI1);

  // SPI SCK / MOSI / MISO pin configuration & initialization
  GPIO_InitStructure.GPIO_Pin = SPI1_SCK_PIN | SPI1_MOSI_PIN | SPI1_MISO_PIN | SPI1_NSS_PIN;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_Speed = GOBUS_GPIO_SPEED;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_DOWN;
  GPIO_Init(GPIOA, &GPIO_InitStructure);

  // SPI configuration & initialization
  SPI_I2S_DeInit(SPI1);

  SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
  SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
  SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;
  SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge;
  SPI_InitStructure.SPI_NSS = SPI_NSS_Hard;
  SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_2;
  SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
  SPI_InitStructure.SPI_CRCPolynomial = 7;
  SPI_InitStructure.SPI_Mode = SPI_Mode_Slave;
  SPI_Init(SPI1, &SPI_InitStructure);

  // DMA configuration (SPI1 RX on DMA2 Stream 0, Channel 3, see RM0033 Reference manual section 9.3.3)
  DMA_DeInit(DMA2_Stream0);

  DMA_InitStructure.DMA_Channel = DMA_Channel_3;
  DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&SPI1->DR;
  DMA_InitStructure.DMA_Memory0BaseAddr = (uint32_t)spiRxBuffer;
  DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralToMemory;
  DMA_InitStructure.DMA_BufferSize = spiRxBufferSize;
  DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
  DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
  DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
  DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
  DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
  DMA_InitStructure.DMA_Priority = DMA_Priority_VeryHigh;
  DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable;
  DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;
  DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;
  DMA_Init(DMA2_Stream0, &DMA_InitStructure); // configure the stream

  // DMA configuration (SPI1 TX on DMA2 Stream 3, Channel 3, see RM0033 Reference manual section 9.3.3)
  DMA_DeInit(DMA2_Stream3);

  DMA_InitStructure.DMA_Channel = DMA_Channel_3;
  DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&SPI1->DR;
  DMA_InitStructure.DMA_Memory0BaseAddr = (uint32_t)spiTxBuffer;
  DMA_InitStructure.DMA_DIR = DMA_DIR_MemoryToPeripheral;
  DMA_InitStructure.DMA_BufferSize = spiTxBufferSize;
  DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
  DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
  DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
  DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
  DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
  DMA_InitStructure.DMA_Priority = DMA_Priority_VeryHigh;
  DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable;
  DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;
  DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;
  DMA_Init(DMA2_Stream3, &DMA_InitStructure); // configure the stream

  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);

  // Set and enable DMA Stream 0 IRQ (Rx)
  NVIC_InitStructure.NVIC_IRQChannel = DMA2_Stream0_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);

  // Set and enable DMA Stream 3 IRQ (Tx)
  NVIC_InitStructure.NVIC_IRQChannel = DMA2_Stream3_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);

  DMA_ITConfig(DMA2_Stream0, DMA_IT_TC, ENABLE);
  DMA_ITConfig(DMA2_Stream3, DMA_IT_TC, ENABLE);

  DMA_Cmd(DMA2_Stream0, ENABLE); // Enable the stream associated with SPI1_RX
  DMA_Cmd(DMA2_Stream3, ENABLE); // Enable the stream associated with SPI1_TX

  SPI_I2S_DMACmd(SPI1, SPI_I2S_DMAReq_Rx, ENABLE); // Enable the DMA to receive SPI data
  SPI_I2S_DMACmd(SPI1, SPI_I2S_DMAReq_Tx, ENABLE); // Enable the DMA to send SPI data

  SPI_Cmd(SPI1, ENABLE); // Enable the SPI peripheral
}

extern void ProcessMessage(unsigned char* message, unsigned short messageBufferSize, unsigned char* responseData, unsigned short responseSize);

// DMA2 Stream 0 - SPI1 RX - IRQ handler
void DMA2_Stream0_IRQHandler(){
  DMA_ClearITPendingBit(DMA2_Stream0, DMA_IT_TCIF0);
  ReleaseGoBusIRQ();
  ProcessMessage(spiRxBuffer, spiRxBufferSize, spiTxBuffer, spiTxBufferSize);
  // Re-activate the SPI RX DMA
  DMA_SetCurrDataCounter(DMA2_Stream0, spiRxBufferSize);
  DMA_Cmd(DMA2_Stream0, ENABLE);
}

// DMA2 Stream 3 - SPI1 TX - IRQ handler
void DMA2_Stream3_IRQHandler(){
  DMA_ClearITPendingBit(DMA2_Stream3, DMA_IT_TCIF3);
  // Re-activate the SPI TX DMA
  DMA_SetCurrDataCounter(DMA2_Stream3, spiTxBufferSize);
  DMA_Cmd(DMA2_Stream3, ENABLE);
}

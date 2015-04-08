/*
[nwazet Open Source Software & Open Source Hardware
Authors: Fabien Royer
Software License Agreement (BSD License)

Copyright (c) 2010-2012, Nwazet, LLC. All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

* Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
* Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
* Neither the name of Nwazet, LLC. nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.
* The names '[nwazet', 'nwazet', the ASCII hazelnut in the [nwazet logo and the color of the logo are Trademarks of nwazet, LLC. and cannot be used to endorse or promote products derived from this software or any hardware designs without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/
#include "stm32f0xx_misc.h"
#include "stm32f0xx_dma.h"
#include "stm32f0xx_rcc.h"
#include "stm32f0xx_spi.h"
#include "stm32f0xx_gpio.h"

#include "peripherals.h"
#include "spi.h"
#include "string.h"
#include "GoBusModuleID.h"
#include "message.h"
#include "vt100.h"
#include <stdio.h>
#include "LedController.h"

#define spiRxBufferSize 512+64
#define spiTxBufferSize 512+64

static uint8_t spiRxBuffer[spiRxBufferSize];
static uint8_t spiTxBuffer[spiTxBufferSize];

void Dump(uint8_t* buffer, int length){
  for(int i = 0; i < length; i++){
    printf("0x%x,", buffer[i]);
    if(!(i % 16) && i){
      printf("\r\n");
    }
  }
  printf("\r\n");
}
void SpiDumpLastMessages(){
  printf("Last received message:\r\n");
  Dump(spiRxBuffer, spiRxBufferSize);
  printf("Last sent message:\r\n");
  Dump(spiTxBuffer, spiTxBufferSize);
}
void AssertGoBusIRQ(){
  GPIO_WriteBit(GO_BUS_IRQ_PORT, GO_BUS_IRQ_PIN, Bit_RESET);
}
void ReleaseGoBusIRQ(){
  GPIO_WriteBit(GO_BUS_IRQ_PORT, GO_BUS_IRQ_PIN, Bit_SET);
}
void CheckMessage(){
  if(!(SPI2->CR1 & SPI_CR1_SPE)){
    DMA_Cmd(DMA1_Channel4, DISABLE);
    DMA_Cmd(DMA1_Channel5, DISABLE);

    LedControllerSet(Digital, 7, Bit_SET);

    ProcessMessage(spiRxBuffer, spiRxBufferSize, spiTxBuffer, spiTxBufferSize);

    LedControllerSet(Digital, 7, Bit_RESET);

    DMA_SetCurrDataCounter(DMA1_Channel4, spiRxBufferSize);
    DMA_SetCurrDataCounter(DMA1_Channel5, spiTxBufferSize);

    DMA_Cmd(DMA1_Channel5, ENABLE);
    DMA_Cmd(DMA1_Channel4, ENABLE);

    SPI_Cmd(SPI2, ENABLE);

    AssertGoBusIRQ();
  }
}
void initSPI(){
  PrintNeutralMessage("SPI channel 2 init");

  InitializeModuleUUID(spiTxBuffer, spiTxBufferSize);

  SPI_InitTypeDef   SPI_InitStructure;
  DMA_InitTypeDef   DMA_InitStructure;
  NVIC_InitTypeDef  NVIC_InitStructure;

  SPI_StructInit(&SPI_InitStructure);
  DMA_StructInit(&DMA_InitStructure);

  // Set the /GOBUS_IRQ line HIGH
  ReleaseGoBusIRQ();

  // SPI configuration & initialization
  SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
  SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
  SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;
  SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge;
  SPI_InitStructure.SPI_NSS = SPI_NSS_Hard;
  SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_2;
  SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
  SPI_InitStructure.SPI_CRCPolynomial = 7;
  SPI_InitStructure.SPI_Mode = SPI_Mode_Slave;
  SPI_Init(SPI2, &SPI_InitStructure);

  // DMA configuration (SPI2 RX on DMA1, Channel 4, see STM32F05xxx Reference Manual RM0091, section 10.3.7, Table 25)
  DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&SPI2->DR;
  DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)spiRxBuffer;
  DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
  DMA_InitStructure.DMA_BufferSize = spiRxBufferSize;
  DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
  DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
  DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
  DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
  DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
  DMA_InitStructure.DMA_Priority = DMA_Priority_VeryHigh;
  DMA_Init(DMA1_Channel4, &DMA_InitStructure); // configure the stream

  // DMA configuration (SPI2 TX on DMA1, Channel 5, see STM32F05xxx Reference Manual RM0091, section 10.3.7, Table 25)
  DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&SPI2->DR;
  DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)spiTxBuffer;
  DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;
  DMA_InitStructure.DMA_BufferSize = spiTxBufferSize;
  DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
  DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
  DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
  DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
  DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
  DMA_InitStructure.DMA_Priority = DMA_Priority_VeryHigh;
  DMA_Init(DMA1_Channel5, &DMA_InitStructure); // configure the stream

  // Set and enable DMA1 Channel 4 & 5 IRQ (RX / TX)
  NVIC_InitStructure.NVIC_IRQChannel = DMA1_Channel4_5_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPriority = 1;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);

  // Enable DMA1 Transfer Complete interrupts
  DMA_ITConfig(DMA1_Channel4, DMA_IT_TC, ENABLE); // SPI2_RX

  DMA_Cmd(DMA1_Channel4, ENABLE); // Enable the DMA channel associated with SPI2_RX
  DMA_Cmd(DMA1_Channel5, ENABLE); // Enable the DMA channel associated with SPI2_TX

  SPI_I2S_DMACmd(SPI2, SPI_I2S_DMAReq_Rx, ENABLE); // Enable the DMA to receive SPI data
  SPI_I2S_DMACmd(SPI2, SPI_I2S_DMAReq_Tx, ENABLE); // Enable the DMA to send SPI data

  SPI_Cmd(SPI2, ENABLE); // Enable the SPI peripheral

  PrintSuccessMessage("SPI channel 2 ok");
}
// DMA1 Channel 4 & 5 - SPI2 RX / TX IRQ handler
void DMA1_Channel4_5_IRQHandler(){
  if ((DMA1->ISR & DMA1_IT_GL4)){
    GO_BUS_IRQ_PORT->BSRR = GO_BUS_IRQ_PIN;
    if ((DMA1->ISR & DMA1_IT_TC4 && DMA1->ISR & DMA1_IT_TC5)){
      SPI2->CR1 &= (uint16_t)~((uint16_t)SPI_CR1_SPE);
      DMA1->IFCR = DMA1_IT_GL5;
    }
    DMA1->IFCR = DMA1_IT_GL4;
  }
}

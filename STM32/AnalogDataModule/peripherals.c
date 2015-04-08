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
#include "stm32f0xx.h"
#include "stm32f0xx_adc.h"
#include "stm32f0xx_misc.h"
#include "stm32f0xx_dma.h"
#include "stm32f0xx_rcc.h"
#include "stm32f0xx_rtc.h"
#include "stm32f0xx_spi.h"
#include "stm32f0xx_i2c.h"
#include "stm32f0xx_gpio.h"
#include "stm32f0xx_tim.h"
#include "stm32f0xx_usart.h"
#include "stm32f0xx_syscfg.h"
#include "stm32f0xx_pwr.h"

#include "GoBusModuleID.h"
#include "peripherals.h"
#include "Adc.h"
#include "spi.h"
#include "LedController.h"
#include "rtc.h"
#include "i2c.h"
#include "digitalgpio.h"
#include "usart.h"
#include "diskio.h"
#include "vt100.h"
#include <stdio.h>
#include "timeouts.h"
#include "lastFunction.h"

void initDeInit(){
  ADC_DeInit(ADC1);

  DMA_DeInit(DMA1_Channel1);
  DMA_DeInit(DMA1_Channel4);
  DMA_DeInit(DMA1_Channel5);

  I2C_DeInit(I2C2);

  SPI_I2S_DeInit(SPI2);

  GPIO_DeInit(GPIOA);
  GPIO_DeInit(GPIOB);
  GPIO_DeInit(GPIOC);
  GPIO_DeInit(GPIOD);
  GPIO_DeInit(GPIOF);

  TIM_DeInit(TIM2);
  TIM_DeInit(TIM3);
  TIM_DeInit(TIM14);
  TIM_DeInit(TIM16);
  TIM_DeInit(TIM17);

  USART_DeInit(USART2);

  SYSCFG_DeInit();
  EXTI_DeInit();
}
void initClocks(){
  RCC_ClockSecuritySystemCmd(ENABLE);
  RCC_HSICmd(DISABLE);
  RCC_MCOConfig(RCC_MCOSource_NoClock);

  RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);

  RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C2, ENABLE);
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1, ENABLE);
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2, ENABLE);
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);

  RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);

  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1 , ENABLE);
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE);
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOB, ENABLE);
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOC, ENABLE);
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOD, ENABLE);
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOF, ENABLE);

  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM14, ENABLE);
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM16, ENABLE);
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM17, ENABLE);

  RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);
}

void initGpios(){
  GPIO_InitTypeDef GpioInitStruct;
  GPIO_StructInit(&GpioInitStruct);

  // Configure the GPIOs corresponding to the analog channels
  GpioInitStruct.GPIO_Pin  = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3 | GPIO_Pin_4 | GPIO_Pin_5;
  GpioInitStruct.GPIO_Mode = GPIO_Mode_AN;
  GpioInitStruct.GPIO_Speed = GPIO_Speed_Level_2;
  GpioInitStruct.GPIO_OType = GPIO_OType_PP;
  GpioInitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_Init(GPIOC, &GpioInitStruct);

  GpioInitStruct.GPIO_Pin  = GPIO_Pin_0 | GPIO_Pin_1;
  GpioInitStruct.GPIO_Mode = GPIO_Mode_AN;
  GpioInitStruct.GPIO_Speed = GPIO_Speed_Level_2;
  GpioInitStruct.GPIO_OType = GPIO_OType_PP;
  GpioInitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_Init(GPIOB, &GpioInitStruct);

  // Configure the /GOBUS_IRQ line used to synchronize with the SPI master
  GpioInitStruct.GPIO_Pin = GO_BUS_IRQ_PIN;
  GpioInitStruct.GPIO_Mode = GPIO_Mode_OUT;
  GpioInitStruct.GPIO_OType = GPIO_OType_PP;
  GpioInitStruct.GPIO_Speed = GOBUS_GPIO_SPEED;
  GpioInitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_Init(GO_BUS_IRQ_PORT, &GpioInitStruct);

  // Connect SPI2 pins to Alternate Function 0
  GPIO_PinAFConfig(SPI2_PORT, SPI2_NSS_PIN_SOURCE, GPIO_AF_0);
  GPIO_PinAFConfig(SPI2_PORT, SPI2_SCK_PIN_SOURCE, GPIO_AF_0);
  GPIO_PinAFConfig(SPI2_PORT, SPI2_MISO_PIN_SOURCE, GPIO_AF_0);
  GPIO_PinAFConfig(SPI2_PORT, SPI2_MOSI_PIN_SOURCE, GPIO_AF_0);

  // SPI2 GPIO configuration & initialization
  GpioInitStruct.GPIO_Pin = SPI2_SCK_PIN | SPI2_MOSI_PIN | SPI2_MISO_PIN | SPI2_NSS_PIN;
  GpioInitStruct.GPIO_Mode = GPIO_Mode_AF;
  GpioInitStruct.GPIO_Speed = GOBUS_GPIO_SPEED;
  GpioInitStruct.GPIO_OType = GPIO_OType_PP;
  GpioInitStruct.GPIO_PuPd  = GPIO_PuPd_DOWN;
  GPIO_Init(SPI2_PORT, &GpioInitStruct);

  // Configure the Led Controller pins driving the shift registers
  GpioInitStruct.GPIO_Pin = SR_CLEAR_PIN | SR_CLOCK_PIN;
  GpioInitStruct.GPIO_Mode = GPIO_Mode_OUT;
  GpioInitStruct.GPIO_OType = GPIO_OType_PP;
  GpioInitStruct.GPIO_Speed = SR_GPIO_SPEED;
  GpioInitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_Init(SR_CLOCK_PORT, &GpioInitStruct);

  GpioInitStruct.GPIO_Pin = SR_LATCH_PIN | SR_OUTPUT_ENABLE_PIN | SR_DATA_IN_PIN;
  GpioInitStruct.GPIO_Mode = GPIO_Mode_OUT;
  GpioInitStruct.GPIO_OType = GPIO_OType_PP;
  GpioInitStruct.GPIO_Speed = SR_GPIO_SPEED;
  GpioInitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_Init(SR_DATA_IN_PORT, &GpioInitStruct);

  // Configure the GPIO driving the I2C LED
  GpioInitStruct.GPIO_Pin = I2C2_LED;
  GpioInitStruct.GPIO_Mode = GPIO_Mode_OUT;
  GpioInitStruct.GPIO_OType = GPIO_OType_PP;
  GpioInitStruct.GPIO_Speed = I2C2_LED_GPIO_SPEED;
  GpioInitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_Init(I2C2_LED_PORT, &GpioInitStruct);

  // Configure the GPIO driving the SD LED
  GpioInitStruct.GPIO_Pin = SD_LED;
  GpioInitStruct.GPIO_Mode = GPIO_Mode_OUT;
  GpioInitStruct.GPIO_OType = GPIO_OType_PP;
  GpioInitStruct.GPIO_Speed = SD_LED_GPIO_SPEED;
  GpioInitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_Init(SD_LED_PORT, &GpioInitStruct);

  // Configure the GPIO driving the USART LED
  GpioInitStruct.GPIO_Pin = USART_LED;
  GpioInitStruct.GPIO_Mode = GPIO_Mode_OUT;
  GpioInitStruct.GPIO_OType = GPIO_OType_PP;
  GpioInitStruct.GPIO_Speed = USART_LED_GPIO_SPEED;
  GpioInitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_Init(USART_LED_PORT, &GpioInitStruct);

  // Configure I2C GPIOs
  GPIO_PinAFConfig(I2C_GPIO_PORT, I2C_SCL_PIN_SOURCE, I2C_AF);
  GPIO_PinAFConfig(I2C_GPIO_PORT, I2C_SDA_PIN_SOURCE, I2C_AF);

  GpioInitStruct.GPIO_Speed = I2C_GPIO_SPEED;
  GpioInitStruct.GPIO_Mode = GPIO_Mode_AF;
  GpioInitStruct.GPIO_OType = GPIO_OType_OD;
  GpioInitStruct.GPIO_PuPd  = GPIO_PuPd_NOPULL;
  GpioInitStruct.GPIO_Pin = I2C_SCL_PIN;
  GPIO_Init(I2C_GPIO_PORT, &GpioInitStruct);
  GpioInitStruct.GPIO_Pin = I2C_SDA_PIN;
  GPIO_Init(I2C_GPIO_PORT, &GpioInitStruct);

  // Configure the GPIOs driving USART2
  GPIO_PinAFConfig(USART_GPIO_PORT, USART_CTS_PIN_SOURCE, USART_AF);
  GPIO_PinAFConfig(USART_GPIO_PORT, USART_RTS_PIN_SOURCE, USART_AF);
  GPIO_PinAFConfig(USART_GPIO_PORT, USART_TX_PIN_SOURCE, USART_AF);
  GPIO_PinAFConfig(USART_GPIO_PORT, USART_RX_PIN_SOURCE, USART_AF);

  GpioInitStruct.GPIO_Pin = USART_TX_PIN | USART_RX_PIN | USART_RTS_PIN | USART_CTS_PIN;
  GpioInitStruct.GPIO_Mode = GPIO_Mode_AF;
  GpioInitStruct.GPIO_OType = GPIO_OType_PP;
  GpioInitStruct.GPIO_Speed = USART_GPIO_SPEED;
  GpioInitStruct.GPIO_PuPd = GPIO_PuPd_UP;
  GPIO_Init(USART_GPIO_PORT, &GpioInitStruct);

  // Configure the write-protection and card detection GPIOs used with the SD card
  GpioInitStruct.GPIO_Pin = SD_DETECT_PIN;
  GpioInitStruct.GPIO_Mode = GPIO_Mode_IN;
  GpioInitStruct.GPIO_PuPd = GPIO_PuPd_UP;
  GpioInitStruct.GPIO_Speed = SD_GPIO_SPEED;
  GPIO_Init(SD_DETECT_GPIO_PORT, &GpioInitStruct);

  GpioInitStruct.GPIO_Pin = SD_WRITE_PROTECT_PIN;
  GpioInitStruct.GPIO_Mode = GPIO_Mode_IN;
  GpioInitStruct.GPIO_PuPd = GPIO_PuPd_UP;
  GpioInitStruct.GPIO_Speed = SD_GPIO_SPEED;
  GPIO_Init(SD_WRITE_PROTECT_GPIO_PORT, &GpioInitStruct);
}
void showClockSpeeds(){
  RCC_ClocksTypeDef     clocks;
  RCC_GetClocksFreq(&clocks);
  printf("Clock freq. kHz:\r\n");
  printf("\tSYSCLK: %li\r\n", clocks.SYSCLK_Frequency);
  printf("\tHCLK: %li\r\n", clocks.HCLK_Frequency);
  printf("\tPCLK: %li\r\n", clocks.PCLK_Frequency);
  printf("\tADCCLK: %li\r\n", clocks.ADCCLK_Frequency);
  printf("\tCECCLK: %li\r\n", clocks.CECCLK_Frequency);
  printf("\tI2C1CLK: %li\r\n", clocks.I2C1CLK_Frequency);
  printf("\tUSART1CLK: %li\r\n", clocks.USART1CLK_Frequency);
}
void initPeripherals(){
  initLastFunctionNames();

  // init. the peripheral clocks
  initClocks();
  // reset the peripherals
  initDeInit();
  // GPIOs init
  initGpios();

  // LED controller init: light up the board in case of a hard fault
  initLedController();

  // USART init first to allow serial output of initialization messages
  initUSART(USART_DEFAULT_BAUD_RATE);

  printf("[nwazet DAQ module init | Rev: %s\r\n", GetMakerString());

  // show clock speeds
  showClockSpeeds();

  // Specific peripheral init
  initRTC();
  initADC();
  initSPI();
  initDigitalGpio();
  initI2C();

  PrintNeutralMessage("SysTick init (1ms freq.)");
  initTimeouts();
  if (SysTick_Config(SystemCoreClock / 1000)){
    PrintErrorMessage("SysTick init failed");
    LedControllerHardFaultDisplay();
    while (1);
  }

  RCC_ClearFlag();

  PrintSuccessMessage("SysTick init ok");
}

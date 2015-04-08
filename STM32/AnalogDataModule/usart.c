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
#include "stm32f0xx_it.h"
#include "stm32f0xx_usart.h"
#include "LedController.h"
#include "peripherals.h"
#include "usart.h"
#include <stdio.h>
#include "daqSystemConfig.h"
#include "vt100.h"
#include "timeouts.h"

#define DefaultRxTxTimeOutMs 20
#define MinRxTxTimeOutMs 1
#define MaxRxTxTimeOutMs 10000

volatile static uint16_t rxTimeOutMs = DefaultRxTxTimeOutMs;
volatile static uint16_t txTimeOutMs = DefaultRxTxTimeOutMs;
volatile static uint8_t redirectPrintfOutput = 0;

uint16_t usartGetRxTimeOutMs(){
  return txTimeOutMs;
}
void usartConfigInternal(USART_InitTypeDef* config){
  USART_Cmd(USART_PORT, DISABLE);
  USART_Init(USART_PORT, config);
  USART_Cmd(USART_PORT, ENABLE);
}
void initUSART(uint32_t baudRate){
  USART_InitTypeDef config;
  config.USART_BaudRate = baudRate;
  config.USART_WordLength = USART_WordLength_8b;
  config.USART_StopBits = USART_StopBits_1;
  config.USART_Parity = USART_Parity_No;
  config.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
  config.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
  usartConfigInternal(&config);
  usartPrintf(GetDaqSystemOption(DiagnosticsToSerial));
}
int usartTest(){
  return (USART_GetFlagStatus(USART_PORT, USART_FLAG_RXNE) == RESET ) ? 0 : 1;
}
int usartGet(){
  LedControllerSet(USART, 0, Bit_SET);
  SetTimeOut(UsartTimeOutId, rxTimeOutMs);
  while(USART_GetFlagStatus(USART_PORT, USART_FLAG_RXNE) == RESET){
    if(IsTimedOut(UsartTimeOutId)){
      LedControllerSet(USART, 0, Bit_RESET);
      return USART_TIMEOUT;
    }
  }
  LedControllerSet(USART, 0, Bit_RESET);
  return USART_ReceiveData(USART_PORT);
}
int usartPut(uint16_t d){
  LedControllerSet(USART, 0, Bit_SET);
  SetTimeOut(UsartTimeOutId, txTimeOutMs);
  while(USART_GetFlagStatus(USART_PORT, USART_FLAG_TXE) == RESET){
    if(IsTimedOut(UsartTimeOutId)){
      LedControllerSet(USART, 0, Bit_RESET);
      return USART_TIMEOUT;
    }
  }
  USART_SendData(USART_PORT, d);
  LedControllerSet(USART, 0, Bit_RESET);
  return 1;
}
int usartPuts(unsigned char* s){
  char c;
  int count = 0;
  while((c = *s++) != '\0'){
    if(usartPut((uint8_t)c) != USART_TIMEOUT){
      count++;
    }else{
      return USART_TIMEOUT;
    }
  }
  return count;
}
void usartPrintf(uint8_t enabled){
  redirectPrintfOutput = enabled;
}
// Retargets the C library printf function to the USART.
int fputc(int ch, FILE *f){
  if(redirectPrintfOutput){
    usartPut((uint8_t)ch);
  }
  return ch;
}

#define MinBaudRate 110
#define MaxBaudRate 921600

enum UsartDataBits {
  UsartDataBitsEight = 8,
  UsartDataBitsNine = 9
};

enum UsartStopBits {
  UsartStopBitsOne = 1,
  UsartStopBitsTwo = 2
};

enum UsartParity {
  UsartParityNone = 0,
  UsartParityOdd = 1,
  UsartParityEven = 2
};

enum UsartHardwareFlowControl {
  UsartHardwareFlowControlDisabled = 0,
  UsartHardwareFlowControlEnabled = 1
};

enum UsartConfigResult {
  UsartConfigSuccess,
  UsartConfigInvalidBaudRate,
  UsartConfigInvalidDataBits,
  UsartConfigInvalidStopBits,
  UsartConfigInvalidParity,
  UsartConfigInvalidHardwareFlowControl,
  UsartConfigInvalidTxTimeout,
  UsartConfigInvalidRxTimeout
};

static const char* usartResultStrings[] = {
  "OK",
  "Baud rate",
  "Data bits",
  "Stop bits",
  "Parity",
  "H/W flow ctrl",
  "Tx timeout",
  "Rx timeout"
};

uint8_t CheckUsartConfigSuccess(uint8_t result){
  if(result != UsartConfigSuccess){
    vt100TextColor(VT100_Red, VT100_Black);
    printf("USART: invalid parameter (%s)",usartResultStrings[result]);
    vt100TextDefault();
  }
  return result;
}

uint8_t usartConfig(SerializerContext* context){
  USART_InitTypeDef config;

  uint32_t baudRate = 0;
  uint8_t dataBits = 0;
  uint8_t stopBits = 0;
  uint8_t parity = 0;
  uint8_t hardwareFlowControl = 0;
  uint16_t txTimeout = 0;
  uint16_t rxTimeout = 0;

  uint8_t result = UsartConfigSuccess;

  Get(context, (void*)&baudRate, sizeof(baudRate));
  Get(context, (void*)&dataBits, sizeof(dataBits));
  Get(context, (void*)&stopBits, sizeof(stopBits));
  Get(context, (void*)&parity, sizeof(parity));
  Get(context, (void*)&hardwareFlowControl, sizeof(hardwareFlowControl));
  Get(context, (void*)&txTimeout, sizeof(txTimeout));
  Get(context, (void*)&rxTimeout, sizeof(rxTimeout));

  if(baudRate < MinBaudRate || baudRate > MaxBaudRate){
    return CheckUsartConfigSuccess(UsartConfigInvalidBaudRate);
  }

  config.USART_BaudRate = baudRate;

  switch(dataBits){
  case UsartDataBitsEight:
    config.USART_WordLength = USART_WordLength_8b;
    break;
  case UsartDataBitsNine:
    config.USART_WordLength = USART_WordLength_9b;
    break;
  default:
    return CheckUsartConfigSuccess(UsartConfigInvalidDataBits);
  }

  switch(stopBits){
  case UsartStopBitsOne:
    config.USART_StopBits = USART_StopBits_1;
    break;
  case UsartStopBitsTwo:
    config.USART_StopBits = USART_StopBits_2;
    break;
  default:
    return CheckUsartConfigSuccess(UsartConfigInvalidStopBits);
  }

  switch(parity){
  case UsartParityNone:
    config.USART_Parity = USART_Parity_No;
    break;
  case UsartParityOdd:
    config.USART_Parity = USART_Parity_Odd;
    break;
  case UsartParityEven:
    config.USART_Parity = USART_Parity_Even;
    break;
  default:
    return CheckUsartConfigSuccess(UsartConfigInvalidParity);
  }

  config.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;

  switch(hardwareFlowControl){
  case UsartHardwareFlowControlEnabled:
    config.USART_HardwareFlowControl = USART_HardwareFlowControl_RTS_CTS;
    break;
  case UsartHardwareFlowControlDisabled:
    config.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    break;
  default:
    return CheckUsartConfigSuccess(UsartConfigInvalidHardwareFlowControl);
  }

  if(txTimeout < MinRxTxTimeOutMs || txTimeout > MaxRxTxTimeOutMs){
    return CheckUsartConfigSuccess(UsartConfigInvalidTxTimeout);
  }

  txTimeOutMs = txTimeout;

  if(rxTimeout < MinRxTxTimeOutMs || rxTimeout > MaxRxTxTimeOutMs){
    return CheckUsartConfigSuccess(UsartConfigInvalidRxTimeout);
  }

  rxTimeOutMs = rxTimeout;

  usartConfigInternal(&config);

  return CheckUsartConfigSuccess(result);
}
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
#include "stm32f0xx_i2c.h"
#include "peripherals.h"
#include "i2c.h"
#include "vt100.h"
#include <stdio.h>
#include "timeouts.h"
#include "lastFunction.h"

typedef struct {
  uint8_t presc;
  uint8_t scldel;
  uint8_t sdadel;
  uint8_t sclh;
  uint8_t scll;
} I2CTiming_t;

static const char* i2cResultStrings[] = {
  "OK",
  "Nak",
  "Bus error",
  "Arbitration lost",
  "Over/Under run",
  "Timeout",
  "Invalid bus speed",
  "Invalid byte count"
};

// see STM32F05xx Reference Manual RM0091 - 23.7.5 Timing register (I2Cx_TIMINGR)
uint32_t i2cComposeTimingRegister(I2CTiming_t* timing){
  uint32_t temp = 0;
  uint32_t timingRegister = 0;

  temp = timing->presc << 4;
  temp <<= 24;
  timingRegister |= temp;

  temp = timing->scldel << 4;
  temp |= timing->sdadel;
  temp <<= 16;
  timingRegister |= temp;

  temp = timing->sclh;
  temp <<= 8;
  temp |= timing->scll;
  timingRegister |= temp;

  return timingRegister;
}

// See STM32F05xx Reference Manual RM0091 - 23.4.10 I2Cx_TIMINGR register configuration examples (10 kHz / 16MHz config.)
// Timing examples from the reference manual have been altered to generate the expected frequencies
I2CTiming_t* i2cFillTimingStructure(I2cSpeed_t speed, I2CTiming_t* timing){
  switch(speed){
  case TenkHz:
    timing->presc = 0x03;
    timing->scll = 0xc7;
    timing->sclh = 0xc3;
    timing->sdadel = 0x02;
    timing->scldel = 0x04;
    break;
  case OneHundredkHz:
    timing->presc = 0x03;
    timing->scll = 0x15;
    timing->sclh = 0x0F;
    timing->sdadel = 0x02;
    timing->scldel = 0x04;
    break;
  case FourHundredkHz:
    timing->presc = 0x01;
    timing->scll = 0x0A;
    timing->sclh = 0x03;
    timing->sdadel = 0x02;
    timing->scldel = 0x03;
    break;
  }
  return timing;
}

#define I2C_ATTENTION_EVENT     1
#define I2C_ATTENTION_NO_EVENT  0

static I2cSpeed_t i2cCurrentBusSpeed = InvalidSpeedkHz;

volatile static uint32_t i2cError = 0;
volatile static uint32_t i2cEvent = 0;
volatile static uint8_t i2cAttention = I2C_ATTENTION_NO_EVENT;

void i2cResetAttention(){
  i2cError = 0;
  i2cEvent = 0;
  i2cAttention = I2C_ATTENTION_NO_EVENT;
}
void i2cEnableInterrupts(FunctionalState state){
  NVIC_InitTypeDef NVIC_InitStructure;
  NVIC_InitStructure.NVIC_IRQChannelCmd = state;
  NVIC_InitStructure.NVIC_IRQChannel = I2C2_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPriority = 2;
  NVIC_Init(&NVIC_InitStructure);
  I2C_ITConfig(I2C_PERIPHERAL, I2C_IT_ERRI | I2C_IT_NACKI, state);
}
i2cResult_t i2cSetupBus(I2cSpeed_t speed){
  i2cResult_t result = OK;
  i2cResetAttention();
  if(speed != TenkHz && speed != OneHundredkHz && speed != FourHundredkHz){
    return InvalidBusSpeed;
  }
  if(speed != i2cCurrentBusSpeed){
    i2cCurrentBusSpeed = speed;
    I2C_InitTypeDef       I2C_InitStructure;
    I2CTiming_t           i2cTimingDefault;
    I2C_Cmd(I2C_PERIPHERAL, DISABLE);
    i2cFillTimingStructure(speed, &i2cTimingDefault);
    I2C_InitStructure.I2C_Mode = I2C_Mode_I2C;
    I2C_InitStructure.I2C_OwnAddress1 = 0;
    I2C_InitStructure.I2C_Ack = I2C_Ack_Enable;
    I2C_InitStructure.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
    I2C_InitStructure.I2C_Timing = i2cComposeTimingRegister(&i2cTimingDefault);
    I2C_InitStructure.I2C_AnalogFilter = I2C_AnalogFilter_Enable;
    I2C_InitStructure.I2C_DigitalFilter = 0;
    I2C_Init(I2C_PERIPHERAL, &I2C_InitStructure);
    I2C_Cmd(I2C_PERIPHERAL, ENABLE);
  }
  return result;
}
i2cResult_t i2cCheckResult(const char* func, i2cResult_t result){
  if(i2cAttention){
    if(i2cEvent & I2C_FLAG_NACKF){
      result = Nack;
    }else if(i2cError & I2C_FLAG_BERR){
      result = BusError;
    }else if(i2cError & I2C_FLAG_ARLO){
      result = ArbitrationLost;
    }else if(i2cError & I2C_FLAG_OVR){
      result = OverOrUnderRun;
    } else {
      result = Timeout;
    }
  }
  if(result != OK){
    vt100TextColor(VT100_Red, VT100_Black);
    printf("I2C: %s failed, reason: %s\r\n", func, i2cResultStrings[result]);
    vt100TextDefault();
  }
  return result;
}

// See STM32F051 Reference manual RM0091
// 23.7.7 Interrupt and Status register (I2Cx_ISR) and 23.8 I2C register map
#define I2C_EVENT_MASK          ((uint32_t)0x00000038)
#define I2C_ERROR_MASK          ((uint32_t)0x00003F00)
#define I2C_BUS_BUSY_MASK       ((uint32_t)0x00008000)
#define I2C_BUS_RESET_BIT       ((uint32_t)0x00002000)
#define I2C_NBYTES_MASK         ((uint32_t)0x00FF0000)
#define I2C_DEFAULT_TIMEOUT_MS  500

void I2C2_IRQHandler(){
  if((I2C_PERIPHERAL->ISR & I2C_ERROR_MASK)){ // Error?
    i2cError = I2C_PERIPHERAL->ISR & I2C_ERROR_MASK;
    I2C_PERIPHERAL->ICR |= I2C_ERROR_MASK;
  } else { // NAK?
    i2cEvent = I2C_PERIPHERAL->ISR & I2C_EVENT_MASK;
    I2C_PERIPHERAL->ICR |= I2C_IT_NACKF;
  }
  i2cAttention = I2C_ATTENTION_EVENT;
}
uint8_t i2cGetTransferBytes(){
  return (uint8_t)((I2C_PERIPHERAL->CR2 & I2C_NBYTES_MASK) >> 16);
}
i2cResult_t i2cWrite(I2cSpeed_t speed, uint16_t address, uint8_t* buffer, uint8_t count){
  SetLastFunctionName(I2cModule, __func__);
  i2cResult_t result = i2cSetupBus(speed);
  uint8_t bytesWritten = 0;
  if(result == OK){
    address <<= 1;
    I2C_MasterRequestConfig(I2C_PERIPHERAL, I2C_Direction_Transmitter);
    I2C_TransferHandling(I2C_PERIPHERAL, address, count, I2C_AutoEnd_Mode, I2C_Generate_Start_Write);
    SetTimeOut(I2cTimeOutId, I2C_DEFAULT_TIMEOUT_MS);
    while(!i2cAttention && count){
      while((I2C_PERIPHERAL->ISR & I2C_FLAG_TXE) == RESET);
      I2C_SendData(I2C_PERIPHERAL, *buffer++);
      bytesWritten++;
      count--;
      while(!i2cAttention){
        if(IsTimedOut(I2cTimeOutId)){
          result = Timeout;
          i2cAttention = I2C_ATTENTION_EVENT;
          break;
        }
        if((I2C_PERIPHERAL->ISR & I2C_FLAG_TXIS) != RESET){
          break;
        }
        if((I2C_PERIPHERAL->ISR & I2C_FLAG_STOPF) != RESET){
          I2C_PERIPHERAL->ICR = I2C_FLAG_STOPF;
          break;
        }
      }
    }
  }
  i2cCheckResult(__func__, result);
  if(count){
    vt100TextColor(VT100_Red, VT100_Black);
    printf("I2C: written %i / %i bytes\r\n", bytesWritten, i2cGetTransferBytes());
    vt100TextDefault();
  }
  return result;
}
i2cResult_t i2cRead(I2cSpeed_t speed, uint16_t address, uint8_t* buffer, uint8_t count){
  SetLastFunctionName(I2cModule, __func__);
  i2cResult_t result = i2cSetupBus(speed);
  if(result == OK){
    address <<= 1;
    I2C_MasterRequestConfig(I2C_PERIPHERAL, I2C_Direction_Receiver);
    I2C_TransferHandling(I2C_PERIPHERAL, address, count, I2C_AutoEnd_Mode, I2C_Generate_Start_Read);
    SetTimeOut(I2cTimeOutId, I2C_DEFAULT_TIMEOUT_MS);
    while(!i2cAttention && count){
      while(!i2cAttention){
        if(IsTimedOut(I2cTimeOutId)){
          result = Timeout;
          i2cAttention = I2C_ATTENTION_EVENT;
          continue;
        }
        if((I2C_PERIPHERAL->ISR & I2C_FLAG_RXNE) != RESET){
          *buffer++ = I2C_ReceiveData(I2C_PERIPHERAL);
          count--;
          break;
        }
        if((I2C_PERIPHERAL->ISR & I2C_FLAG_STOPF) != RESET){
          I2C_PERIPHERAL->ICR = I2C_FLAG_STOPF;
          break;
        }
      }
    }
  }
  i2cCheckResult(__func__, result);
  if(count){
    vt100TextColor(VT100_Red, VT100_Black);
    uint8_t bytesToRead = i2cGetTransferBytes();
    printf("I2C: read %i / %i bytes\r\n", bytesToRead - count, bytesToRead);
    vt100TextDefault();
  }
  return result;
}
void i2cBusReset(){
  SetLastFunctionName(I2cModule, __func__);
  I2C_PERIPHERAL->CR1 |= I2C_BUS_RESET_BIT;
}
uint8_t i2cIsBusBusy(){
  SetLastFunctionName(I2cModule, __func__);
  return (uint8_t)((I2C_PERIPHERAL->ISR & I2C_BUS_BUSY_MASK) >> 15);
}
void initI2C(){
  PrintNeutralMessage("I2C init");
  i2cCurrentBusSpeed = InvalidSpeedkHz;
  i2cSetupBus(OneHundredkHz);
  i2cEnableInterrupts(ENABLE);
  PrintSuccessMessage("I2C init ok");
}
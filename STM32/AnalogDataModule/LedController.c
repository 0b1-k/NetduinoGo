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
#include "LedController.h"
#include "peripherals.h"

unsigned char gpioStateBuffer[2] = {0, 0};

void initLedController(){
  GPIO_WriteBit(SR_OUTPUT_ENABLE_PORT, SR_OUTPUT_ENABLE_PIN, Bit_SET);
  GPIO_WriteBit(SR_LATCH_PORT, SR_LATCH_PIN, Bit_SET);
  GPIO_WriteBit(SR_CLEAR_PORT, SR_CLEAR_PIN, Bit_SET);
  GPIO_WriteBit(SR_DATA_IN_PORT, SR_DATA_IN_PIN, Bit_RESET);
  GPIO_WriteBit(SR_CLOCK_PORT, SR_CLOCK_PIN, Bit_RESET);
}
void PulsePin(GPIO_TypeDef* port, uint16_t pin){
  GPIO_WriteBit(port, pin, Bit_SET);
  GPIO_WriteBit(port, pin, Bit_RESET);
}
void ShiftOut(unsigned char data){
  for(int bit = 0; bit < 8; bit++){
      unsigned char bitValue = data & 0x80;
      GPIO_WriteBit(SR_DATA_IN_PORT, SR_DATA_IN_PIN, (bitValue) ? Bit_SET : Bit_RESET);
      PulsePin(SR_CLOCK_PORT, SR_CLOCK_PIN);
      data <<= 1;
  }
}
void LedControllerSet(LedType gpioType, unsigned char ledIndex, BitAction action){
  unsigned char gpioState;
  switch(gpioType){
  case Analog:
  case Digital:
    if(ledIndex == 0xff){
      gpioState = 0xff;
    }else{
      gpioState = 1;
      gpioState <<= ledIndex;
    }
    if(action){
      gpioStateBuffer[gpioType] |= gpioState;
    }else{
      gpioStateBuffer[gpioType] &= ~gpioState;
    }
    GPIO_WriteBit(SR_OUTPUT_ENABLE_PORT, SR_OUTPUT_ENABLE_PIN, Bit_SET);
    ShiftOut(gpioStateBuffer[Analog]);
    PulsePin(SR_LATCH_PORT, SR_LATCH_PIN);
    ShiftOut(gpioStateBuffer[Digital]);
    PulsePin(SR_LATCH_PORT, SR_LATCH_PIN);
    GPIO_WriteBit(SR_OUTPUT_ENABLE_PORT, SR_OUTPUT_ENABLE_PIN, Bit_RESET);
    break;
  case I2CPort:
    GPIO_WriteBit(I2C2_LED_PORT, I2C2_LED, action);
    break;
  case SD:
    GPIO_WriteBit(SD_LED_PORT, SD_LED, action);
    break;
  case USART:
    GPIO_WriteBit(USART_LED_PORT, USART_LED, action);
    break;
  }
}
void LedControllerHardFaultDisplay(){
  LedControllerSet(Analog, 0xff, Bit_SET);
  LedControllerSet(Digital, 0xff, Bit_SET);
  LedControllerSet(I2CPort, 0xff, Bit_SET);
  LedControllerSet(SD, 0xff, Bit_SET);
  LedControllerSet(USART, 0xff, Bit_SET);
}
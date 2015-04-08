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
#include "message.h"
#include "Nwazet.h"
#include "Adc.h"
#include "LedController.h"
#include "rtc.h"
#include "digitalgpio.h"
#include "fshandler.h"
#include "vt100.h"
#include <stdio.h>
#include "usarthandler.h"
#include "i2chandler.h"
#include "daqSystemConfig.h"
#include "usart.h"

static SerializerContext DAQContext;

void initMessage(){
  fsCommandHandlerReset();
  vt100TextColor(VT100_Yellow, VT100_Black);
  printf("[nwazet DAQ module ready\r\n");
  vt100TextDefault();
}
void OnSetDateTime(SerializerContext* context){
  SetRtcTime(context);
}
void OnGetDateTime(SerializerContext* context, uint8_t* responseBuffer, uint16_t responseBufferSize){
  InitializeSendBuffer(&DAQContext, responseBuffer, responseBufferSize);
  GetRtcTime(&DAQContext);
  FinalizeSendBuffer(&DAQContext);
}
void OnGetClockState(SerializerContext* context, uint8_t* responseBuffer, uint16_t responseBufferSize){
  uint8_t rtcDateTimeSetState = GetRtcDateTimeSetState();
  InitializeSendBuffer(&DAQContext, responseBuffer, responseBufferSize);
  Put(&DAQContext, (void*)&rtcDateTimeSetState, sizeof(rtcDateTimeSetState), 1);
  FinalizeSendBuffer(&DAQContext);
}
void OnDefineDigitalGpio(SerializerContext* context){
  uint8_t id = 0;
  uint8_t mode = 0;
  uint8_t type = 0;
  uint8_t resistorType = 0;
  uint8_t alternateFunction = 0;
  uint8_t state = 0;
  Get(context, (void*)&id, sizeof(id));
  Get(context, (void*)&mode, sizeof(mode));
  Get(context, (void*)&type, sizeof(type));
  Get(context, (void*)&resistorType, sizeof(resistorType));
  Get(context, (void*)&alternateFunction, sizeof(alternateFunction));
  Get(context, (void*)&state, sizeof(state));
  GpioDefinition* gpio = DigitalGpioFind((GpioId)id);
  if(gpio){
    gpio->Mode = (GpioMode)mode;
    gpio->Type = (GpioType)type;
    gpio->ResistorType = (GpioResistorType)resistorType;
    gpio->AlternateFunction = (GpioAlternateFunction)alternateFunction;
    DigitalGpioDefine(gpio);
    if(gpio->Mode == Output){
      DigitalGpioWrite(gpio->Id, (BitAction)state);
    }
    if(alternateFunction == InputInterrupt){
      uint8_t state = 0;
      uint8_t trigger = 0;
      uint32_t rollOverLimit = 0;
      Get(context, (void*)&state, sizeof(state));
      Get(context, (void*)&trigger, sizeof(trigger));
      Get(context, (void*)&rollOverLimit, sizeof(rollOverLimit));
      DigitalGpioInterruptConfig(gpio->Id, (FunctionalState)state, (EXTITrigger_TypeDef)trigger, rollOverLimit);
      if((GpioId)id == GpioD6){
        // Hack: D4/D6 must both be enabled for D6 to work, even though D4 never fires interrupts
        DigitalGpioInterruptConfig(GpioD4, (FunctionalState)state, (EXTITrigger_TypeDef)trigger, rollOverLimit);
      }
    }
  }
}
void OnUndefineDigitalGpio(SerializerContext* context){
  uint8_t id = 0;
  Get(context, (void*)&id, sizeof(id));
  GpioDefinition* gpio = DigitalGpioFind((GpioId)id);
  if(gpio){
    DigitalGpioUndefine((GpioId)id);
    if((GpioId)id == GpioD6){
      // Hack: D4/D6 must both be enabled for D6 to work, even though D4 never fires interrupts
      // D4 must also be undefined when undefining D6
      DigitalGpioUndefine(GpioD4);
    }
  }
}
void OnSetDigitalOutputState(SerializerContext* context){
  uint8_t id = 0;
  uint8_t state = 0;
  Get(context, (void*)&id, sizeof(id));
  Get(context, (void*)&state, sizeof(state));
  DigitalGpioWrite((GpioId)id, (BitAction)state);
}
void OnPulseDigitalOutputState(SerializerContext* context){
  uint8_t id = 0;
  uint8_t state = 0;
  Get(context, (void*)&id, sizeof(id));
  Get(context, (void*)&state, sizeof(state));
  DigitalGpioPulse((GpioId)id, (BitAction)state);
}
void OnSetPwmOutputDutyCycle(SerializerContext* context){
  uint8_t id = 0;
  unsigned long pulseLength = 0;
  Get(context, (void*)&id, sizeof(id));
  Get(context, (void*)&pulseLength, sizeof(pulseLength));
  DigitalGpioPwmDutyCycleSet((GpioId)id, pulseLength);
}
void OnGetDigitalInputState(SerializerContext* context, uint8_t* responseBuffer, uint16_t responseBufferSize){
  uint8_t id = 0;
  Get(context, (void*)&id, sizeof(id));
  InitializeSendBuffer(&DAQContext, responseBuffer, responseBufferSize);
  if((GpioId)id == GpioCount){
    DigitalGpioReadAll(&DAQContext);
  }else{
    uint8_t state = DigitalGpioRead((GpioId)id);
    Put(&DAQContext, (void*)&state, sizeof(state), 1);
  }
  FinalizeSendBuffer(&DAQContext);
}
void OnSetLedState(SerializerContext* context){
  uint8_t ledType = 0;
  uint8_t ledIndex = 0;
  uint8_t ledState = 0;
  Get(context, (void*)&ledType, sizeof(ledType));
  Get(context, (void*)&ledIndex, sizeof(ledIndex));
  Get(context, (void*)&ledState, sizeof(ledState));
  LedControllerSet((LedType)ledType, ledIndex, (BitAction)ledState);
}
void OnGetAdcSample(SerializerContext* context, uint8_t* responseBuffer, uint16_t responseBufferSize){
  InitializeSendBuffer(&DAQContext, responseBuffer, responseBufferSize);
  GetRtcTime(&DAQContext);
  StartAdcConversion(&DAQContext);
  FinalizeSendBuffer(&DAQContext);
}
void OnGetBackupRegisters(SerializerContext* context, uint8_t* responseBuffer, uint16_t responseBufferSize){
  InitializeSendBuffer(&DAQContext, responseBuffer, responseBufferSize);
  GetRtcBackupRegisters(&DAQContext);
  FinalizeSendBuffer(&DAQContext);
}
void OnSetBackupRegisters(SerializerContext* context){
  SetRtcBackupRegisters(context);
  usartPrintf(GetDaqSystemOption(DiagnosticsToSerial));
}
void OnSetPwmTimeBase(SerializerContext* context){
  uint8_t timerId = 0;
  uint16_t period = 0;
  uint16_t prescaler = 0;
  Get(context, (void*)&timerId, sizeof(timerId));
  Get(context, (void*)&period, sizeof(period));
  Get(context, (void*)&prescaler, sizeof(prescaler));
  DigitalGpioTimeBaseDefine((GpioId)timerId, period, prescaler);
}
void OnGetInterruptCounters(SerializerContext* context, uint8_t* responseBuffer, uint16_t responseBufferSize){
  uint8_t reset = 0;
  Get(context, (void*)&reset, sizeof(reset));
  InitializeSendBuffer(&DAQContext, responseBuffer, responseBufferSize);
  DigitalGpioGetInterruptCounters(&DAQContext, reset);
  FinalizeSendBuffer(&DAQContext);
}
enum Command {
  SetDateTime,
  GetDateTime,
  GetClockState,
  DefineDigitalGpio,
  SetDigitalOutputState,
  PulseDigitalOutputState,
  GetDigitalInputState,
  SetPwmOutputDutyCycle,
  SetLedState,
  GetAdcSample,
  Reboot,
  FileSystem,
  GetBackupRegisters,
  SetBackupRegisters,
  Usart,
  I2c,
  SetPwmTimeBase,
  UndefineDigitalGpio,
  GetInterruptCounters
};

static SerializerContext context;

void ProcessMessage(uint8_t* message, uint16_t messageBufferSize, uint8_t* responseData, uint16_t responseSize) {
  if(InitializeReceiveBuffer(&context, message, messageBufferSize) == 0) return;
  if(!context.IsValidPacket) return;
  uint8_t command = 0;
  while(MoreData(&context)){
    Get(&context, (void*)(&command),sizeof(command));
    switch (command) {
    case SetDateTime:
      OnSetDateTime(&context);
      break;
    case GetDateTime:
      OnGetDateTime(&context, responseData, responseSize);
      break;
    case GetClockState:
      OnGetClockState(&context, responseData, responseSize);
      break;
    case DefineDigitalGpio:
      OnDefineDigitalGpio(&context);
      break;
    case SetDigitalOutputState:
      OnSetDigitalOutputState(&context);
      break;
    case PulseDigitalOutputState:
      OnPulseDigitalOutputState(&context);
      break;
    case GetDigitalInputState:
      OnGetDigitalInputState(&context, responseData, responseSize);
      break;
    case SetPwmOutputDutyCycle:
      OnSetPwmOutputDutyCycle(&context);
      break;
    case SetLedState:
      OnSetLedState(&context);
      break;
    case GetAdcSample:
      OnGetAdcSample(&context, responseData, responseSize);
      break;
    case Reboot:
      NVIC_SystemReset();
      break;
    case FileSystem:
      OnFileSystemCommand(&context, &DAQContext, responseData, responseSize);
      Terminate(&context);
      break;
    case GetBackupRegisters:
      OnGetBackupRegisters(&context, responseData, responseSize);
      break;
    case SetBackupRegisters:
      OnSetBackupRegisters(&context);
      break;
    case Usart:
      OnUsartCommand(&context, &DAQContext, responseData, responseSize);
      Terminate(&context);
      break;
    case I2c:
      OnI2cCommand(&context, &DAQContext, responseData, responseSize);
      Terminate(&context);
      break;
    case SetPwmTimeBase:
      OnSetPwmTimeBase(&context);
      break;
    case UndefineDigitalGpio:
      OnUndefineDigitalGpio(&context);
      break;
    case GetInterruptCounters:
      OnGetInterruptCounters(&context, responseData, responseSize);
      break;
    }
  }
}
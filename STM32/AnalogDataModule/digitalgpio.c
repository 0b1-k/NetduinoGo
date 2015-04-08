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
#include "stm32f0xx_syscfg.h"
#include "stm32f0xx_misc.h"
#include "peripherals.h"
#include "digitalgpio.h"
#include "vt100.h"
#include <stdio.h>

static GpioDefinition GpioDefinitions[GpioCount];
static volatile uint32_t InterruptCounters[GpioCount];

#define TimerCoreClockRatio     (uint32_t)3     // SystemCoreClock / HSE_VALUE;
#define DefaultFrequencykHz     (uint32_t)1000  // All timers are set to 1 kHz by default
#define DefaultPrescaler        (uint16_t)0     // No division of the timer's frequency by default

void GpioIdOutOfRange(const char* function, GpioId id){
  vt100TextColor(VT100_Red, VT100_Black);
  printf("%s: GpioId %i out of range\r\n", function, id);
  vt100TextDefault();
}
void GpioFunctionNotSupported(const char* function, const char* message, GpioId id){
  vt100TextColor(VT100_Red, VT100_Black);
  printf("%s: %s is unsupported (GpioId %i)\r\n", function, message, id);
  vt100TextDefault();
}

#define INTERRUPT_COUNTER_DEFAULT_ROLLOVER_LIMIT        (uint32_t)0xFFFFFFFF

void DigitalGpioResetInterruptCounter(GpioId id){
  GpioDefinitions[id].InterruptCounter.Count = 0;
  GpioDefinitions[id].InterruptCounter.RollOverCount = 0;
  GpioDefinitions[id].InterruptCounter.Updated = 0;
}
void InitializeGpioDefaults(GpioId id){
  GpioDefinitions[id].Mode = Output;
  GpioDefinitions[id].Type = PushPull;
  GpioDefinitions[id].ResistorType = (GpioResistorType)GPIO_PuPd_NOPULL;
  GpioDefinitions[id].AlternateFunction = GpioAlternateFunctionNone;
  DigitalGpioDefine(&GpioDefinitions[id]);
  DigitalGpioWrite(id, Bit_RESET);

  DigitalGpioTimeBaseDefine(id, (SystemCoreClock / (DefaultFrequencykHz * TimerCoreClockRatio) - 1), DefaultPrescaler);

  DigitalGpioInterruptConfig(id, DISABLE, EXTI_Trigger_Rising_Falling, INTERRUPT_COUNTER_DEFAULT_ROLLOVER_LIMIT);
  DigitalGpioResetInterruptCounter(id);
}
void initDigitalGpio(){
  PrintNeutralMessage("Digital GPIO init");

  GpioDefinitions[GpioD0].Id = GpioD0;
  GpioDefinitions[GpioD0].Port = D0_PORT;
  GpioDefinitions[GpioD0].Pin = D0_PIN;
  GpioDefinitions[GpioD0].Timer = D0_TIMER;
  GpioDefinitions[GpioD0].TimerChannel = D0_TIMER_CHANNEL;
  GpioDefinitions[GpioD0].TimerAlternateFunctionPinSource = D0_TIMER_PIN_SOURCE;
  GpioDefinitions[GpioD0].TimerAlternateFunctionNumber = D0_TIMER_ALT_FUNCTION;
  GpioDefinitions[GpioD0].ExtiPortSource = D0_EXTI_PORT_SOURCE;
  GpioDefinitions[GpioD0].ExtiPinSource = D0_EXTI_PIN_SOURCE;
  GpioDefinitions[GpioD0].ExtiLine = D0_EXTI_LINE;

  GpioDefinitions[GpioD1].Id = GpioD1;
  GpioDefinitions[GpioD1].Port = D1_PORT;
  GpioDefinitions[GpioD1].Pin = D1_PIN;
  GpioDefinitions[GpioD1].Timer = D1_TIMER;
  GpioDefinitions[GpioD1].TimerChannel = D1_TIMER_CHANNEL;
  GpioDefinitions[GpioD1].TimerAlternateFunctionPinSource = D1_TIMER_PIN_SOURCE;
  GpioDefinitions[GpioD1].TimerAlternateFunctionNumber = D1_TIMER_ALT_FUNCTION;
  GpioDefinitions[GpioD1].ExtiPortSource = D1_EXTI_PORT_SOURCE;
  GpioDefinitions[GpioD1].ExtiPinSource = D1_EXTI_PIN_SOURCE;
  GpioDefinitions[GpioD1].ExtiLine = D1_EXTI_LINE;

  GpioDefinitions[GpioD2].Id = GpioD2;
  GpioDefinitions[GpioD2].Port = D2_PORT;
  GpioDefinitions[GpioD2].Pin = D2_PIN;
  GpioDefinitions[GpioD2].Timer = D2_TIMER;
  GpioDefinitions[GpioD2].TimerChannel = D2_TIMER_CHANNEL;
  GpioDefinitions[GpioD2].TimerAlternateFunctionPinSource = D2_TIMER_PIN_SOURCE;
  GpioDefinitions[GpioD2].TimerAlternateFunctionNumber = D2_TIMER_ALT_FUNCTION;
  GpioDefinitions[GpioD2].ExtiPortSource = D2_EXTI_PORT_SOURCE;
  GpioDefinitions[GpioD2].ExtiPinSource = D2_EXTI_PIN_SOURCE;
  GpioDefinitions[GpioD2].ExtiLine = D2_EXTI_LINE;

  GpioDefinitions[GpioD3].Id = GpioD3;
  GpioDefinitions[GpioD3].Port = D3_PORT;
  GpioDefinitions[GpioD3].Pin = D3_PIN;
  GpioDefinitions[GpioD3].Timer = D3_TIMER;
  GpioDefinitions[GpioD3].TimerChannel = D3_TIMER_CHANNEL;
  GpioDefinitions[GpioD3].TimerAlternateFunctionPinSource = D3_TIMER_PIN_SOURCE;
  GpioDefinitions[GpioD3].TimerAlternateFunctionNumber = D3_TIMER_ALT_FUNCTION;
  GpioDefinitions[GpioD3].ExtiPortSource = D3_EXTI_PORT_SOURCE;
  GpioDefinitions[GpioD3].ExtiPinSource = D3_EXTI_PIN_SOURCE;
  GpioDefinitions[GpioD3].ExtiLine = D3_EXTI_LINE;

  GpioDefinitions[GpioD4].Id = GpioD4;
  GpioDefinitions[GpioD4].Port = D4_PORT;
  GpioDefinitions[GpioD4].Pin = D4_PIN;
  GpioDefinitions[GpioD4].Timer = D4_TIMER;
  GpioDefinitions[GpioD4].TimerChannel = D4_TIMER_CHANNEL;
  GpioDefinitions[GpioD4].TimerAlternateFunctionPinSource = D4_TIMER_PIN_SOURCE;
  GpioDefinitions[GpioD4].TimerAlternateFunctionNumber = D4_TIMER_ALT_FUNCTION;
  GpioDefinitions[GpioD4].ExtiPortSource = D4_EXTI_PORT_SOURCE;
  GpioDefinitions[GpioD4].ExtiPinSource = D4_EXTI_PIN_SOURCE;
  GpioDefinitions[GpioD4].ExtiLine = D4_EXTI_LINE;

  GpioDefinitions[GpioD5].Id = GpioD5;
  GpioDefinitions[GpioD5].Port = D5_PORT;
  GpioDefinitions[GpioD5].Pin = D5_PIN;
  GpioDefinitions[GpioD5].Timer = D5_TIMER;
  GpioDefinitions[GpioD5].TimerChannel = D5_TIMER_CHANNEL;
  GpioDefinitions[GpioD5].TimerAlternateFunctionPinSource = D5_TIMER_PIN_SOURCE;
  GpioDefinitions[GpioD5].TimerAlternateFunctionNumber = D5_TIMER_ALT_FUNCTION;
  GpioDefinitions[GpioD5].ExtiPortSource = D5_EXTI_PORT_SOURCE;
  GpioDefinitions[GpioD5].ExtiPinSource = D5_EXTI_PIN_SOURCE;
  GpioDefinitions[GpioD5].ExtiLine = D5_EXTI_LINE;

  GpioDefinitions[GpioD6].Id = GpioD6;
  GpioDefinitions[GpioD6].Port = D6_PORT;
  GpioDefinitions[GpioD6].Pin = D6_PIN;
  GpioDefinitions[GpioD6].Timer = D6_TIMER;
  GpioDefinitions[GpioD6].TimerChannel = D6_TIMER_CHANNEL;
  GpioDefinitions[GpioD6].TimerAlternateFunctionPinSource = D6_TIMER_PIN_SOURCE;
  GpioDefinitions[GpioD6].TimerAlternateFunctionNumber = D6_TIMER_ALT_FUNCTION;
  GpioDefinitions[GpioD6].ExtiPortSource = D6_EXTI_PORT_SOURCE;
  GpioDefinitions[GpioD6].ExtiPinSource = D6_EXTI_PIN_SOURCE;
  GpioDefinitions[GpioD6].ExtiLine = D6_EXTI_LINE;

  GpioDefinitions[GpioD7].Id = GpioD7;
  GpioDefinitions[GpioD7].Port = D7_PORT;
  GpioDefinitions[GpioD7].Pin = D7_PIN;
  GpioDefinitions[GpioD7].Timer = D7_TIMER;
  GpioDefinitions[GpioD7].TimerChannel = D7_TIMER_CHANNEL;
  GpioDefinitions[GpioD7].TimerAlternateFunctionPinSource = D7_TIMER_PIN_SOURCE;
  GpioDefinitions[GpioD7].TimerAlternateFunctionNumber = D7_TIMER_ALT_FUNCTION;
  GpioDefinitions[GpioD7].ExtiPortSource = D7_EXTI_PORT_SOURCE;
  GpioDefinitions[GpioD7].ExtiPinSource = D7_EXTI_PIN_SOURCE;
  GpioDefinitions[GpioD7].ExtiLine = D7_EXTI_LINE;

  for(GpioId i = (GpioId)0; i < GpioCount; i++){
    InitializeGpioDefaults(i);
  }

  NVIC_InitTypeDef   NVIC_InitStructure;
  NVIC_InitStructure.NVIC_IRQChannel = EXTI4_15_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPriority = 0x01;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);

  NVIC_InitStructure.NVIC_IRQChannel = EXTI2_3_IRQn;
  NVIC_Init(&NVIC_InitStructure);

  NVIC_InitStructure.NVIC_IRQChannel = EXTI0_1_IRQn;
  NVIC_Init(&NVIC_InitStructure);

  PrintSuccessMessage("Digital GPIO init ok");
}
void DigitalGpioInterruptConfig(GpioId id, FunctionalState state, EXTITrigger_TypeDef trigger, uint32_t rollOverLimit){
  if(id >= GpioCount){
    GpioIdOutOfRange(__func__, id);
    return;
  }
  EXTI_InitTypeDef   EXTI_InitStructure;
  GpioDefinitions[id].InterruptCounter.RollOverLimit = rollOverLimit;
  EXTI_InitStructure.EXTI_Line = GpioDefinitions[id].ExtiLine;
  EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
  EXTI_InitStructure.EXTI_Trigger = trigger;
  EXTI_InitStructure.EXTI_LineCmd = state;
  EXTI_Init(&EXTI_InitStructure);
}
void DigitalGpioDefine(GpioDefinition* gpioDef){
  GPIO_InitTypeDef GpioInitStruct;
  GPIO_StructInit(&GpioInitStruct);

  GpioInitStruct.GPIO_Pin  = gpioDef->Pin;
  GpioInitStruct.GPIO_Speed = GPIO_Speed_Level_3; // 50 MHz
  GpioInitStruct.GPIO_OType = (GPIOOType_TypeDef) gpioDef->Type;
  GpioInitStruct.GPIO_PuPd = (GPIOPuPd_TypeDef)gpioDef->ResistorType;

  switch((GPIOMode_TypeDef)gpioDef->AlternateFunction){
  case GpioAlternateFunctionNone:
  case InputInterrupt:
    SYSCFG_EXTILineConfig(GpioDefinitions[gpioDef->Id].ExtiPortSource, GpioDefinitions[gpioDef->Id].ExtiPinSource);
    GpioInitStruct.GPIO_Mode = (GPIOMode_TypeDef)gpioDef->Mode;
    break;
  case InputCaptureMode:
    GpioFunctionNotSupported(__func__, "InputCaptureMode", gpioDef->Id);
    return;
    break;
  case InputEncoderMode:
    GpioFunctionNotSupported(__func__, "InputEncoderMode", gpioDef->Id);
    return;
    break;
  case InputPwmMode:
    GpioFunctionNotSupported(__func__, "InputPwmMode", gpioDef->Id);
    return;
    break;
  case OutputPwmMode:
    GpioInitStruct.GPIO_Mode = GPIO_Mode_AF;
    break;
  default:
    GpioFunctionNotSupported(__func__, "Unknown mode", gpioDef->Id);
    return;
  }

  GPIO_Init(gpioDef->Port, &GpioInitStruct);

  if(gpioDef->AlternateFunction == OutputPwmMode){
    GPIO_PinAFConfig(gpioDef->Port, gpioDef->TimerAlternateFunctionPinSource, gpioDef->TimerAlternateFunctionNumber);
  }
}
void DigitalGpioUndefine(GpioId id){
  // cleanup any previous gpio state before re-definition
  switch(GpioDefinitions[id].AlternateFunction){
  case OutputPwmMode:
    DigitalGpioPwmDutyCycleSet(id, 0);
    break;
  case InputInterrupt:
    DigitalGpioInterruptConfig(id, DISABLE, EXTI_Trigger_Rising_Falling, INTERRUPT_COUNTER_DEFAULT_ROLLOVER_LIMIT);
    DigitalGpioResetInterruptCounter(id);
    break;
  }
  InitializeGpioDefaults(id);
}
void DigitalGpioWrite(GpioId id, BitAction action){
  if(id < GpioCount){
    GPIO_WriteBit(GpioDefinitions[id].Port, GpioDefinitions[id].Pin, action);
  }
}
void DigitalGpioPulse(GpioId id, BitAction action){
  if(id < GpioCount){
    GPIO_WriteBit(GpioDefinitions[id].Port, GpioDefinitions[id].Pin, action);
    GPIO_WriteBit(GpioDefinitions[id].Port, GpioDefinitions[id].Pin, (BitAction)!action);
  }
}
uint8_t DigitalGpioRead(GpioId id){
  if(id < GpioCount && GpioDefinitions[id].Mode == Input){
    return GPIO_ReadInputDataBit(GpioDefinitions[id].Port, GpioDefinitions[id].Pin);
  }
  return 255;
}
void DigitalGpioReadAll(SerializerContext* context){
  for(GpioId id = GpioD0; id < GpioCount; id++){
    if(GpioDefinitions[id].Mode == Input){
      Put(context, (void*)&id, sizeof(uint8_t), 1);
      uint8_t state = GPIO_ReadInputDataBit(GpioDefinitions[id].Port, GpioDefinitions[id].Pin);
      Put(context, (void*)&state, sizeof(state), 1);
    }
  }
  GpioId endMarker = GpioCount;
  Put(context, (void*)&endMarker, sizeof(uint8_t), 1);
}
GpioDefinition* DigitalGpioFind(GpioId id){
  if(id < GpioCount){
    return &GpioDefinitions[id];
  }
  return 0;
}
void DigitalGpioTimeBaseDefine(GpioId id, uint16_t period, uint16_t prescaler){
  if(id >= GpioCount){
    GpioIdOutOfRange(__func__, id);
    return;
  }
  TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
  TIM_TimeBaseStructure.TIM_Prescaler = prescaler;
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
  TIM_TimeBaseStructure.TIM_Period = period;
  TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
  TIM_TimeBaseStructure.TIM_RepetitionCounter = 0;
  TIM_Cmd(GpioDefinitions[id].Timer, DISABLE);
  TIM_TimeBaseInit(GpioDefinitions[id].Timer, &TIM_TimeBaseStructure);
  TIM_Cmd(GpioDefinitions[id].Timer, ENABLE);
}
void DigitalGpioPwmDutyCycleSet(GpioId id, uint32_t pulseLength){
  if(id >= GpioCount){
    GpioIdOutOfRange(__func__, id);
    return;
  }
  TIM_OCInitTypeDef  TIM_OCInitStructure;
  TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM2;
  TIM_OCInitStructure.TIM_OutputState = (pulseLength == 0) ? TIM_OutputState_Disable : TIM_OutputState_Enable;
  TIM_OCInitStructure.TIM_Pulse = pulseLength;
  TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low;

  if(GpioDefinitions[id].Timer == TIM1 ||
     GpioDefinitions[id].Timer == TIM15 ||
     GpioDefinitions[id].Timer == TIM16 ||
     GpioDefinitions[id].Timer == TIM17
     ){
    TIM_OCInitStructure.TIM_OCIdleState = TIM_OCIdleState_Reset;
  }
  switch(GpioDefinitions[id].TimerChannel){
  case TIM_Channel_1:
    TIM_OC1Init(GpioDefinitions[id].Timer, &TIM_OCInitStructure);
    break;
  case TIM_Channel_2:
    TIM_OC2Init(GpioDefinitions[id].Timer, &TIM_OCInitStructure);
    break;
  case TIM_Channel_3:
    TIM_OC3Init(GpioDefinitions[id].Timer, &TIM_OCInitStructure);
    break;
  case TIM_Channel_4:
    TIM_OC4Init(GpioDefinitions[id].Timer, &TIM_OCInitStructure);
    break;
  }
  if(GpioDefinitions[id].Timer == TIM1 ||
     GpioDefinitions[id].Timer == TIM15 ||
     GpioDefinitions[id].Timer == TIM16 ||
     GpioDefinitions[id].Timer == TIM17
     ){
       TIM_CtrlPWMOutputs(GpioDefinitions[id].Timer, (pulseLength == 0) ? DISABLE : ENABLE);
  }
}
void ResolveInterrupt(){
  for(GpioId id = (GpioId)0; id < GpioCount; id++){
    if(EXTI->PR & GpioDefinitions[id].Pin){
      EXTI->PR = GpioDefinitions[id].Pin;
      GpioDefinitions[id].InterruptCounter.Count++;
      GpioDefinitions[id].InterruptCounter.Updated = 1;
      if(GpioDefinitions[id].InterruptCounter.Count >= GpioDefinitions[id].InterruptCounter.RollOverLimit){
        GpioDefinitions[id].InterruptCounter.RollOverCount++;
        GpioDefinitions[id].InterruptCounter.Count = 0;
      }
      return;
    }
  }
}
void EXTI0_1_IRQHandler(){
  if(EXTI->PR & GPIO_Pin_0){
    EXTI->PR = GPIO_Pin_0; // not a DAQ-controlled interrupt, clear it.
    return;
  }
  if(EXTI->PR & GPIO_Pin_1){
    EXTI->PR = GPIO_Pin_1; // not a DAQ-controlled interrupt, clear it.
  }
}
void EXTI2_3_IRQHandler(){
  if(EXTI->PR & GPIO_Pin_2){
    EXTI->PR = GPIO_Pin_2; // not a DAQ-controlled interrupt, clear it.
    return;
  }
  ResolveInterrupt();
}
void EXTI4_15_IRQHandler(){
  ResolveInterrupt();
}
void DigitalGpioGetInterruptCounters(SerializerContext* context, uint8_t reset){
  uint8_t count = GpioCount;
  Put(context, (void*)&count, sizeof(count), 1);

  // Hack: copy the interrupt counter values of GpioD4 into GpioD6
  GpioDefinitions[GpioD6].InterruptCounter.Count = GpioDefinitions[GpioD4].InterruptCounter.Count;
  GpioDefinitions[GpioD6].InterruptCounter.RollOverCount = GpioDefinitions[GpioD4].InterruptCounter.RollOverCount;
  GpioDefinitions[GpioD6].InterruptCounter.Updated = GpioDefinitions[GpioD4].InterruptCounter.Updated;
  // Ensure that D4 doesn't show up as updated
  GpioDefinitions[GpioD4].InterruptCounter.Updated = 0;

  for(GpioId id = (GpioId)0; id < GpioCount; id++){
    Put(context, (void*)&GpioDefinitions[id].InterruptCounter.Count, sizeof(uint32_t), 1);
    Put(context, (void*)&GpioDefinitions[id].InterruptCounter.RollOverCount, sizeof(uint32_t), 1);
    Put(context, (void*)&GpioDefinitions[id].InterruptCounter.Updated, sizeof(uint16_t), 1);
    if(reset){
      DigitalGpioResetInterruptCounter(id);
    }else{
      GpioDefinitions[id].InterruptCounter.Updated = 0;
    }
  }
}

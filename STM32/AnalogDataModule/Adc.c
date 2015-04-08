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
#include "stm32f0xx_dma.h"
#include "peripherals.h"
#include "Adc.h"
#include "vt100.h"

#define Adc1BaseAddress 0x40012440

#define ADC_DEFAULT_SAMPLING_RATE       ADC_SampleTime_41_5Cycles

typedef enum {
  RawVBat,
  RawA6,
  RawA7,
  RawA0,
  RawA1,
  RawA2,
  RawA3,
  RawA4,
  RawA5,
  RawVRef,
} RawAnalogChannelMap_t;

static volatile unsigned short RawAnalogSamples[AnalogChannelsCount];
static volatile unsigned short AnalogChannels[AnalogChannelsCount];

void initADC() {

  PrintNeutralMessage("ADC init");

  ADC_InitTypeDef     ADC_InitStructure;
  DMA_InitTypeDef     DMA_InitStructure;

  ADC_StructInit(&ADC_InitStructure);
  DMA_StructInit(&DMA_InitStructure);

  // Configure DMA1 Channel1
  DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)Adc1BaseAddress;
  DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)RawAnalogSamples;
  DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
  DMA_InitStructure.DMA_BufferSize = AnalogChannelsCount;
  DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
  DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
  DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
  DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
  DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
  DMA_InitStructure.DMA_Priority = DMA_Priority_High;
  DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
  DMA_Init(DMA1_Channel1, &DMA_InitStructure);

  DMA_Cmd(DMA1_Channel1, ENABLE);

  ADC_DMARequestModeConfig(ADC1, ADC_DMAMode_Circular);

  ADC_DMACmd(ADC1, ENABLE);

  ADC_InitStructure.ADC_Resolution = ADC_Resolution_12b;
  ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;
  ADC_InitStructure.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_None;
  ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
  ADC_InitStructure.ADC_ScanDirection = ADC_ScanDirection_Upward;
  ADC_Init(ADC1, &ADC_InitStructure);

  ADC_ChannelConfig(ADC1, ADC_Channel_8, ADC_DEFAULT_SAMPLING_RATE);
  ADC_ChannelConfig(ADC1, ADC_Channel_9, ADC_DEFAULT_SAMPLING_RATE);
  ADC_ChannelConfig(ADC1, ADC_Channel_10, ADC_DEFAULT_SAMPLING_RATE);
  ADC_ChannelConfig(ADC1, ADC_Channel_11, ADC_DEFAULT_SAMPLING_RATE);
  ADC_ChannelConfig(ADC1, ADC_Channel_12, ADC_DEFAULT_SAMPLING_RATE);
  ADC_ChannelConfig(ADC1, ADC_Channel_13, ADC_DEFAULT_SAMPLING_RATE);
  ADC_ChannelConfig(ADC1, ADC_Channel_14, ADC_DEFAULT_SAMPLING_RATE);
  ADC_ChannelConfig(ADC1, ADC_Channel_15, ADC_DEFAULT_SAMPLING_RATE);

  ADC_ChannelConfig(ADC1, ADC_Channel_Vrefint, ADC_DEFAULT_SAMPLING_RATE); // ADC_Channel_17: Vref
  ADC_VrefintCmd(ENABLE);

  ADC_ChannelConfig(ADC1, ADC_Channel_Vbat, ADC_DEFAULT_SAMPLING_RATE); // ADC_Channel_18: Vbat
  ADC_VbatCmd(ENABLE);

  ADC_GetCalibrationFactor(ADC1);
  ADC_Cmd(ADC1, ENABLE);
  while(!ADC_GetFlagStatus(ADC1, ADC_FLAG_ADEN));
  ADC_StartOfConversion(ADC1);

  PrintSuccessMessage("ADC init ok");
}
uint16_t* GetAnalogChannels(){
  // Resolve the fucked up mess created by the ADC DMA transfer
  AnalogChannels[A0] = RawAnalogSamples[RawA0];
  AnalogChannels[A1] = RawAnalogSamples[RawA1];
  AnalogChannels[A2] = RawAnalogSamples[RawA2];
  AnalogChannels[A3] = RawAnalogSamples[RawA3];
  AnalogChannels[A4] = RawAnalogSamples[RawA4];
  AnalogChannels[A5] = RawAnalogSamples[RawA5];
  AnalogChannels[A6] = RawAnalogSamples[RawA6];
  AnalogChannels[A7] = RawAnalogSamples[RawA7];
  AnalogChannels[VRef] = RawAnalogSamples[RawVRef];
  AnalogChannels[VBat] = RawAnalogSamples[RawVBat] * 2;

  return (uint16_t*)&AnalogChannels[0];
}
void StartAdcConversion(SerializerContext* context){
  Put(context, (void*)GetAnalogChannels(), sizeof(uint16_t), AnalogChannelsCount);
}

#include "adc.h"

#define adcChannelSamplingCycles  ADC_SampleTime_3Cycles

GPIO_InitTypeDef GPIO_InitStructure;

void adcInit (void){
  ADC_InitTypeDef ADC_InitStructure;
  ADC_CommonInitTypeDef ADC_CommonInitStructure;

  ADC_StructInit(&ADC_InitStructure);
  GPIO_StructInit(&GPIO_InitStructure);

  // Start the clocks on the required peripherals
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);

  ADC_DeInit();

  // ADC Common Init
  ADC_CommonInitStructure.ADC_Mode = ADC_Mode_Independent;
  ADC_CommonInitStructure.ADC_Prescaler = ADC_Prescaler_Div2;
  ADC_CommonInitStructure.ADC_DMAAccessMode = ADC_DMAAccessMode_Disabled;
  ADC_CommonInitStructure.ADC_TwoSamplingDelay = ADC_TwoSamplingDelay_5Cycles;
  ADC_CommonInit(&ADC_CommonInitStructure);

  // Initialize the ADCx
  ADC_InitStructure.ADC_Resolution = ADC_Resolution_12b;
  ADC_InitStructure.ADC_ScanConvMode = DISABLE;
  ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;
  ADC_InitStructure.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_None;
  ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
  ADC_InitStructure.ADC_NbrOfConversion = 1;
  ADC_Init(ADC1, &ADC_InitStructure);

  // Enable the ADCx
  ADC_Cmd(ADC1, ENABLE);
}

uint16_t adcRead(uint8_t channel){
  unsigned int pin;

  switch(channel){
  case ADC_YM_CHANNEL:
    pin = ADC_YM_Pin;
    break;
  case ADC_XM_CHANNEL:
    pin = ADC_XM_Pin;
    break;
  case ADC_YP_CHANNEL:
    pin = ADC_YP_Pin;
    break;
  case ADC_XP_CHANNEL:
    pin = ADC_XP_Pin;
    break;
  }

  // Configure the GPIO as an analog input
  GPIO_InitStructure.GPIO_Pin = pin;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_Init(ADC_PORT, &GPIO_InitStructure);

  // Configure the channel corresponding to the GPIO
  ADC_RegularChannelConfig(ADC1, channel, 1, adcChannelSamplingCycles);

  // Start the conversion
  ADC_SoftwareStartConv(ADC1);

  // Wait until conversion completion
  while(ADC_GetSoftwareStartConvStatus(ADC1) == SET);

//  while(ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC) == SET);

  __asm("nop");
  __asm("nop");
  __asm("nop");
  __asm("nop");
  __asm("nop");
  __asm("nop");
  __asm("nop");
  __asm("nop");
  __asm("nop");
  __asm("nop");

  // Return the conversion value
  return ADC_GetConversionValue(ADC1);
}

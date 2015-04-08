#ifndef _ADC_H_
  #define _ADC_H_

#include "projectconfig.h"

#define ADC_YD_CHANNEL  ADC_Channel_0
#define ADC_XR_CHANNEL  ADC_Channel_1
#define ADC_YU_CHANNEL  ADC_Channel_2
#define ADC_XL_CHANNEL  ADC_Channel_3

#define ADC_YM_CHANNEL ADC_YD_CHANNEL
#define ADC_XM_CHANNEL ADC_XR_CHANNEL
#define ADC_YP_CHANNEL ADC_YU_CHANNEL
#define ADC_XP_CHANNEL ADC_XL_CHANNEL

#define ADC_YM_Pin  GPIO_Pin_0
#define ADC_XM_Pin  GPIO_Pin_1
#define ADC_YP_Pin  GPIO_Pin_2
#define ADC_XP_Pin  GPIO_Pin_3

#define ADC_PORT  GPIOA

void        adcInit(void);
uint16_t    adcRead(uint8_t channel);

#endif

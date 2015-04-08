#ifndef _INC_ADC_
#define _INC_ADC_

#include "Nwazet.h"
#include "stdint.h"

typedef enum {
  A0,
  A1,
  A2,
  A3,
  A4,
  A5,
  A6,
  A7,
  VRef,
  VBat,
  AnalogChannelsCount
} AnalogChannelMap_t;

void initADC();
void StartAdcConversion(SerializerContext* context);
uint16_t* GetAnalogChannels();

#endif

#ifndef _INC_LedController_
  #define _INC_LedController_

#include "stm32f0xx_gpio.h"

typedef enum {
  Analog,
  Digital,
  I2CPort,
  SD,
  USART
} LedType;

void initLedController();
void LedControllerSet(LedType gpioType, unsigned char ledIndex, BitAction action);
void LedControllerHardFaultDisplay();

#endif
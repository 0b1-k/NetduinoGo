#ifndef _SYSTICK_H_
  #define _SYSTICK_H_

#include "projectconfig.h"

void      systickInit(void);
void      delay(uint32_t t);
uint32_t  systickGetTicks();
void      nopDelay(unsigned short cycles);
void      OnSystickInterrupt(void);

#endif
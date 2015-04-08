#include "systick.h"
#include "projectconfig.h"

static volatile uint32_t _timingDelay;
static volatile uint32_t _ticks;

void systickInit(void){
  if (SysTick_Config(SystemCoreClock / 1000)){
    /* Capture error */
    while (1);
  }
  _ticks = 0;
}
void nopDelay(unsigned short cycles){
  cycles *= 1000;
  for(unsigned short cycle = 0; cycle < cycles; cycle++){
    __asm("nop");
  }
}
void delay(uint32_t nTime){
  _timingDelay = nTime;
  while(_timingDelay != 0);
}

uint32_t systickGetTicks(){
  return _ticks;
}

void OnSystickInterrupt(void)
{
  if(_timingDelay != 0x00){
    _timingDelay--;
  }
  _ticks++;
}

#ifndef _INC_TIMEOUTS_
#define _INC_TIMEOUTS_

#include "stdint.h"

void initTimeouts();
void DecrementTimeouts();

typedef enum {
  SleepMillisecondsTimeOutId,
  UsartTimeOutId,
  I2cTimeOutId,
  EndOfTimeOutArray
} TimeOutId_t;

void SetTimeOut(TimeOutId_t id, uint32_t milliseconds);
uint8_t IsTimedOut(TimeOutId_t id);
void SleepMilliseconds(uint32_t millisecs);
uint16_t GetMillisecond();

#endif
#ifndef _INC_MESSAGE_
  #define _INC_MESSAGE_

#include "stdint.h"

void initMessage();
void ProcessMessage(uint8_t* message, uint16_t messageBufferSize, uint8_t* responseData, uint16_t responseSize);

#endif

#ifndef _INC_I2C_HANDLER_
#define _INC_I2C_HANDLER_

#include "Nwazet.h"

void OnI2cCommand(
           SerializerContext* cmdContext,
           SerializerContext* respContext,
           unsigned char* responseBuffer,
           unsigned short responseBufferSize);
#endif
#ifndef _INC_USART_HANDLER_
#define _INC_USART_HANDLER_

#include "Nwazet.h"

void OnUsartCommand(
           SerializerContext* cmdContext,
           SerializerContext* respContext,
           unsigned char* responseBuffer,
           unsigned short responseBufferSize);
#endif
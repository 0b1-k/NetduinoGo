#ifndef _INC_SUBMESSAGEHANDLER_CONTEXT_
#define _INC_SUBMESSAGEHANDLER_CONTEXT_

#include "stdint.h"
#include "Nwazet.h"

typedef struct {
  SerializerContext* cmdContext;
  SerializerContext* respContext;
  unsigned char* responseBuffer;
  uint16_t responseBufferSize;
} nwazetMessageContext_t;

void StartResponse(nwazetMessageContext_t* nmc, uint8_t res, int8_t finalize);
void EndResponse(nwazetMessageContext_t* nmc);

#endif
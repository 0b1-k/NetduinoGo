#include "GoBusModuleID.h"
#include "string.h"

static const unsigned char* makerString = "[nwazet.jstk.1.0";

void InitializeModuleUUID(unsigned char* buffer, int bufferLength) {
  if(bufferLength == 17){
    memcpy((void*)(buffer+1), (void*)makerString, 16);
    buffer[0] = 0x80;
  }
}
#include "GoBusModuleID.h"
#include "string.h"

static const unsigned char* makerString = "[nwazet.disp.1.0";

void InitializeModuleUUID(unsigned char* buffer, int bufferLength) {
  if(bufferLength > 18){
    memcpy((void*)(buffer+1), (void*)makerString, 16);
    buffer[0] = 0x80;
    buffer[17] = 0x36; // CRC-8-CCITT, 0x07 polynomial, CRC initialized with 0
  }
}
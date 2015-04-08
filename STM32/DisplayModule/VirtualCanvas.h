#ifndef __VirtualCanvas__
  #define __VirtualCanvas__

#include "Nwazet.h"

void VirtualCanvasReplay(SerializerContext* context, unsigned char* responseBuffer, unsigned short responseBufferSize);
bool IsSynchronizationRequired();
void Reset();
bool IsRebootRequired();

#endif

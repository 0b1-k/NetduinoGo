#ifndef __INC_GO_SPI_COMMUNICATION__
  #define __INC_GO_SPI_COMMUNICATION__

#include "Nwazet.h"

void initSPI();
void AssertGoBusIRQ();
void ReleaseGoBusIRQ();
void CheckMessage();
void SpiDumpLastMessages();
#endif

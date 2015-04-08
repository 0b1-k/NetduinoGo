#ifndef __INC_GO_SPI_COMMUNICATION__
  #define __INC_GO_SPI_COMMUNICATION__

#include "Nwazet.h"

//void spiInit(SerializerContext* context);
void spiInit();
void AssertGoBusIRQ();
void ReleaseGoBusIRQ();

#endif

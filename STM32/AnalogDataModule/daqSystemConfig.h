#ifndef _INC_DAQ_SYSTEM_CONFIG_
#define _INC_DAQ_SYSTEM_CONFIG_

#define ClockConfigured         0x00000001
#define ClockTimeSet            0x00000002
#define DiagnosticsToSerial     0x00000004
#define FirmataMode             0x00000008

#include "stdint.h"

uint8_t GetDaqSystemOption(uint32_t option);
void SetDaqSystemOption(uint32_t option, uint8_t state);

#endif

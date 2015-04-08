#ifndef __EEPROM_H__ 
#define __EEPROM_H__

#include "projectconfig.h"

// Method Prototypes
bool      eepromCheckAddress ( uint16_t addr );
uint8_t   eepromReadU8 ( uint16_t addr );
int8_t    eepromReadS8 ( uint16_t addr );
uint16_t  eepromReadU16 ( uint16_t addr );
int16_t   eepromReadS16 ( uint16_t addr );
uint32_t  eepromReadU32 ( uint16_t addr );
int32_t   eepromReadS32 ( uint16_t addr );
uint64_t  eepromReadU64 ( uint16_t addr );
int64_t   eepromReadS64 ( uint16_t addr );
void      eepromReadBuffer ( uint16_t addr, uint8_t *buffer, uint32_t bufferLength);
void      eepromWriteU8 ( uint16_t addr, uint8_t value );
void      eepromWriteS8 ( uint16_t addr, int8_t value );
void      eepromWriteU16 ( uint16_t addr, uint16_t value );
void      eepromWriteS16 ( uint16_t addr, int16_t value );
void      eepromWriteU32 ( uint16_t addr, uint32_t value );
void      eepromWriteS32 ( uint16_t addr, int32_t value );
void      eepromWriteU64 ( uint16_t addr, uint64_t value );
void      eepromWriteS64 ( uint16_t addr, int64_t value );

#endif

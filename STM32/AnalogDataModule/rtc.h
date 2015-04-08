#ifndef _INC_RTC_
  #define _INC_RTC_

#include "Nwazet.h"

typedef enum {
  System,
  User1,
  User2,
  User3,
  User4,
} RtcBackupRegister_t;

void initRTC();
unsigned char GetRtcDateTimeSetState();
void SetRtcTime(SerializerContext* context);
void GetRtcTime(SerializerContext* context);
void GetRtcBackupRegisters(SerializerContext* context);
void SetRtcBackupRegisters(SerializerContext* context);
uint32_t GetRtcBackupRegister(RtcBackupRegister_t id);
void SetRtcBackupRegister(RtcBackupRegister_t id, uint32_t value);
unsigned long GetRtcTimeFatFormat();
#endif

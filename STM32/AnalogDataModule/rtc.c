/*
[nwazet Open Source Software & Open Source Hardware
Authors: Fabien Royer
Software License Agreement (BSD License)

Copyright (c) 2010-2012, Nwazet, LLC. All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

* Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
* Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
* Neither the name of Nwazet, LLC. nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.
* The names '[nwazet', 'nwazet', the ASCII hazelnut in the [nwazet logo and the color of the logo are Trademarks of nwazet, LLC. and cannot be used to endorse or promote products derived from this software or any hardware designs without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/
#include "stm32f0xx_pwr.h"
#include "stm32f0xx_rcc.h"
#include "stm32f0xx_rtc.h"
#include "daqSystemConfig.h"
#include "rtc.h"
#include "vt100.h"
#include <stdio.h>
#include "timeouts.h"

void configRTC(){
  PrintNeutralMessage("RTC config");

  // Backup domain reset
  RCC->BDCR = RCC_BDCR_BDRST;
  int count = 1000;
  while(count--);
  RCC->BDCR = 0;

   // Backup domain access enabled
  PWR->CR |= PWR_CR_DBP;

  // Turn the LSE on
  RCC->BDCR |= RCC_BDCR_LSEON;
  while ((RCC->BDCR & RCC_BDCR_LSERDY) == 0);

  // Select the LSE as the RTC clock source
  RCC_RTCCLKConfig(RCC_RTCCLKSource_LSE);

  // Enable the RTC Clock
  RCC_RTCCLKCmd(ENABLE);

  // Configure the RTC data register and RTC prescaler
  RTC_InitTypeDef RtcInitStructure;
  RtcInitStructure.RTC_AsynchPrediv = 0x7F;
  RtcInitStructure.RTC_SynchPrediv = 0xFF;
  RtcInitStructure.RTC_HourFormat = RTC_HourFormat_24;

  // Initialize the RTC
  if (RTC_Init(&RtcInitStructure) == ERROR){
    PrintErrorMessage("RTC config failed");
    return;
  }

  // Wait for RTC APB registers synchronization
  RTC_WaitForSynchro();

  // Remember that the RTC has been configured
  SetDaqSystemOption(ClockConfigured, true);

  PrintSuccessMessage("RTC config ok");

  return;
}
void ShowCurrentTime(){
  RTC_TimeTypeDef ts;
  RTC_DateTypeDef ds;

  RTC_GetDate(RTC_Format_BIN, &ds);
  RTC_GetTime(RTC_Format_BIN, &ts);

  vt100TextDefault();
  printf("Time: %i/%02i/%0i, %02i:%02i:%02i.%i\r\n",
         ds.RTC_Year + 2000, ds.RTC_Month, ds.RTC_Date,
         ts.RTC_Hours, ts.RTC_Minutes, ts.RTC_Seconds, GetMillisecond());
}
void ShowBackupRegisters(){
  vt100TextDefault();
  PrintNeutralMessage("Backup registers:");
  printf("\tSR: 0x%x\r\n", GetRtcBackupRegister(System));
  printf("\tU1: 0x%x\r\n", GetRtcBackupRegister(User1));
  printf("\tU2: 0x%x\r\n", GetRtcBackupRegister(User2));
  printf("\tU3: 0x%x\r\n", GetRtcBackupRegister(User3));
  printf("\tU4: 0x%x\r\n", GetRtcBackupRegister(User4));
}
void initRTC(){
  PrintNeutralMessage("RTC init");
  if (!GetDaqSystemOption(ClockConfigured)){
    configRTC();
  }else{
    RCC_RTCCLKCmd(ENABLE);
    RTC_WaitForSynchro();
  }
  PrintSuccessMessage("RTC init ok");
  ShowCurrentTime();
  ShowBackupRegisters();
}
enum ClockState {
    Invalid,
    Valid
};
unsigned char GetRtcDateTimeSetState(){
  if (!GetDaqSystemOption(ClockTimeSet)){
    return Invalid;
  }
  return Valid;
}
void GetRtcTime(SerializerContext* context){
  RTC_TimeTypeDef ts;
  RTC_DateTypeDef ds;

  RTC_GetDate(RTC_Format_BIN, &ds);
  RTC_GetTime(RTC_Format_BIN, &ts);

  uint32_t subSeconds = GetMillisecond();

  Put(context, (void*)&ds.RTC_Year, sizeof(ds.RTC_Year), 1);
  Put(context, (void*)&ds.RTC_Month, sizeof(ds.RTC_Month), 1);
  Put(context, (void*)&ds.RTC_Date, sizeof(ds.RTC_Date), 1);
  Put(context, (void*)&ds.RTC_WeekDay, sizeof(ds.RTC_WeekDay), 1);

  Put(context, (void*)&ts.RTC_Hours, sizeof(ts.RTC_Hours), 1);
  Put(context, (void*)&ts.RTC_Minutes, sizeof(ts.RTC_Minutes), 1);
  Put(context, (void*)&ts.RTC_Seconds, sizeof(ts.RTC_Seconds), 1);
  Put(context, (void*)&subSeconds, sizeof(subSeconds), 1);
}
void SetRtcTime(SerializerContext* context){
  RTC_TimeTypeDef ts;
  RTC_DateTypeDef ds;

  RTC_TimeStructInit(&ts);
  RTC_DateStructInit(&ds);

  PWR_BackupAccessCmd(ENABLE);

  Get(context, (void*)&ds.RTC_Year, sizeof(ds.RTC_Year));
  Get(context, (void*)&ds.RTC_Month, sizeof(ds.RTC_Month));
  Get(context, (void*)&ds.RTC_Date, sizeof(ds.RTC_Date));
  Get(context, (void*)&ds.RTC_WeekDay, sizeof(ds.RTC_WeekDay));

  ErrorStatus dsStatus = RTC_SetDate(RTC_Format_BIN, &ds);

  Get(context, (void*)&ts.RTC_Hours, sizeof(ts.RTC_Hours));
  Get(context, (void*)&ts.RTC_Minutes, sizeof(ts.RTC_Minutes));
  Get(context, (void*)&ts.RTC_Seconds, sizeof(ts.RTC_Seconds));
  ts.RTC_H12 = RTC_H12_AM;

  ErrorStatus tsStatus = RTC_SetTime(RTC_Format_BIN, &ts);

  if(tsStatus == SUCCESS && dsStatus == SUCCESS){
    SetDaqSystemOption(ClockTimeSet, true);
  }

  PWR_BackupAccessCmd(DISABLE);
}
unsigned long GetRtcTimeFatFormat(){
  RTC_TimeTypeDef ts;
  RTC_DateTypeDef ds;

  RTC_GetDate(RTC_Format_BIN, &ds);
  RTC_GetTime(RTC_Format_BIN, &ts);

  unsigned long fatTime =  (((unsigned long)ds.RTC_Year + 20) << 25) // Year since 1980
			| ((unsigned long)ds.RTC_Month << 21)
			| ((unsigned long)ds.RTC_Date << 16)
			| (unsigned short)(ts.RTC_Hours << 11)
			| (unsigned short)(ts.RTC_Minutes << 5)
			| (unsigned short)(ts.RTC_Seconds >> 1);
  return fatTime;
}
void GetRtcBackupRegisters(SerializerContext* context){
  uint32_t rtcRegister = 0;

  rtcRegister = GetRtcBackupRegister(System);
  Put(context, (void*)&rtcRegister, sizeof(rtcRegister), 1);

  rtcRegister = GetRtcBackupRegister(User1);
  Put(context, (void*)&rtcRegister, sizeof(rtcRegister), 1);

  rtcRegister = GetRtcBackupRegister(User2);
  Put(context, (void*)&rtcRegister, sizeof(rtcRegister), 1);

  rtcRegister = GetRtcBackupRegister(User3);
  Put(context, (void*)&rtcRegister, sizeof(rtcRegister), 1);

  rtcRegister = GetRtcBackupRegister(User4);
  Put(context, (void*)&rtcRegister, sizeof(rtcRegister), 1);
}
void SetRtcBackupRegisters(SerializerContext* context){
  uint32_t rtcRegister = 0;

  Get(context, (void*)&rtcRegister, sizeof(rtcRegister));
  SetRtcBackupRegister(System, rtcRegister);

  Get(context, (void*)&rtcRegister, sizeof(rtcRegister));
  SetRtcBackupRegister(User1, rtcRegister);

  Get(context, (void*)&rtcRegister, sizeof(rtcRegister));
  SetRtcBackupRegister(User2, rtcRegister);

  Get(context, (void*)&rtcRegister, sizeof(rtcRegister));
  SetRtcBackupRegister(User3, rtcRegister);

  Get(context, (void*)&rtcRegister, sizeof(rtcRegister));
  SetRtcBackupRegister(User4, rtcRegister);
}
uint32_t GetRtcBackupRegister(RtcBackupRegister_t id){
  if(id <= User4){
    return RTC_ReadBackupRegister((uint32_t) id);
  }
  return 0;
}
void SetRtcBackupRegister(RtcBackupRegister_t id, uint32_t value){
  if(id <= User4){
    PWR_BackupAccessCmd(ENABLE);
    RTC_WriteBackupRegister((uint32_t) id, value);
    PWR_BackupAccessCmd(DISABLE);
  }
}

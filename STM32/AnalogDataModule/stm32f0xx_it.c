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
#include "stm32f0xx_it.h"
#include "LedController.h"
#include "timeouts.h"
#include "vt100.h"
#include <stdio.h>
#include "spi.h"
#include "lastFunction.h"

void NMI_Handler(void){
}

// Custom Hard Fault handler called from HardFault_Handler in startup_stm32f0xx.s
//
// References:
// https://my.st.com/public/STe2ecommunities/mcu/Lists/cortex_mx_stm32/flat.aspx?RootFolder=%2Fpublic%2FSTe2ecommunities%2Fmcu%2FLists%2Fcortex_mx_stm32%2FHard%20Fault%20and%20stack%20pushing&FolderCTID=0x01200200770978C69A1141439FE559EB459D7580009C4E14902C3CDE46A77F0FFD06506F5B&currentviews=9647
// https://my.st.com/public/STe2ecommunities/mcu/Lists/cortex_mx_stm32/Flat.aspx?RootFolder=%2Fpublic%2FSTe2ecommunities%2Fmcu%2FLists%2Fcortex_mx_stm32%2FHardFault%20Exception%20Why%20oh%20why!&FolderCTID=0x01200200770978C69A1141439FE559EB459D7580009C4E14902C3CDE46A77F0FFD06506F5B&currentviews=4098
// http://blog.frankvh.com/2011/12/07/cortex-m3-m4-hard-fault-handler/
// http://support.code-red-tech.com/CodeRedWiki/DebugHardFault
void HardFault_Handler_C(unsigned long *hardfault_args){
  volatile unsigned long stacked_r0;
  volatile unsigned long stacked_r1;
  volatile unsigned long stacked_r2;
  volatile unsigned long stacked_r3;
  volatile unsigned long stacked_r12;
  volatile unsigned long stacked_lr;
  volatile unsigned long stacked_pc;
  volatile unsigned long stacked_psr;
  volatile unsigned long _CFSR;
  volatile unsigned long _HFSR;
  volatile unsigned long _DFSR;
  volatile unsigned long _AFSR;
  volatile unsigned long _BFAR;
  volatile unsigned long _MMAR;

  stacked_r0 = ((unsigned long)hardfault_args[0]);
  stacked_r1 = ((unsigned long)hardfault_args[1]);
  stacked_r2 = ((unsigned long)hardfault_args[2]);
  stacked_r3 = ((unsigned long)hardfault_args[3]);
  stacked_r12 = ((unsigned long)hardfault_args[4]);
  stacked_lr = ((unsigned long)hardfault_args[5]);
  stacked_pc = ((unsigned long)hardfault_args[6]);
  stacked_psr = ((unsigned long)hardfault_args[7]);

  LedControllerHardFaultDisplay();

  vt100TextColor(VT100_Cyan, VT100_Black);

  printf("Hard Fault Handler\r\n");
  printf("Registers:\r\n");
  printf ("R0 = %x\n", stacked_r0);
  printf ("R1 = %x\n", stacked_r1);
  printf ("R2 = %x\n", stacked_r2);
  printf ("R3 = %x\n", stacked_r3);
  printf ("R12 = %x\n", stacked_r12);
  printf ("LR = %x\n", stacked_lr);
  printf ("PC = %x\n", stacked_pc);
  printf ("PSR = %x\n", stacked_psr);
  printf ("BFAR = %x\n", (*((volatile unsigned long *)(0xE000ED38)))); // Bus Fault Address Register
  printf ("CFSR = %x\n", (*((volatile unsigned long *)(0xE000ED28)))); // Configurable Fault Status Register. Consists of MMSR, BFSR and UFSR
  printf ("HFSR = %x\n", (*((volatile unsigned long *)(0xE000ED2C)))); // Hard Fault Status Register
  printf ("DFSR = %x\n", (*((volatile unsigned long *)(0xE000ED30)))); // Debug Fault Status Register
  printf ("AFSR = %x\n", (*((volatile unsigned long *)(0xE000ED3C)))); // Auxiliary Fault Status Register
  printf ("MMAR = %x\n", (*((volatile unsigned long *)(0xE000ED34)))); // MemManage Fault Address Register
                                                                       // Read the Fault Address Registers. These may not contain valid values.
                                                                       // Check BFARVALID/MMARVALID to see if they are valid values
  DumpLastFunctionNames();
  SpiDumpLastMessages();

  NVIC_SystemReset();
}

void SVC_Handler(void){
}
void PendSV_Handler(void){
}
void SysTick_Handler(void){
  DecrementTimeouts();
}

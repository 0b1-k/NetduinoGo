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
#include "i2chandler.h"
#include "Nwazet.h"
#include "submessagehandlercontext.h"
#include "i2c.h"
#include "vt100.h"
#include <stdio.h>
#include "LedController.h"

enum I2cCommand {
    Write,
    Read,
    IsBusBusy,
    BusReset
};

#define MaxByteChunk    255

void OnI2cWrite(nwazetMessageContext_t* nmc){
  uint8_t res = (uint8_t) OK;
  uint8_t speed = 0;
  uint16_t address = 0;
  uint16_t byteCount = 0;
  Get(nmc->cmdContext, (void*)&speed, sizeof(speed));
  Get(nmc->cmdContext, (void*)&address, sizeof(address));
  Get(nmc->cmdContext, (void*)&byteCount, sizeof(byteCount));
  if(byteCount > MaxByteChunk){
    res = InvalidByteCount;
  }else{
    uint8_t* buffer = (uint8_t*)(nmc->cmdContext->Buffer + nmc->cmdContext->CurrentIndex);
    res = (uint8_t)i2cWrite((I2cSpeed_t)speed, address, buffer, byteCount);
  }
  StartResponse(nmc, i2cCheckResult(__func__, (i2cResult_t)res), true);
}
void OnI2cRead(nwazetMessageContext_t* nmc){
  uint8_t res = (uint8_t) OK;
  uint8_t speed = 0;
  uint16_t address = 0;
  uint16_t byteCount = 0;
  Get(nmc->cmdContext, (void*)&speed, sizeof(speed));
  Get(nmc->cmdContext, (void*)&address, sizeof(address));
  Get(nmc->cmdContext, (void*)&byteCount, sizeof(byteCount));
  if(byteCount > MaxByteChunk){
    StartResponse(nmc, i2cCheckResult(__func__, InvalidByteCount), true);
  }else{
    SerializerContext snapshot;
    InitializeSendBuffer(nmc->respContext, nmc->responseBuffer, nmc->responseBufferSize);
    TakeContextSnapshot(&snapshot, nmc->respContext);
    Put(nmc->respContext, (void*)&res, sizeof(res), 1);
    uint8_t* buffer = (uint8_t*)(nmc->responseBuffer + nmc->respContext->CurrentIndex);
    res = (uint8_t)i2cRead((I2cSpeed_t)speed, address, buffer, byteCount);
    RestoreContextFromSnapshot(&snapshot, nmc->respContext);
    Put(nmc->respContext, (void*)&res, sizeof(res), 1);
    FinalizeSendBuffer(nmc->respContext);
  }
}
void OnI2cIsBusBusy(nwazetMessageContext_t* nmc){
  StartResponse(nmc, i2cCheckResult(__func__, (i2cResult_t)OK), false);
  uint8_t busyState = i2cIsBusBusy();
  Put(nmc->respContext, (void*)&busyState, sizeof(busyState), 1);
  EndResponse(nmc);
}
static nwazetMessageContext_t nmc;

void OnI2cCommand(
  SerializerContext* cmdContext,
  SerializerContext* respContext,
  unsigned char* responseBuffer,
  unsigned short responseBufferSize){

  nmc.cmdContext = cmdContext;
  nmc.respContext = respContext;
  nmc.responseBuffer = responseBuffer;
  nmc.responseBufferSize = responseBufferSize;

  LedControllerSet(I2CPort, 0, Bit_SET);
  uint8_t cmd = 0;
  Get(cmdContext, (void*)&cmd, sizeof(cmd));
  switch(cmd){
  case Write:
    OnI2cWrite(&nmc);
    break;
  case Read:
    OnI2cRead(&nmc);
    break;
  case IsBusBusy:
    OnI2cIsBusBusy(&nmc);
    break;
  case BusReset:
    i2cBusReset();
    break;
  }
  LedControllerSet(I2CPort, 0, Bit_RESET);
}
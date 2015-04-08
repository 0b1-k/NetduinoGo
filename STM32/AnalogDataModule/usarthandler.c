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
#include "usarthandler.h"
#include "Nwazet.h"
#include "submessagehandlercontext.h"
#include "usart.h"
#include "vt100.h"
#include <stdio.h>

// Usart commands
enum UsartCommand {
    SetConfig,
    Write,
    Read
};
enum Result {
    OK,
    Timeout,
    TerminatorReceived,
    InvalidParameter
};
const char* ResultStrings[] = {
  "OK",
  "Timeout",
  "Terminator",
  "Invalid parameter"
};

const char* ResultLookupOutOfRange = "invalid result code";

const char* ResultLookup(uint8_t res){
  if(res <= InvalidParameter){
    return ResultStrings[res];
  }else{
    return ResultLookupOutOfRange;
  }
}
uint8_t usartCheckSuccess(const char* function, const char* msg, uint8_t res, const char* objName){
  if(res != OK){
    vt100TextColor(VT100_Red, VT100_Black);
    if(objName){
      printf("usart: %s(%s): %s (%i), object: %s\r\n", function, msg, ResultLookup(res), res, objName);
    }else{
      printf("usart: %s(%s): %s (%i)\r\n", function, msg, ResultLookup(res), res);
    }
    vt100TextDefault();
  }
  return res;
}
void OnSetConfig(nwazetMessageContext_t* nmc){
  uint8_t result = usartConfig(nmc->cmdContext);
  InitializeSendBuffer(nmc->respContext, nmc->responseBuffer, nmc->responseBufferSize);
  Put(nmc->respContext, (void*)&result, sizeof(result), 1);
  FinalizeSendBuffer(nmc->respContext);
}
#define MaxByteChunk    512
void OnWrite(nwazetMessageContext_t* nmc){
  uint8_t res = (uint8_t) OK;
  uint16_t bytesToWrite = 0;
  uint16_t bytesWritten = 0;
  Get(nmc->cmdContext, (void*)&bytesToWrite, sizeof(bytesToWrite));
  if(bytesToWrite > MaxByteChunk){
    usartCheckSuccess(__func__, "bytesToWrite", InvalidParameter, 0);
    StartResponse(nmc, InvalidParameter, true);
  }else{
    uint8_t* buffer = (uint8_t*)(nmc->cmdContext->Buffer + nmc->cmdContext->CurrentIndex);
    while(bytesToWrite--){
      if(usartPut((uint8_t) *buffer++) != USART_TIMEOUT){
        bytesWritten++;
      }else{
        res = Timeout;
        break;
      }
    }
    usartCheckSuccess(__func__, "usartPut", res, 0);
    InitializeSendBuffer(nmc->respContext, nmc->responseBuffer, nmc->responseBufferSize);
    Put(nmc->respContext, (void*)&res, sizeof(res), 1);
    Put(nmc->respContext, (void*)&bytesWritten, sizeof(bytesWritten), 1);
    FinalizeSendBuffer(nmc->respContext);
  }
}
void OnRead(nwazetMessageContext_t* nmc){
  SerializerContext snapshot;
  uint16_t bytesToRead = 0;
  uint16_t bytesRead = 0;
  uint32_t totalTimeoutMs = 0;
  uint8_t useTerminator = 0;
  uint8_t terminator = 0;
  uint8_t res = (uint8_t) Timeout;

  Get(nmc->cmdContext, (void*)&bytesToRead, sizeof(bytesToRead));
  Get(nmc->cmdContext, (void*)&totalTimeoutMs, sizeof(totalTimeoutMs));
  Get(nmc->cmdContext, (void*)&useTerminator, sizeof(useTerminator));
  Get(nmc->cmdContext, (void*)&terminator, sizeof(terminator));

  if(!totalTimeoutMs){
    totalTimeoutMs = MAX_USART_TIMEOUT;
  }

  if(bytesToRead > nmc->responseBufferSize){
    usartCheckSuccess(__func__, "bytesToRead", InvalidParameter, 0);
    StartResponse(nmc, InvalidParameter, true);
  }else{
    InitializeSendBuffer(nmc->respContext, nmc->responseBuffer, nmc->responseBufferSize);
    TakeContextSnapshot(&snapshot, nmc->respContext);
    Put(nmc->respContext, (void*)&res, sizeof(res), 1);
    Put(nmc->respContext, (void*)&bytesRead, sizeof(bytesRead), 1);

    uint32_t timeoutCountMs = 0;
    uint16_t rxTimeOutMs = usartGetRxTimeOutMs();
    uint8_t* buffer = (uint8_t*)(nmc->responseBuffer + nmc->respContext->CurrentIndex);

    while((timeoutCountMs < totalTimeoutMs)){
      int c = usartGet();
      if(c == USART_TIMEOUT){
        timeoutCountMs += rxTimeOutMs;
        continue;
      }
      *buffer++ = (uint8_t)c;
      bytesRead++;
      bytesToRead--;
      if(!bytesToRead){
        res = OK;
        break;
      }
      if(useTerminator && (uint8_t) c == terminator){
        res = TerminatorReceived;
        break;
      }
    }
    RestoreContextFromSnapshot(&snapshot, nmc->respContext);
    Put(nmc->respContext, (void*)&res, sizeof(res), 1);
    Put(nmc->respContext, (void*)&bytesRead, sizeof(bytesRead), 1);
    FinalizeSendBuffer(nmc->respContext);
  }
}
void OnUsartCommand(
  SerializerContext* cmdContext,
  SerializerContext* respContext,
  unsigned char* responseBuffer,
  unsigned short responseBufferSize){
  nwazetMessageContext_t nmc;

  nmc.cmdContext = cmdContext;
  nmc.respContext = respContext;
  nmc.responseBuffer = responseBuffer;
  nmc.responseBufferSize = responseBufferSize;

  unsigned char usartCmd = 0;
  Get(cmdContext, (void*)&usartCmd, sizeof(usartCmd));
  switch(usartCmd){
  case SetConfig:
    OnSetConfig(&nmc);
    break;
  case Write:
    OnWrite(&nmc);
    break;
  case Read:
    OnRead(&nmc);
    break;
  }
}
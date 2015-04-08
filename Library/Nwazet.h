/*
Copyright (c) 2012, Nwazet, LLC. All rights reserved.

Author: Fabien Royer

New BSD License.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:
* Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
* Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
* Neither the name of Nwazet, LLC. nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#ifndef NwazetLibInc
  #define NwazetLibInc 1

#define NwazetLibSerializerHeaderVersion 3

#define BufferStartOffset 2
#define NwazetLibSerializerHeaderVersionOffset BufferStartOffset + 0
#define NwazetLibSerializerHeaderContentSizeOffset BufferStartOffset + 1

#ifndef __STM8S_H
  #ifndef bool
  typedef unsigned char bool;
  #define true  1
  #define false 0
  #endif
#endif

typedef struct {
  unsigned short CurrentIndex;
  unsigned short BufferLength;
  unsigned short PayloadLength;
  unsigned char* Buffer;
  bool IsLittleEndian;
  bool DataOverflow;
  bool IsValidPacket;
} SerializerContext;

// Functions used to serialize data before sending
void InitializeSendBuffer(SerializerContext* context, unsigned char* buffer, unsigned short bufferLength);
bool Put(SerializerContext* context, void* data, unsigned short size, unsigned short count);
unsigned short FinalizeSendBuffer(SerializerContext* context);

// Functions used to deserialize received data
unsigned short InitializeReceiveBuffer(SerializerContext* context, unsigned char* buffer, unsigned short bufferLength);
bool MoreData(SerializerContext* context);
bool Get(SerializerContext* context, void* data, unsigned short size);
void Terminate(SerializerContext* context);

void* GetStringReference(SerializerContext* context, unsigned short* textLength, unsigned char* isTextASCII);
void* GetArrayReference(SerializerContext* context, unsigned short* arrayLength, unsigned short elementSize);

void PutStringReference(SerializerContext* context, unsigned char* text, unsigned short characterCount, unsigned char isTextASCII);

// Take a snapshot of the current serialization context
void TakeContextSnapshot(SerializerContext* contextSnapShot, SerializerContext* currentContext);
// Restore the current serialization context from a snapshot
void RestoreContextFromSnapshot(SerializerContext* contextSnapShot, SerializerContext* currentContext);

// Private functions
void Store(SerializerContext* context, unsigned char data);
void Retrieve(SerializerContext* context, unsigned char* data);
void DetermineEndianness(SerializerContext* context);

#endif
#include "Nwazet.h"
#include "stdint.h"
#include "string.h"

void InitializeSendBuffer(SerializerContext* context, unsigned char* buffer, unsigned short bufferLength) {
  if(!buffer || bufferLength == 0){
    return;
  }
  DetermineEndianness(context);
  context->CurrentIndex = BufferStartOffset;
  context->Buffer = buffer;
  context->BufferLength = bufferLength;
  context->DataOverflow = false;
  context->IsValidPacket = true;
  context->PayloadLength = 0;
  // Add a header to the buffer
  unsigned char version = NwazetLibSerializerHeaderVersion;
  Put(context, (void*)&version, sizeof(version), 1);
  // Reserve space in the header for the buffer length
  Put(context, (void*)&context->PayloadLength, sizeof(context->PayloadLength), 1);
}
void PutStringReference(SerializerContext* context, unsigned char* text, unsigned short characterCount, unsigned char isTextASCII){
  Put(context, (void*) &isTextASCII, sizeof(isTextASCII), 1);
  Put(context, (void*) &characterCount, sizeof(characterCount), 1);
  unsigned short lengthInBytes;
  if(isTextASCII){
    lengthInBytes = characterCount;
    memcpy((void*)&context->Buffer[context->CurrentIndex], (void*)text, lengthInBytes);
    context->CurrentIndex += lengthInBytes;
    unsigned char terminator = 0;
    Put(context, (void*) &terminator, sizeof(terminator), 1);
  }else{
    lengthInBytes = characterCount * sizeof(unsigned short);
    memcpy((void*)&context->Buffer[context->CurrentIndex], (void*)text, lengthInBytes);
    context->CurrentIndex += lengthInBytes;
    unsigned short terminator = 0;
    Put(context, (void*) &terminator, sizeof(terminator), 1);
  }
}
bool Put(SerializerContext* context, void* data, unsigned short size, unsigned short count){
  if(!count) return false;
  unsigned char* temp = (unsigned char*)data;
  switch(size){
  case 1:
    if(count==1){
      Store(context, *temp);
    }else{
      Put(context, (void*)&count, sizeof(count), 1);
      for(unsigned short i = 0; i < count; i++){
        Store(context, temp[i]);
      }
    }
    break;
  case 2:
    if(count==1){
      if(context->IsLittleEndian){
        Store(context, *(temp+1));
        Store(context, *(temp));
      }else{
        Store(context, *(temp));
        Store(context, *(temp+1));
      }
    }else{
      Put(context, (void*)&count, sizeof(count), 1);
        unsigned short* tempShort = (unsigned short*)data;
        for(unsigned short i = 0; i < count; i++){
          Put(context, (void*)&tempShort[i], sizeof(unsigned short), 1);
      }
    }
    break;
  case 4:
    if(count==1){
      if(context->IsLittleEndian){
        Store(context, *(temp+3));
        Store(context, *(temp+2));
        Store(context, *(temp+1));
        Store(context, *(temp));
      }else{
        Store(context, *(temp));
        Store(context, *(temp+1));
        Store(context, *(temp+2));
        Store(context, *(temp+3));
      }
    }else{
      Put(context, (void*)&count, sizeof(count), 1);
        unsigned long* tempLong = (unsigned long*)data;
        for(unsigned short i = 0; i < count; i++){
          Put(context, (void*)&tempLong[i], sizeof(unsigned long), 1);
      }
    }
    break;
#ifdef uint64_t
  case 8:
    if(count==1){
      if(context->IsLittleEndian){
        Store(context, temp+7);
        Store(context, temp+6);
        Store(context, temp+5);
        Store(context, temp+4);
        Store(context, temp+3);
        Store(context, temp+2);
        Store(context, temp+1);
        Store(context, temp);
      else{
        Store(context, temp);
        Store(context, temp+1);
        Store(context, temp+2);
        Store(context, temp+3);
        Store(context, temp+4);
        Store(context, temp+5);
        Store(context, temp+6);
        Store(context, temp+7);
      }
    }else{
      Put(context, UInt16, (void*)&length, 1);
      int64_t* tempLongLong = (int64_t*)data;
      for(unsigned short i = 0; i < length; i++){
        Put(context, (void*)&tempLongLong[i], sizeof(int64_t), 1);
      }
    }
    break;
#endif
  default:
    return false;
  }
  return true;
}
unsigned short FinalizeSendBuffer(SerializerContext* context){
  context->PayloadLength = context->CurrentIndex;
  if(context->IsLittleEndian){
    context->Buffer[NwazetLibSerializerHeaderContentSizeOffset + 1] = (unsigned char)(context->PayloadLength);
    context->Buffer[NwazetLibSerializerHeaderContentSizeOffset] = (unsigned char)(context->PayloadLength >> 8);
  }else{
    context->Buffer[NwazetLibSerializerHeaderContentSizeOffset] = (unsigned char)(context->PayloadLength >> 8);
    context->Buffer[NwazetLibSerializerHeaderContentSizeOffset + 1] = (unsigned char)(context->PayloadLength);
  }
  context->CurrentIndex = BufferStartOffset;
  return context->PayloadLength;
}
unsigned short InitializeReceiveBuffer(SerializerContext* context, unsigned char* buffer, unsigned short bufferLength){
  context->IsValidPacket = false;
  context->Buffer = buffer;
  context->BufferLength = bufferLength;
  context->DataOverflow = false;
  context->PayloadLength = 0;
  context->CurrentIndex = BufferStartOffset + NwazetLibSerializerHeaderVersion;

  if(!buffer || bufferLength == 0){
    return 0;
  }

  DetermineEndianness(context);

  if(context->Buffer[NwazetLibSerializerHeaderVersionOffset] == NwazetLibSerializerHeaderVersion){
    context->IsValidPacket = true;
  }else{
    return 0;
  }

  unsigned char* temp = (unsigned char*) &context->PayloadLength;
  if(context->IsLittleEndian){
    *temp = context->Buffer[NwazetLibSerializerHeaderContentSizeOffset+1];
    *(temp+1) = context->Buffer[NwazetLibSerializerHeaderContentSizeOffset];
  }else{
    *temp = context->Buffer[NwazetLibSerializerHeaderContentSizeOffset];
    *(temp+1) = context->PayloadLength | context->Buffer[NwazetLibSerializerHeaderContentSizeOffset+1];
  }
  return context->PayloadLength;
}
bool Get(SerializerContext* context, void* data, unsigned short size){
  unsigned char* temp = (unsigned char*)data;
  switch(size){
  case 1:
    Retrieve(context, temp);
    break;
  case 2:
    if(context->IsLittleEndian){
      Retrieve(context, temp+1);
      Retrieve(context, temp);
    }else{
      Retrieve(context, temp);
      Retrieve(context, temp+1);
    }
    break;
  case 4:
    if(context->IsLittleEndian){
      Retrieve(context, temp+3);
      Retrieve(context, temp+2);
      Retrieve(context, temp+1);
      Retrieve(context, temp);
    }else{
      Retrieve(context, temp);
      Retrieve(context, temp+1);
      Retrieve(context, temp+2);
      Retrieve(context, temp+3);
    }
    break;
  case 8:
    if(context->IsLittleEndian){
      Retrieve(context, temp+7);
      Retrieve(context, temp+6);
      Retrieve(context, temp+5);
      Retrieve(context, temp+4);
      Retrieve(context, temp+3);
      Retrieve(context, temp+2);
      Retrieve(context, temp+1);
      Retrieve(context, temp);
    }else{
      Retrieve(context, temp);
      Retrieve(context, temp+1);
      Retrieve(context, temp+2);
      Retrieve(context, temp+3);
      Retrieve(context, temp+4);
      Retrieve(context, temp+5);
      Retrieve(context, temp+6);
      Retrieve(context, temp+7);
    }
    break;
  default:
    return false;
  }
  return true;
}
void Store(SerializerContext* context, unsigned char data){
  if(context->CurrentIndex < context->BufferLength){
    context->Buffer[context->CurrentIndex++] = data;
  }else{
    context->DataOverflow = true;
    context->IsValidPacket = false;
  }
}
void Retrieve(SerializerContext* context, unsigned char* data){
  if(context->CurrentIndex < context->BufferLength && context->CurrentIndex < context->PayloadLength){
    *data = context->Buffer[context->CurrentIndex++];
  }else{
    context->DataOverflow = true;
    context->IsValidPacket = false;
  }
}
bool MoreData(SerializerContext* context){
  return (context->CurrentIndex < context->BufferLength && context->CurrentIndex < context->PayloadLength) ? true : false;
}
void Terminate(SerializerContext* context){
  context->CurrentIndex = context->PayloadLength;
}
void DetermineEndianness(SerializerContext* context){
  unsigned short test = 0xDEAD;
  unsigned char* buffer = (unsigned char*)&test;
  if(buffer[0]==0xAD){
    context->IsLittleEndian = true;
  }else {
    context->IsLittleEndian = false;
  }
}
void* GetStringReference(SerializerContext* context, unsigned short* textLength, unsigned char* isTextASCII){
  if(Get(context, (void*)isTextASCII, sizeof(unsigned char))){
    if(Get(context, (void*)textLength, sizeof(unsigned short))){
      void* text = (void*) &context->Buffer[context->CurrentIndex];
      // Skip the string and its terminator
      if(*isTextASCII){
        context->CurrentIndex += (*textLength) + 1;
      }else{
        context->CurrentIndex += ((*textLength) * sizeof(unsigned short)) + sizeof(unsigned short);
      }
      return text;
    }
  }
  return 0;
}
void* GetArrayReference(SerializerContext* context, unsigned short* arrayLength, unsigned short elementSize){
  Get(context, (void*)arrayLength, sizeof(unsigned short));
  void* array = (void*) &context->Buffer[context->CurrentIndex];
  if(context->IsLittleEndian && elementSize >= 2){
    unsigned short length = (*arrayLength);
    unsigned char* currentItem = (unsigned char*) array;
    unsigned char* temp = 0;
    uint16_t twoBytes;
    uint32_t fourBytes;
#ifdef uint64_t
    uint64_t eightBytes;
#endif

    while(length--){
      switch(elementSize){
      case 2:
        temp = (unsigned char*) &twoBytes;
        *(temp) = *(currentItem+1);
        *(temp+1) = *currentItem;
        *((uint16_t*)currentItem) = twoBytes;
        break;
      case 4:
        temp = (unsigned char*) &fourBytes;
        *(temp) = *(currentItem+3);
        *(temp+1) = *(currentItem+2);
        *(temp+2) = *(currentItem+1);
        *(temp+3) = *(currentItem);
        *((uint32_t*)currentItem) = fourBytes;
        break;
#ifdef uint64_t
      case 8:
        temp = (unsigned char*) &eightBytes;
        *(temp) = *(currentItem+7);
        *(temp+1) = *(currentItem+6);
        *(temp+2) = *(currentItem+5);
        *(temp+3) = *(currentItem+4);
        *(temp+4) = *(currentItem+3);
        *(temp+5) = *(currentItem+2);
        *(temp+6) = *(currentItem+1);
        *(temp+7) = *(currentItem);
        *((uint64_t*)currentItem) = eightBytes;
        break;
#endif
      }
      currentItem += elementSize;
    }
  }
  // Skip the length of the array to the next item...
  context->CurrentIndex += (*arrayLength) * elementSize;
  return array;
}
// Take a snapshot of the current serialization context
void TakeContextSnapshot(SerializerContext* contextSnapShot, SerializerContext* currentContext){
  memcpy((void*)contextSnapShot, (void*)currentContext, sizeof(SerializerContext));
}
// Restore the current serialization context from a snapshot
void RestoreContextFromSnapshot(SerializerContext* contextSnapShot, SerializerContext* currentContext){
  memcpy((void*)currentContext, (void*)contextSnapShot, sizeof(SerializerContext));
}

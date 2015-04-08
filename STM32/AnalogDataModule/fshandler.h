#ifndef _INC_FS_HANDLER_
#define _INC_FS_HANDLER_

#include "Nwazet.h"

void fsCommandHandlerReset();

void OnFileSystemCommand(
             SerializerContext* cmdContext,
             SerializerContext* respContext,
             unsigned char* responseBuffer,
             unsigned short responseBufferSize);
#endif

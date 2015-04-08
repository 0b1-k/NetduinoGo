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
#include "fshandler.h"
#include "Nwazet.h"
#include "ff.h"
#include "diskio.h"
#include <stdio.h>
#include "vt100.h"
#include "stdint.h"
#include "vt100.h"
#include <string.h>
#include "submessagehandlercontext.h"

static FATFS fs;        // File system object

typedef struct {
  FIL*          file;
  uint16_t      filenameHash;
  int16_t       id;
} fileObjectMap_t;

typedef struct {
  DIR*          dir;
  uint16_t      dirnameHash;
  int16_t       id;
} dirObjectMap_t;

#define INVALID_OBJECT_ID -1

static FIL      file0;
static FIL      file1;
static FIL      file2;
static FIL      file3;
static FIL      file4;

static DIR      rootDir;         // only used when mounting the file system
static DIR      dir0;
static DIR      dir1;
static DIR      dir2;
static DIR      dir3;
static DIR      dir4;

static fileObjectMap_t  fileObject0;
static fileObjectMap_t  fileObject1;
static fileObjectMap_t  fileObject2;
static fileObjectMap_t  fileObject3;
static fileObjectMap_t  fileObject4;

static dirObjectMap_t   dirObject0;
static dirObjectMap_t   dirObject1;
static dirObjectMap_t   dirObject2;
static dirObjectMap_t   dirObject3;
static dirObjectMap_t   dirObject4;

static fileObjectMap_t* fileObjectMap[_FS_SHARE] = {&fileObject0, &fileObject1, &fileObject2, &fileObject3, &fileObject4};
static dirObjectMap_t* dirObjectMap[_FS_SHARE] = {&dirObject0, &dirObject1, &dirObject2, &dirObject3, &dirObject4};

// File system commands
enum FsCommand {
    Mount,
    UnMount,
    OpenFile,
    CloseFile,
    ReadFile,
    WriteFile,
    SeekFile,
    TruncateFile,
    SyncFile,
    ReadString,
    WriteString,
    GetFilePosition,
    GetEndOfFile,
    GetFileSize,
    GetFileError,
    OpenDir,
    CloseDir,
    ReadDir,
    MakeDir,
    ChangeDir,
    GetCurrentDir,
    GetFreeSpace,
    Unlink,
    ChangeAttribute,
    ChangeTime,
    Rename,
    GetDiskStatus
};

const char* fsResultStrings[] = {
  "OK", /* (0) Succeeded */
  "Hardware disk I/O error", /* (1) A hard error occured in the low level disk I/O layer */
  "Assertion failed", /* (2) Assertion failed */
  "Physical drive unavailable", /* (3) The physical drive cannot do work */
  "File not found", /* (4) Could not find the file */
  "Path not found", /* (5) Could not find the path */
  "Invalid file/path name", /* (6) The path name format is invalid */
  "Access denied / directory full", /* (7) Acces denied due to prohibited access or directory full */
  "Access denied", /* (8) Acces denied due to prohibited access */
  "Invalid file/directory object", /* (9) The file/directory object is invalid */
  "Write protected", /* (10) The physical drive is write protected */
  "Invalid logical drive #", /* (11) The logical drive number is invalid */
  "No volume work area", /* (12) The volume has no work area */
  "Invalid FAT volume", /* (13) There is no valid FAT volume */
  "Invalid f_mkfs param. Aborted", /* (14) The f_mkfs() aborted due to any parameter error */
  "Timeout accessing volume", /* (15) Could not get a grant to access the volume within defined period */
  "File sharing violation", /* (16) The operation is rejected according to the file shareing policy */
  "LFN buffer alloc failed", /* (17) LFN working buffer could not be allocated */
  "Too many open files/dirs", /* (18) Number of open files > _FS_SHARE */
  "Invalid parameter", /* (19) Given parameter is invalid */
};

const char* fsResultLookupOutOfRange = "invalid result code";

const char* fsResultLookup(uint8_t res){
  if(res <= FR_INVALID_PARAMETER){
    return fsResultStrings[res];
  }else{
    return fsResultLookupOutOfRange;
  }
}
uint8_t fsCheckSuccess(const char* function, const char* msg, uint8_t res, const char* objName){
  if(res != FR_OK){
    vt100TextColor(VT100_Red, VT100_Black);
    if(objName){
      printf("fatFs: %s(%s): %s (%i), object: %s\r\n", function, msg, fsResultLookup(res), res, objName);
    }else{
      printf("fatFs: %s(%s): %s (%i)\r\n", function, msg, fsResultLookup(res), res);
    }
    vt100TextDefault();
  }
  return res;
}
void fsCommandHandlerReset(){
  PrintNeutralMessage("File system reset");

  // SD card state reset
  disk_reset();

  // Setup the file objects
  fileObject0.file = &file0;
  fileObject0.filenameHash = 0;
  fileObject0.id = INVALID_OBJECT_ID;

  fileObject1.file = &file1;
  fileObject1.filenameHash = 0;
  fileObject1.id = INVALID_OBJECT_ID;

  fileObject2.file = &file2;
  fileObject2.filenameHash = 0;
  fileObject2.id = INVALID_OBJECT_ID;

  fileObject3.file = &file3;
  fileObject3.filenameHash = 0;
  fileObject3.id = INVALID_OBJECT_ID;

  fileObject4.file = &file4;
  fileObject4.filenameHash = 0;
  fileObject4.id = INVALID_OBJECT_ID;

  // Setup the directory objects
  dirObject0.dir = &dir0;
  dirObject0.dirnameHash = 0;
  dirObject0.id = INVALID_OBJECT_ID;

  dirObject1.dir = &dir1;
  dirObject1.dirnameHash = 0;
  dirObject1.id = INVALID_OBJECT_ID;

  dirObject2.dir = &dir2;
  dirObject2.dirnameHash = 0;
  dirObject2.id = INVALID_OBJECT_ID;

  dirObject3.dir = &dir3;
  dirObject3.dirnameHash = 0;
  dirObject3.id = INVALID_OBJECT_ID;

  dirObject4.dir = &dir4;
  dirObject4.dirnameHash = 0;
  dirObject4.id = INVALID_OBJECT_ID;

  PrintSuccessMessage("File system reset ok");
}
fileObjectMap_t* MapFilenameToFileObject(const char* filename, int length){
  fileObjectMap_t* fileObject = 0;
  uint16_t crc = crc16((const uint8_t*) filename, length);
  for(int id = 0; id < _FS_SHARE; id++){
    fileObject = fileObjectMap[id];
    if(fileObject->filenameHash == crc){
      fsCheckSuccess(__func__, "file already open", FR_INVALID_OBJECT, filename);
      return 0;
    }
    if(fileObject->id == INVALID_OBJECT_ID){
      fileObject->filenameHash = crc;
      fileObject->id = id;
      return fileObject;
    }
  }
  fsCheckSuccess(__func__, "file mapping", FR_TOO_MANY_OPEN_FILES, filename);
  return 0;
}
dirObjectMap_t* MapDirectoryNameToDirectoryObject(const char* dirname, int length){
  dirObjectMap_t* dirObject = 0;
  uint16_t crc = crc16((const uint8_t*) dirname, length);
  for(int id = 0; id < _FS_SHARE; id++){
    dirObject = dirObjectMap[id];
    if(dirObject->dirnameHash == crc){
      fsCheckSuccess(__func__, "directory already open", FR_INVALID_OBJECT, dirname);
      return 0;
    }
    if(dirObject->id == INVALID_OBJECT_ID){
      dirObject->dirnameHash = crc;
      dirObject->id = id;
      return dirObject;
    }
  }
  fsCheckSuccess(__func__, "directory mapping", FR_TOO_MANY_OPEN_FILES, dirname);
  return 0;
}
void OnMount(nwazetMessageContext_t* nmc){
  uint8_t res = fsCheckSuccess(__func__, "f_mount(root)", f_mount(0, &fs), 0);
  if(res == FR_OK){
    // Attempt to initialize the file system by mounting the root of the drive
    res = fsCheckSuccess(__func__, "f_opendir(root)", f_opendir(&rootDir, ""), 0);
  }
  StartResponse(nmc, res, true);
}
void CloseAllOpenFiles(){
  for(int f = 0; f < _FS_SHARE; f++){
    fileObjectMap_t* fileObj = fileObjectMap[f];
    if(fileObj->id != INVALID_OBJECT_ID && fileObj->file->fs){
      f_close(fileObj->file);
    }
  }
}
void OnUnMount(nwazetMessageContext_t* nmc){
  // Ensure that all files are closed before unmounting the file system
  CloseAllOpenFiles();
  // Unmount the file system
  uint8_t res = fsCheckSuccess(__func__, "f_mount(null)", f_mount(0, 0), 0);
  // low-level disk reset
  disk_reset();
  // Reset file object mappings
  fsCommandHandlerReset();
  // return a response to the caller
  StartResponse(nmc, res, true);
}
fileObjectMap_t* GetFileObjectById(nwazetMessageContext_t* nmc){
  uint16_t fileId = 0;
  Get(nmc->cmdContext, (void*)&fileId, sizeof(fileId));
  if(fileId < _FS_SHARE){
    return fileObjectMap[fileId];
  }
  StartResponse(nmc, FR_INVALID_OBJECT, true);
  return 0;
}
dirObjectMap_t* GetDirectoryObjectById(nwazetMessageContext_t* nmc){
  uint16_t dirId = 0;
  Get(nmc->cmdContext, (void*)&dirId, sizeof(dirId));
  if(dirId < _FS_SHARE){
    return dirObjectMap[dirId];
  }
  StartResponse(nmc, FR_INVALID_OBJECT, true);
  return 0;
}
void OnOpenFile(nwazetMessageContext_t* nmc){
  uint8_t res = (uint8_t) FR_OK;
  uint8_t isTextASCII = 0;
  uint16_t textLength = 0;
  uint8_t modeMask = 0;
  fileObjectMap_t* fileObj = 0;
  const char* filename = (const char*)GetStringReference(nmc->cmdContext, &textLength, &isTextASCII);
  Get(nmc->cmdContext, (void*)&modeMask, sizeof(modeMask));
  if(filename && isTextASCII){
    fileObj = MapFilenameToFileObject(filename, textLength);
    if(fileObj){
      res = fsCheckSuccess(__func__, "f_open", f_open(fileObj->file, filename, modeMask), filename);
      StartResponse(nmc, res, false);
      Put(nmc->respContext, (void*)&fileObj->id, sizeof(fileObj->id), 1);
      EndResponse(nmc);
      return;
    }
  }
  res = FR_INVALID_NAME;
  fsCheckSuccess(__func__, "filename", res, filename);
  StartResponse(nmc, res, true);
}
void OnCloseFile(nwazetMessageContext_t* nmc){
  fileObjectMap_t* fileObj = GetFileObjectById(nmc);
  if(fileObj){
    fileObj->filenameHash = 0;
    fileObj->id = INVALID_OBJECT_ID;
    StartResponse(nmc, fsCheckSuccess(__func__, "f_close", f_close(fileObj->file), 0), true);
  }
}
void OnReadFile(nwazetMessageContext_t* nmc){
  SerializerContext snapshot;
  uint16_t bytesToRead = 0;
  uint16_t bytesRead = 0;
  uint8_t res = (uint8_t) FR_OK;
  fileObjectMap_t* fileObj = GetFileObjectById(nmc);
  if(fileObj){
    Get(nmc->cmdContext, (void*)&bytesToRead, sizeof(bytesToRead));
    if(bytesToRead > nmc->responseBufferSize){
      fsCheckSuccess(__func__, "bytesToRead", FR_INVALID_PARAMETER, 0);
      StartResponse(nmc, FR_INVALID_PARAMETER, true);
    }else{
      InitializeSendBuffer(nmc->respContext, nmc->responseBuffer, nmc->responseBufferSize);
      TakeContextSnapshot(&snapshot, nmc->respContext);
      Put(nmc->respContext, (void*)&res, sizeof(res), 1);
      Put(nmc->respContext, (void*)&bytesRead, sizeof(bytesRead), 1);

      UINT bytesReadTemp = 0;
      res = (uint8_t) f_read(fileObj->file, (void*)(nmc->respContext->Buffer + nmc->respContext->CurrentIndex), bytesToRead, &bytesReadTemp);
      fsCheckSuccess(__func__, "f_read", res, 0);
      bytesRead = (uint16_t)bytesReadTemp;

      RestoreContextFromSnapshot(&snapshot, nmc->respContext);
      Put(nmc->respContext, (void*)&res, sizeof(res), 1);
      Put(nmc->respContext, (void*)&bytesRead, sizeof(bytesRead), 1);
      FinalizeSendBuffer(nmc->respContext);
    }
  }
}
void OnWriteFile(nwazetMessageContext_t* nmc){
  uint8_t res = (uint8_t) FR_OK;
  uint16_t bytesToWrite = 0;
  uint16_t bytesWrittenTemp = 0;
  UINT bytesWritten = 0;
  fileObjectMap_t* fileObj = GetFileObjectById(nmc);
  if(fileObj){
    Get(nmc->cmdContext, (void*)&bytesToWrite, sizeof(bytesToWrite));
    if(bytesToWrite > _MAX_SS){
      fsCheckSuccess(__func__, "bytesToWrite", FR_INVALID_PARAMETER, 0);
      StartResponse(nmc, FR_INVALID_PARAMETER, true);
    }else{
      res = (uint8_t) f_write(fileObj->file, (void*)(nmc->cmdContext->Buffer + nmc->cmdContext->CurrentIndex), (UINT)bytesToWrite, &bytesWritten);
      fsCheckSuccess(__func__, "f_write", res, 0);

      bytesWrittenTemp = (uint16_t)bytesWritten;

      InitializeSendBuffer(nmc->respContext, nmc->responseBuffer, nmc->responseBufferSize);
      Put(nmc->respContext, (void*)&res, sizeof(res), 1);
      Put(nmc->respContext, (void*)&bytesWrittenTemp, sizeof(bytesWrittenTemp), 1);
      FinalizeSendBuffer(nmc->respContext);
    }
  }
}
void OnSeekFile(nwazetMessageContext_t* nmc){
  uint8_t res = (uint8_t) FR_OK;
  uint32_t bytesToSeek = 0;
  fileObjectMap_t* fileObj = GetFileObjectById(nmc);
  if(fileObj){
    Get(nmc->cmdContext, (void*)&bytesToSeek, sizeof(bytesToSeek));
    res = (uint8_t) f_lseek(fileObj->file, bytesToSeek);
    fsCheckSuccess(__func__, "f_lseek", res, 0);
    StartResponse(nmc, res, true);
  }
}
void OnTruncateFile(nwazetMessageContext_t* nmc){
  uint8_t res = (uint8_t) FR_OK;
  fileObjectMap_t* fileObj = GetFileObjectById(nmc);
  if(fileObj){
    res = (uint8_t) f_truncate(fileObj->file);
    fsCheckSuccess(__func__, "f_truncate", res, 0);
    StartResponse(nmc, res, true);
  }
}
void OnSyncFile(nwazetMessageContext_t* nmc){
  uint8_t res = (uint8_t) FR_OK;
  fileObjectMap_t* fileObj = GetFileObjectById(nmc);
  if(fileObj){
    res = (uint8_t) f_sync(fileObj->file);
    fsCheckSuccess(__func__, "f_sync", res, 0);
    StartResponse(nmc, res, true);
  }
}
#define MAX_FGETS_BUFFER_LENGTH_IN_BYTES 512

void OnReadString(nwazetMessageContext_t* nmc){
  SerializerContext snapshot;
  uint16_t characterCount = 0;
  uint16_t bufferSizeInCharacters = MAX_FGETS_BUFFER_LENGTH_IN_BYTES / sizeof(TCHAR);
  uint8_t isTextASCII = 1;
  uint8_t res = (uint8_t) FR_OK;
  uint8_t terminator = 0;
  fileObjectMap_t* fileObj = GetFileObjectById(nmc);
  if(fileObj){
    InitializeSendBuffer(nmc->respContext, nmc->responseBuffer, nmc->responseBufferSize);
    Put(nmc->respContext, (void*)&res, sizeof(res), 1);
    Put(nmc->respContext, (void*)&isTextASCII, sizeof(isTextASCII), 1);
    TakeContextSnapshot(&snapshot, nmc->respContext);
    Put(nmc->respContext, (void*)&characterCount, sizeof(characterCount), 1);
    TCHAR* strBuffer = (TCHAR*)(nmc->respContext->Buffer + nmc->respContext->CurrentIndex);
    Put(nmc->respContext, (void*)&terminator, sizeof(terminator), 1); // will get overwritten by the string or will stay if f_gets fails

    TCHAR* str = f_gets(strBuffer, bufferSizeInCharacters, fileObj->file);
    if(str){
      characterCount = strnlen(str, bufferSizeInCharacters);
      nmc->respContext->CurrentIndex += characterCount;
    }

    RestoreContextFromSnapshot(&snapshot, nmc->respContext);
    Put(nmc->respContext, (void*)&characterCount, sizeof(characterCount), 1);
    FinalizeSendBuffer(nmc->respContext);
  }
}
void OnWriteString(nwazetMessageContext_t* nmc){
  fileObjectMap_t* fileObj = GetFileObjectById(nmc);
  if(fileObj){
    uint8_t isTextASCII = 0;
    uint16_t textLength = 0;
    void* text = GetStringReference(nmc->cmdContext, &textLength, &isTextASCII);
    if(text && textLength){
      int32_t charactersWritten = f_puts((const TCHAR*) text, fileObj->file);
      StartResponse(nmc, FR_OK, false);
      Put(nmc->respContext, (void*)&charactersWritten, sizeof(charactersWritten), 1);
      EndResponse(nmc);
    }
  }
}
void OnGetFilePosition(nwazetMessageContext_t* nmc){
  fileObjectMap_t* fileObj = GetFileObjectById(nmc);
  if(fileObj){
    uint32_t position = f_tell(fileObj->file);
    StartResponse(nmc, FR_OK, false);
    Put(nmc->respContext, (void*)&position, sizeof(position), 1);
    EndResponse(nmc);
  }
}
void OnGetEndOfFile(nwazetMessageContext_t* nmc){
  fileObjectMap_t* fileObj = GetFileObjectById(nmc);
  if(fileObj){
    uint8_t eof = f_eof(fileObj->file);
    StartResponse(nmc, FR_OK, false);
    Put(nmc->respContext, (void*)&eof, sizeof(eof), 1);
    EndResponse(nmc);
  }
}
void OnGetFileSize(nwazetMessageContext_t* nmc){
  fileObjectMap_t* fileObj = GetFileObjectById(nmc);
  if(fileObj){
    uint32_t size = f_size(fileObj->file);
    StartResponse(nmc, FR_OK, false);
    Put(nmc->respContext, (void*)&size, sizeof(size), 1);
    EndResponse(nmc);
  }
}
void OnGetFileError(nwazetMessageContext_t* nmc){
  fileObjectMap_t* fileObj = GetFileObjectById(nmc);
  if(fileObj){
    uint8_t error = f_error(fileObj->file);
    StartResponse(nmc, FR_OK, false);
    Put(nmc->respContext, (void*)&error, sizeof(error), 1);
    EndResponse(nmc);
  }
}
void OnOpenDir(nwazetMessageContext_t* nmc){
  uint8_t res = (uint8_t) FR_OK;
  uint8_t isTextASCII = 0;
  uint16_t textLength = 0;
  dirObjectMap_t* dirObj = 0;
  const char* dirname = (const char*)GetStringReference(nmc->cmdContext, &textLength, &isTextASCII);
  if(dirname && isTextASCII){
    dirObj = MapDirectoryNameToDirectoryObject(dirname, textLength);
    if(dirObj){
      res = fsCheckSuccess(__func__, "f_opendir", f_opendir(dirObj->dir, dirname), dirname);
      StartResponse(nmc, res, false);
      Put(nmc->respContext, (void*)&dirObj->id, sizeof(dirObj->id), 1);
      EndResponse(nmc);
      return;
    }
  }
  res = FR_INVALID_NAME;
  fsCheckSuccess(__func__, "dirname", res, dirname);
  StartResponse(nmc, res, true);
}
void OnCloseDir(nwazetMessageContext_t* nmc){
  dirObjectMap_t* dirObj = GetDirectoryObjectById(nmc);
  if(dirObj){
    dirObj->dirnameHash = 0;
    dirObj->id = INVALID_OBJECT_ID;
    StartResponse(nmc, FR_OK, true);
  }
}
void OnReadDir(nwazetMessageContext_t* nmc){
  dirObjectMap_t* dirObj = GetDirectoryObjectById(nmc);
  if(dirObj){
    FILINFO fileInfo;
    uint8_t res = fsCheckSuccess(__func__, "f_readdir", f_readdir(dirObj->dir, &fileInfo), 0);
    if(res == FR_OK){
      StartResponse(nmc, res, false);
      Put(nmc->respContext, (void*)&fileInfo.fsize, sizeof(uint32_t), 1);
      Put(nmc->respContext, (void*)&fileInfo.fdate, sizeof(uint16_t), 1);
      Put(nmc->respContext, (void*)&fileInfo.ftime, sizeof(uint16_t), 1);
      Put(nmc->respContext, (void*)&fileInfo.fattrib, sizeof(uint8_t), 1);
      PutStringReference(nmc->respContext, (unsigned char*)&fileInfo.fname[0], 13, 1);
      EndResponse(nmc);
    }else{
      StartResponse(nmc, res, true);
    }
  }
}
void OnMakeDir(nwazetMessageContext_t* nmc){
  uint8_t res = (uint8_t) FR_OK;
  uint8_t isTextASCII = 0;
  uint16_t textLength = 0;
  const char* dirname = (const char*)GetStringReference(nmc->cmdContext, &textLength, &isTextASCII);
  if(dirname && isTextASCII){
    res = fsCheckSuccess(__func__, "f_mkdir", f_mkdir(dirname), dirname);
    StartResponse(nmc, res, true);
    return;
  }
  res = FR_INVALID_NAME;
  fsCheckSuccess(__func__, "dirname", res, dirname);
  StartResponse(nmc, res, true);
}
void OnChangeDir(nwazetMessageContext_t* nmc){
  uint8_t res = (uint8_t) FR_OK;
  uint8_t isTextASCII = 0;
  uint16_t textLength = 0;
  const char* dirname = (const char*)GetStringReference(nmc->cmdContext, &textLength, &isTextASCII);
  if(dirname && isTextASCII){
    res = fsCheckSuccess(__func__, "f_chdir", f_chdir(dirname), dirname);
    StartResponse(nmc, res, true);
    return;
  }
  res = FR_INVALID_NAME;
  fsCheckSuccess(__func__, "dirname", res, dirname);
  StartResponse(nmc, res, true);
}
void OnGetCurrentDir(nwazetMessageContext_t* nmc){
  SerializerContext snapshot;
  uint16_t characterCount = 0;
  uint16_t bufferSizeInCharacters = MAX_FGETS_BUFFER_LENGTH_IN_BYTES / sizeof(TCHAR);
  uint8_t isTextASCII = 1;
  uint8_t res = (uint8_t) FR_OK;
  uint8_t terminator = 0;

  InitializeSendBuffer(nmc->respContext, nmc->responseBuffer, nmc->responseBufferSize);
  Put(nmc->respContext, (void*)&res, sizeof(res), 1);
  Put(nmc->respContext, (void*)&isTextASCII, sizeof(isTextASCII), 1);
  TakeContextSnapshot(&snapshot, nmc->respContext);
  Put(nmc->respContext, (void*)&characterCount, sizeof(characterCount), 1);
  TCHAR* strBuffer = ((TCHAR*)(nmc->respContext->Buffer + nmc->respContext->CurrentIndex));
  Put(nmc->respContext, (void*)&terminator, sizeof(terminator), 1); // will get overwritten by the string or will stay if f_getcwd fails

  res = fsCheckSuccess(__func__, "f_getcwd", f_getcwd(strBuffer, bufferSizeInCharacters), strBuffer);
  if(res == FR_OK){
    characterCount = strnlen(strBuffer, bufferSizeInCharacters);
    nmc->respContext->CurrentIndex += characterCount;
  }
  RestoreContextFromSnapshot(&snapshot, nmc->respContext);
  Put(nmc->respContext, (void*)&characterCount, sizeof(characterCount), 1);
  FinalizeSendBuffer(nmc->respContext);
}
void OnGetFreeSpace(nwazetMessageContext_t* nmc){
  DWORD clusters = 0;
  FATFS* tempFs = &fs;
  uint8_t res = fsCheckSuccess(__func__, "f_getfree", f_getfree("", &clusters, &tempFs), 0);
  if(res == FR_OK){
    DWORD totalSectors = (fs.n_fatent - 2) * fs.csize;
    DWORD freeSectors = clusters * fs.csize;
    uint32_t totalDiskSpaceKB = totalSectors / 2; // 1024 / 2 = 512 bytes / sector with SD card
    uint32_t totalFreeSpaceKB = freeSectors / 2; // same as above
    InitializeSendBuffer(nmc->respContext, nmc->responseBuffer, nmc->responseBufferSize);
    Put(nmc->respContext, (void*)&res, sizeof(res), 1);
    Put(nmc->respContext, (void*)&totalDiskSpaceKB, sizeof(totalDiskSpaceKB), 1);
    Put(nmc->respContext, (void*)&totalFreeSpaceKB, sizeof(totalFreeSpaceKB), 1);
    FinalizeSendBuffer(nmc->respContext);
  }else{
    StartResponse(nmc, res, true);
  }
}
void OnUnlink(nwazetMessageContext_t* nmc){
  uint8_t res = (uint8_t) FR_OK;
  uint8_t isTextASCII = 0;
  uint16_t textLength = 0;
  const char* objName = (const char*)GetStringReference(nmc->cmdContext, &textLength, &isTextASCII);
  if(objName && isTextASCII){
    res = fsCheckSuccess(__func__, "f_unlink", f_unlink(objName), objName);
    StartResponse(nmc, res, true);
    return;
  }
  res = FR_INVALID_NAME;
  fsCheckSuccess(__func__, "objName", res, objName);
  StartResponse(nmc, res, true);
}
void OnChangeAttribute(nwazetMessageContext_t* nmc){
  uint8_t res = (uint8_t) FR_OK;
  uint8_t attribute = 0;
  uint8_t attributeMask = 0;
  uint8_t isTextASCII = 0;
  uint16_t textLength = 0;
  const char* objName = (const char*)GetStringReference(nmc->cmdContext, &textLength, &isTextASCII);
  Get(nmc->cmdContext, (void*)&attribute, sizeof(attribute));
  Get(nmc->cmdContext, (void*)&attributeMask, sizeof(attributeMask));
  if(objName && isTextASCII){
    res = fsCheckSuccess(__func__, "f_chmod", f_chmod(objName, attribute, attributeMask), objName);
    StartResponse(nmc, res, true);
    return;
  }
  res = FR_INVALID_NAME;
  fsCheckSuccess(__func__, "objName", res, objName);
  StartResponse(nmc, res, true);
}
void OnChangeTime(nwazetMessageContext_t* nmc){
  FILINFO fileInfo;
  uint8_t res = (uint8_t) FR_OK;
  uint8_t isTextASCII = 0;
  uint16_t year = 0;
  uint16_t month = 0;
  uint16_t day = 0;
  uint16_t hour = 0;
  uint16_t min = 0;
  uint16_t sec = 0;
  uint16_t textLength = 0;

  const char* objName = (const char*)GetStringReference(nmc->cmdContext, &textLength, &isTextASCII);
  Get(nmc->cmdContext, (void*)&year, sizeof(year));
  Get(nmc->cmdContext, (void*)&month, sizeof(month));
  Get(nmc->cmdContext, (void*)&day, sizeof(day));
  Get(nmc->cmdContext, (void*)&hour, sizeof(hour));
  Get(nmc->cmdContext, (void*)&min, sizeof(min));
  Get(nmc->cmdContext, (void*)&sec, sizeof(sec));

  fileInfo.fdate = (uint16_t)(((year - 1980) * 512U) | month * 32U | day);
  fileInfo.ftime = (uint16_t)(hour * 2048U | min * 32U | sec / 2U);

  if(objName && isTextASCII){
    res = fsCheckSuccess(__func__, "f_utime", f_utime(objName, &fileInfo), objName);
    StartResponse(nmc, res, true);
    return;
  }
  res = FR_INVALID_NAME;
  fsCheckSuccess(__func__, "objName", res, objName);
  StartResponse(nmc, res, true);
}
void OnRename(nwazetMessageContext_t* nmc){
  uint8_t res = (uint8_t) FR_OK;
  uint8_t isTextASCII = 0;
  uint16_t textLength = 0;
  const char* oldObjName = (const char*)GetStringReference(nmc->cmdContext, &textLength, &isTextASCII);
  if(!oldObjName || !isTextASCII){
    res = FR_INVALID_NAME;
    fsCheckSuccess(__func__, "oldObjName", res, oldObjName);
    StartResponse(nmc, res, true);
    return;
  }
  const char* newObjName = (const char*)GetStringReference(nmc->cmdContext, &textLength, &isTextASCII);
  if(!newObjName || !isTextASCII){
    res = FR_INVALID_NAME;
    fsCheckSuccess(__func__, "newObjName", res, newObjName);
    StartResponse(nmc, res, true);
    return;
  }
  res = fsCheckSuccess(__func__, "f_rename", f_rename(oldObjName, newObjName), oldObjName);
  StartResponse(nmc, res, true);
}
void OnGetDiskStatus(nwazetMessageContext_t* nmc){
  uint8_t res = (uint8_t) FR_OK;
  uint8_t status = disk_status(fs.drv);
  InitializeSendBuffer(nmc->respContext, nmc->responseBuffer, nmc->responseBufferSize);
  Put(nmc->respContext, (void*)&res, sizeof(res), 1);
  Put(nmc->respContext, (void*)&status, sizeof(status), 1);
  FinalizeSendBuffer(nmc->respContext);
}

static nwazetMessageContext_t nmc;

void OnFileSystemCommand(
                        SerializerContext* cmdContext,
                        SerializerContext* respContext,
                        unsigned char* responseBuffer,
                        unsigned short responseBufferSize)
{
  nmc.cmdContext = cmdContext;
  nmc.respContext = respContext;
  nmc.responseBuffer = responseBuffer;
  nmc.responseBufferSize = responseBufferSize;

  unsigned char fsCmd = 0;
  Get(cmdContext, (void*)&fsCmd, sizeof(fsCmd));
  switch(fsCmd){
  case Mount:
    OnMount(&nmc);
    break;
  case UnMount:
    OnUnMount(&nmc);
    break;
  case OpenFile:
    OnOpenFile(&nmc);
    break;
  case CloseFile:
    OnCloseFile(&nmc);
    break;
  case ReadFile:
    OnReadFile(&nmc);
    break;
  case WriteFile:
    OnWriteFile(&nmc);
    break;
  case SeekFile:
    OnSeekFile(&nmc);
    break;
  case TruncateFile:
    OnTruncateFile(&nmc);
    break;
  case SyncFile:
    OnSyncFile(&nmc);
    break;
  case ReadString:
    OnReadString(&nmc);
    break;
  case WriteString:
    OnWriteString(&nmc);
    break;
  case GetFilePosition:
    OnGetFilePosition(&nmc);
    break;
  case GetEndOfFile:
    OnGetEndOfFile(&nmc);
    break;
  case GetFileSize:
    OnGetFileSize(&nmc);
    break;
  case GetFileError:
    OnGetFileError(&nmc);
    break;
  case OpenDir:
    OnOpenDir(&nmc);
    break;
  case CloseDir:
    OnCloseDir(&nmc);
    break;
  case ReadDir:
    OnReadDir(&nmc);
    break;
  case MakeDir:
    OnMakeDir(&nmc);
    break;
  case ChangeDir:
    OnChangeDir(&nmc);
    break;
  case GetCurrentDir:
    OnGetCurrentDir(&nmc);
    break;
  case GetFreeSpace:
    OnGetFreeSpace(&nmc);
    break;
  case Unlink:
    OnUnlink(&nmc);
    break;
  case ChangeAttribute:
    OnChangeAttribute(&nmc);
    break;
  case ChangeTime:
    OnChangeTime(&nmc);
    break;
  case Rename:
    OnRename(&nmc);
    break;
  case GetDiskStatus:
    OnGetDiskStatus(&nmc);
    break;
  default:
    vt100TextColor(VT100_Red, VT100_Black);
    printf("%s: invalid command: %i\r\n", __func__, (int)fsCmd);
    vt100TextDefault();
    break;
  }
}

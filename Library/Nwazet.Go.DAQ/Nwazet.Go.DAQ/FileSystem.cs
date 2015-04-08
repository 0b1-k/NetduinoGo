using System;
using Nwazet.Go.Helpers;
namespace Nwazet.Go.DAQ {
    public class FileSystem : IDisposable {
        // File function return code
        protected enum Fresult {
            FR_OK = 0,				/* (0) Succeeded */
            FR_DISK_ERR,			/* (1) A hard error occured in the low level disk I/O layer */
            FR_INT_ERR,				/* (2) Assertion failed */
            FR_NOT_READY,			/* (3) The physical drive cannot work */
            FR_NO_FILE,				/* (4) Could not find the file */
            FR_NO_PATH,				/* (5) Could not find the path */
            FR_INVALID_NAME,		/* (6) The path name format is invalid */
            FR_DENIED,				/* (7) Acces denied due to prohibited access or directory full */
            FR_EXIST,				/* (8) Acces denied due to prohibited access */
            FR_INVALID_OBJECT,		/* (9) The file/directory object is invalid */
            FR_WRITE_PROTECTED,		/* (10) The physical drive is write protected */
            FR_INVALID_DRIVE,		/* (11) The logical drive number is invalid */
            FR_NOT_ENABLED,			/* (12) The volume has no work area */
            FR_NO_FILESYSTEM,		/* (13) There is no valid FAT volume */
            FR_MKFS_ABORTED,		/* (14) The f_mkfs() aborted due to any parameter error */
            FR_TIMEOUT,				/* (15) Could not get a grant to access the volume within defined period */
            FR_LOCKED,				/* (16) The operation is rejected according to the file shareing policy */
            FR_NOT_ENOUGH_CORE,		/* (17) LFN working buffer could not be allocated */
            FR_TOO_MANY_OPEN_FILES,	/* (18) Number of open files > _FS_SHARE */
            FR_INVALID_PARAMETER	/* (19) Given parameter is invalid */
        }
        // File system commands
        protected enum Command {
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
        }
        public enum FileMode {
            OpenExisting = 0x00,
            Read = 0x01,
            Write = 0x02,
            CreateNew = 0x04,
            CreateAlways = 0x08,
            OpenAlways = 0x10
        }

        protected NwazetDAQ Daq;
        protected BasicTypeSerializerContext SendContext;
        protected BasicTypeDeSerializerContext ReceiveContext;
        public const UInt16 InvalidFileId = 255;

        public FileSystem(
            NwazetDAQ daq,
            BasicTypeSerializerContext sendContext,
            BasicTypeDeSerializerContext receiveContext){
            Daq = daq;
            SendContext = sendContext;
            ReceiveContext = receiveContext;
        }
        ~FileSystem() {
            Dispose();
        }
        protected string[] ResultStrings = {
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
        protected void CheckResult() {
            byte result = BasicTypeDeSerializer.Get(ReceiveContext);
            if (result != 0) {
                throw new FatFsException((int)result, ResultStrings[result]);
            }
        }
        //
        // Volume functions
        //
        public void Mount() {
            lock (Daq) {
                BasicTypeSerializer.Put(SendContext, (byte)NwazetDAQ.Command.FileSystem);
                BasicTypeSerializer.Put(SendContext, (byte)Command.Mount);
                Daq.Execute();
                Daq.Receive();
                CheckResult();
            }
        }
        public void UnMount() {
            lock (Daq) {
                BasicTypeSerializer.Put(SendContext, (byte)NwazetDAQ.Command.FileSystem);
                BasicTypeSerializer.Put(SendContext, (byte)Command.UnMount);
                Daq.Execute();
                Daq.Receive();
                CheckResult();
            }
        }
        public enum DiskStatus {
            Ok = 0x00,
            HardwareNotInitialized = 0x01,
            NoDisk = 0x02,
            DiskWriteProtected = 0x04
        }
        public DiskStatus GetDiskStatus() {
            lock (Daq) {
                BasicTypeSerializer.Put(SendContext, (byte)NwazetDAQ.Command.FileSystem);
                BasicTypeSerializer.Put(SendContext, (byte)Command.GetDiskStatus);
                Daq.Execute();
                Daq.Receive();
                CheckResult();
                DiskStatus diskResult = (DiskStatus)BasicTypeDeSerializer.Get(ReceiveContext);
                return diskResult;
            }
        }
        //
        // File functions
        //
        public UInt16 OpenFile(string filename, FileMode mode) {
            lock (Daq) {
                BasicTypeSerializer.Put(SendContext, (byte)NwazetDAQ.Command.FileSystem);
                BasicTypeSerializer.Put(SendContext, (byte)Command.OpenFile);
                BasicTypeSerializer.Put(SendContext, filename, true);
                BasicTypeSerializer.Put(SendContext, (byte)mode);
                Daq.Execute();
                Daq.Receive();
                CheckResult();
                UInt16 fileId = 0;
                fileId = BasicTypeDeSerializer.Get(ReceiveContext, fileId);
                return fileId;
            }
        }
        public void CloseFile(UInt16 fileId) {
            lock (Daq) {
                BasicTypeSerializer.Put(SendContext, (byte)NwazetDAQ.Command.FileSystem);
                BasicTypeSerializer.Put(SendContext, (byte)Command.CloseFile);
                BasicTypeSerializer.Put(SendContext, fileId);
                Daq.Execute();
                Daq.Receive();
                CheckResult();
            }
        }
        protected const int MaxByteChunk = 512;
        public UInt16 ReadFile(UInt16 fileId, byte[] buffer, int bytesToRead) {
            lock (Daq) {
                int byteCount = bytesToRead;
                UInt16 bytesRead = 0;
                UInt16 totalBytesRead = 0;
                int offset = 0;
                while (byteCount != 0) {
                    UInt16 byteChunk = (UInt16)Math.Min(MaxByteChunk, byteCount);
                    BasicTypeSerializer.Put(SendContext, (byte)NwazetDAQ.Command.FileSystem);
                    BasicTypeSerializer.Put(SendContext, (byte)Command.ReadFile);
                    BasicTypeSerializer.Put(SendContext, fileId);
                    BasicTypeSerializer.Put(SendContext, byteChunk);
                    Daq.Execute();
                    Daq.Receive();
                    CheckResult();
                    bytesRead = BasicTypeDeSerializer.Get(ReceiveContext, bytesRead);
                    ReceiveContext.CopyBytesFromInternalBuffer(buffer, offset, bytesRead);
                    totalBytesRead += bytesRead;
                    offset += bytesRead;
                    byteCount -= bytesRead;
                    if (bytesRead < byteChunk) {
                        break;
                    }
                }
                return totalBytesRead;
            }
        }
        public UInt16 WriteFile(UInt16 fileId, byte[] buffer, int bytesToWrite) {
            lock (Daq) {
                int byteCount = bytesToWrite;
                UInt16 bytesWritten = 0;
                UInt16 totalBytesWritten = 0;
                int offset = 0;
                while (byteCount != 0) {
                    UInt16 byteChunk = (UInt16)Math.Min(MaxByteChunk, byteCount);
                    BasicTypeSerializer.Put(SendContext, (byte)NwazetDAQ.Command.FileSystem);
                    BasicTypeSerializer.Put(SendContext, (byte)Command.WriteFile);
                    BasicTypeSerializer.Put(SendContext, fileId);
                    BasicTypeSerializer.Put(SendContext, byteChunk);
                    SendContext.Store(buffer, offset, byteChunk);
                    Daq.Execute();
                    Daq.Receive();
                    CheckResult();
                    bytesWritten = BasicTypeDeSerializer.Get(ReceiveContext, bytesWritten);
                    totalBytesWritten += bytesWritten;
                    offset += bytesWritten;
                    byteCount -= bytesWritten;
                }
                return totalBytesWritten;
            }
        }
        public void SeekFile(UInt16 fileId, UInt32 offset) {
            lock (Daq) {
                BasicTypeSerializer.Put(SendContext, (byte)NwazetDAQ.Command.FileSystem);
                BasicTypeSerializer.Put(SendContext, (byte)Command.SeekFile);
                BasicTypeSerializer.Put(SendContext, fileId);
                BasicTypeSerializer.Put(SendContext, offset);
                Daq.Execute();
                Daq.Receive();
                CheckResult();
            }
        }
        public void TruncateFile(UInt16 fileId) {
            lock (Daq) {
                BasicTypeSerializer.Put(SendContext, (byte)NwazetDAQ.Command.FileSystem);
                BasicTypeSerializer.Put(SendContext, (byte)Command.TruncateFile);
                BasicTypeSerializer.Put(SendContext, fileId);
                Daq.Execute();
                Daq.Receive();
                CheckResult();
            }
        }
        public void SyncFile(UInt16 fileId) {
            lock (Daq) {
                BasicTypeSerializer.Put(SendContext, (byte)NwazetDAQ.Command.FileSystem);
                BasicTypeSerializer.Put(SendContext, (byte)Command.SyncFile);
                BasicTypeSerializer.Put(SendContext, fileId);
                Daq.Execute();
                Daq.Receive();
                CheckResult();
            }
        }
        public string ReadString(UInt16 fileId) {
            lock (Daq) {
                BasicTypeSerializer.Put(SendContext, (byte)NwazetDAQ.Command.FileSystem);
                BasicTypeSerializer.Put(SendContext, (byte)Command.ReadString);
                BasicTypeSerializer.Put(SendContext, fileId);
                Daq.Execute();
                Daq.Receive();
                CheckResult();
                string str = "";
                str = BasicTypeDeSerializer.Get(ReceiveContext, str);
                return str;
            }
        }
        public Int32 WriteString(UInt16 fileId, string str) {
            lock (Daq) {
                BasicTypeSerializer.Put(SendContext, (byte)NwazetDAQ.Command.FileSystem);
                BasicTypeSerializer.Put(SendContext, (byte)Command.WriteString);
                BasicTypeSerializer.Put(SendContext, fileId);
                BasicTypeSerializer.Put(SendContext, str, true);
                Daq.Execute();
                Daq.Receive();
                CheckResult();
                Int32 charactersWritten = 0;
                charactersWritten = BasicTypeDeSerializer.Get(ReceiveContext, charactersWritten);
                return charactersWritten;
            }
        }
        public UInt32 GetFilePosition(UInt16 fileId) {
            lock (Daq) {
                BasicTypeSerializer.Put(SendContext, (byte)NwazetDAQ.Command.FileSystem);
                BasicTypeSerializer.Put(SendContext, (byte)Command.GetFilePosition);
                BasicTypeSerializer.Put(SendContext, fileId);
                Daq.Execute();
                Daq.Receive();
                CheckResult();
                UInt32 position = 0;
                position = BasicTypeDeSerializer.Get(ReceiveContext, position);
                return position;
            }
        }
        public bool GetEndOfFile(UInt16 fileId) {
            lock (Daq) {
                BasicTypeSerializer.Put(SendContext, (byte)NwazetDAQ.Command.FileSystem);
                BasicTypeSerializer.Put(SendContext, (byte)Command.GetEndOfFile);
                BasicTypeSerializer.Put(SendContext, fileId);
                Daq.Execute();
                Daq.Receive();
                CheckResult();
                byte eof = BasicTypeDeSerializer.Get(ReceiveContext);
                return (eof == 1) ? true : false;
            }
        }
        public UInt32 GetFileSize(UInt16 fileId) {
            lock (Daq) {
                BasicTypeSerializer.Put(SendContext, (byte)NwazetDAQ.Command.FileSystem);
                BasicTypeSerializer.Put(SendContext, (byte)Command.GetFileSize);
                BasicTypeSerializer.Put(SendContext, fileId);
                Daq.Execute();
                Daq.Receive();
                CheckResult();
                UInt32 size = 0;
                size = BasicTypeDeSerializer.Get(ReceiveContext, size);
                return size;
            }
        }
        public bool GetFileError(UInt16 fileId) {
            lock (Daq) {
                BasicTypeSerializer.Put(SendContext, (byte)NwazetDAQ.Command.FileSystem);
                BasicTypeSerializer.Put(SendContext, (byte)Command.GetFileError);
                BasicTypeSerializer.Put(SendContext, fileId);
                Daq.Execute();
                Daq.Receive();
                CheckResult();
                byte error = BasicTypeDeSerializer.Get(ReceiveContext);
                return (error == 1) ? true : false;
            }
        }
        //
        // Directory functions
        //
        public UInt16 OpenDir(string directoryName) {
            lock (Daq) {
                BasicTypeSerializer.Put(SendContext, (byte)NwazetDAQ.Command.FileSystem);
                BasicTypeSerializer.Put(SendContext, (byte)Command.OpenDir);
                BasicTypeSerializer.Put(SendContext, directoryName, true);
                Daq.Execute();
                Daq.Receive();
                CheckResult();
                UInt16 dirId = 0;
                dirId = BasicTypeDeSerializer.Get(ReceiveContext, dirId);
                return dirId;
            }
        }
        public void CloseDir(UInt16 dirId) {
            lock (Daq) {
                BasicTypeSerializer.Put(SendContext, (byte)NwazetDAQ.Command.FileSystem);
                BasicTypeSerializer.Put(SendContext, (byte)Command.CloseDir);
                BasicTypeSerializer.Put(SendContext, dirId);
                Daq.Execute();
                Daq.Receive();
                CheckResult();
            }
        }
        public FileInfo ReadDir(UInt16 dirId) {
            lock (Daq) {
                BasicTypeSerializer.Put(SendContext, (byte)NwazetDAQ.Command.FileSystem);
                BasicTypeSerializer.Put(SendContext, (byte)Command.ReadDir);
                BasicTypeSerializer.Put(SendContext, dirId);
                Daq.Execute();
                Daq.Receive();
                CheckResult();
                var fileInfo = new FileInfo();
                fileInfo.Get(ReceiveContext);
                return fileInfo;
            }
        }
        public void MakeDir(string directoryName) {
            lock (Daq) {
                BasicTypeSerializer.Put(SendContext, (byte)NwazetDAQ.Command.FileSystem);
                BasicTypeSerializer.Put(SendContext, (byte)Command.MakeDir);
                BasicTypeSerializer.Put(SendContext, directoryName, true);
                Daq.Execute();
                Daq.Receive();
                CheckResult();
            }
        }
        public void ChangeDir(string path) {
            lock (Daq) {
                BasicTypeSerializer.Put(SendContext, (byte)NwazetDAQ.Command.FileSystem);
                BasicTypeSerializer.Put(SendContext, (byte)Command.ChangeDir);
                BasicTypeSerializer.Put(SendContext, path, true);
                Daq.Execute();
                Daq.Receive();
                CheckResult();
            }
        }
        public string GetCurrentDir() {
            lock (Daq) {
                BasicTypeSerializer.Put(SendContext, (byte)NwazetDAQ.Command.FileSystem);
                BasicTypeSerializer.Put(SendContext, (byte)Command.GetCurrentDir);
                Daq.Execute();
                Daq.Receive();
                CheckResult();
                string dir = "";
                dir = BasicTypeDeSerializer.Get(ReceiveContext, dir);
                return dir;
            }
        }
        //
        // Disk space functions
        //
        public DiskSpaceInfo GetFreeSpace() {
            lock (Daq) {
                BasicTypeSerializer.Put(SendContext, (byte)NwazetDAQ.Command.FileSystem);
                BasicTypeSerializer.Put(SendContext, (byte)Command.GetFreeSpace);
                Daq.Execute();
                Daq.Receive();
                CheckResult();
                var info = new DiskSpaceInfo();
                info.Get(ReceiveContext);
                return info;
            }
        }
        //
        // File and directory functions
        //
        public void Unlink(string objectName) {
            lock (Daq) {
                BasicTypeSerializer.Put(SendContext, (byte)NwazetDAQ.Command.FileSystem);
                BasicTypeSerializer.Put(SendContext, (byte)Command.Unlink);
                BasicTypeSerializer.Put(SendContext, objectName, true);
                Daq.Execute();
                Daq.Receive();
                CheckResult();
            }
        }
        public void ChangeAttribute(string objectName, byte attribute, byte attributeMask) {
            lock (Daq) {
                BasicTypeSerializer.Put(SendContext, (byte)NwazetDAQ.Command.FileSystem);
                BasicTypeSerializer.Put(SendContext, (byte)Command.ChangeAttribute);
                BasicTypeSerializer.Put(SendContext, objectName, true);
                BasicTypeSerializer.Put(SendContext, attribute);
                BasicTypeSerializer.Put(SendContext, attributeMask);
                Daq.Execute();
                Daq.Receive();
                CheckResult();
            }
        }
        public void ChangeTime(string objectName, DateTime dt) {
            lock (Daq) {
                BasicTypeSerializer.Put(SendContext, (byte)NwazetDAQ.Command.FileSystem);
                BasicTypeSerializer.Put(SendContext, (byte)Command.ChangeTime);
                BasicTypeSerializer.Put(SendContext, objectName, true);
                BasicTypeSerializer.Put(SendContext, (UInt16)dt.Year);
                BasicTypeSerializer.Put(SendContext, (UInt16)dt.Month);
                BasicTypeSerializer.Put(SendContext, (UInt16)dt.Day);
                BasicTypeSerializer.Put(SendContext, (UInt16)dt.Hour);
                BasicTypeSerializer.Put(SendContext, (UInt16)dt.Minute);
                BasicTypeSerializer.Put(SendContext, (UInt16)dt.Second);
                Daq.Execute();
                Daq.Receive();
                CheckResult();
            }
        }
        public void Rename(string OldObjectName, string NewObjectName) {
            lock (Daq) {
                BasicTypeSerializer.Put(SendContext, (byte)NwazetDAQ.Command.FileSystem);
                BasicTypeSerializer.Put(SendContext, (byte)Command.Rename);
                BasicTypeSerializer.Put(SendContext, OldObjectName, true);
                BasicTypeSerializer.Put(SendContext, NewObjectName, true);
                Daq.Execute();
                Daq.Receive();
                CheckResult();
            }
        }
        //
        // File system resource management functions
        //
        public void Dispose() {
            UnMount();
        }
    }
}

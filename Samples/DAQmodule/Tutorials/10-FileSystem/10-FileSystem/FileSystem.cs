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
using System;
using System.Threading;
using Microsoft.SPOT;
using Microsoft.SPOT.Hardware;
using SecretLabs.NETMF.Hardware.NetduinoGo;
using Nwazet.Go.DAQ;
namespace Tutorials {
    public class FS {
        public static NwazetDAQ daq = new NwazetDAQ();
        public static void Main() {
            try {
                daq.Initialize(GoSockets.Socket1);

                // Initialize the file system on the SD card
                daq.FatFs.Mount();

                var spaceInfo = daq.FatFs.GetFreeSpace();

                Debug.Print("Total disk space: " + spaceInfo.DiskCapacityKB + " KB");
                Debug.Print("Total free space: " + spaceInfo.FreeDiskSpaceKB + " KB");

                var maxLine = 0;
                
                // Create a file
                var fileId = daq.FatFs.OpenFile("file.txt", FileSystem.FileMode.CreateAlways | FileSystem.FileMode.Read | FileSystem.FileMode.Write);
                
                // Write some text to it...
                daq.FatFs.WriteString(fileId, "Line #" + ++maxLine + ": Hello World!\r\n");
                daq.FatFs.WriteString(fileId, "Line #" + ++maxLine + ": Violets are blue\r\n");
                daq.FatFs.WriteString(fileId, "Line #" + ++maxLine + ": Roses are red\r\n");
                daq.FatFs.WriteString(fileId, "Line #" + ++maxLine + ": This is only a test\r\n");

                // Close it
                daq.FatFs.CloseFile(fileId);

                //
                // Make a copy of the file...
                //
                var sourceFile = daq.FatFs.OpenFile("file.txt", FileSystem.FileMode.OpenExisting | FileSystem.FileMode.Read);
                var destFile = daq.FatFs.OpenFile("copy.txt", FileSystem.FileMode.CreateAlways | FileSystem.FileMode.Write);
                var buffer = new byte[1024];
                while (true) {
                    var bytesRead = daq.FatFs.ReadFile(sourceFile, buffer, buffer.Length);
                    var bytesWritten = daq.FatFs.WriteFile(destFile, buffer, bytesRead);
                    if (bytesRead != bytesWritten) {
                        throw new ApplicationException("should never happen unless storage is full or corrupted");
                    }
                    if (bytesRead < buffer.Length) {
                        break;
                    }
                }
                daq.FatFs.CloseFile(sourceFile);
                daq.FatFs.CloseFile(destFile);

                //
                // Basic File system enumeration
                //
                var dirId = daq.FatFs.OpenDir("/"); // open the root directory
                while (true) {
                    var fileInfo = daq.FatFs.ReadDir(dirId);
                    if (fileInfo.IsEndOfDirectory) break;
                    Debug.Print("Attributes: " + fileInfo.Attribute);
                    if (fileInfo.IsVolumeLabel) {
                        Debug.Print("Volume: " + fileInfo.Name);
                        continue;
                    }
                    if (fileInfo.IsDirectory) {
                        Debug.Print("Dir: " + fileInfo.Name);
                        continue;
                    }
                    Debug.Print("File: " + fileInfo.Name + ", bytes: " + fileInfo.Size);
                }
                daq.FatFs.CloseDir(dirId);

            } catch (FatFsException e) {
                Debug.Print(e.Message);
            }

            // Release all file system resources
            daq.FatFs.UnMount();
        }
    }
}

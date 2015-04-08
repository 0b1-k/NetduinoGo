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
using System.Text;
using System.Threading;
using Microsoft.SPOT;
using Microsoft.SPOT.Hardware;
using SecretLabs.NETMF.Hardware.NetduinoGo;
using Nwazet.Go.DAQ;
using Honeywell;
using Bosch;
using Taos;
namespace DAQmodule {
    public class Program {
        public static NwazetDAQ daq = new NwazetDAQ();

        public static void Main() {
            daq.Initialize(GoSockets.Socket1);
            
            Log("Initialize()");

            daq.Interrupts += new InterruptEventEventHandler(InterruptHandler);

            using (var daqConfig = new DaqConfig(daq)) {
                if (daqConfig.IsOptionEnabled(DaqSetting.DiagnosticsToSerial) == false) {
                    daqConfig.SetOption(DaqSetting.DiagnosticsToSerial, true);
                    daqConfig.Commit(daq);
                    daq.Reboot();
                }
            }

            try {
                MultiGpioWriteTest();
                DigitalInputInterruptTest();
                
                //Uncomment this section only if you have these I2C sensors connected to the DAQ module
                //I2cHIH613xTest();
                //I2cBmp085Test();
                //I2cTSL2561Test();

                UsartTest();
                RtcBackupRegistersTest();
                FileSystemTest();
                DigitalOutputPwmTest();
                DigitalInputTest();
                DigitalOutputTest();
                ClockTest();
                AdcTest();
                SetLedStateTest();
            }catch(Exception e){
                Log(">>>UNCAUGHT EXCEPTION: " + e.Message);
            }

            Log("Reboot()");
            daq.Reboot();
        }
        public static void MultiGpioWriteTest() {
            Log("MultiGpioWriteTest begin");

            var d0 = daq.Add(new GpioOutput(GpioId.D0, false));
            var d1 = (GpioOutputPwm)daq.Add(new GpioOutputPwm(GpioId.D1));
            var d2 = daq.Add(new GpioOutput(GpioId.D2, false));
            var d3 = (GpioOutputPwm)daq.Add(new GpioOutputPwm(GpioId.D3));
            var d4 = daq.Add(new GpioOutput(GpioId.D4_33v, false));
            var d5 = (GpioOutputPwm)daq.Add(new GpioOutputPwm(GpioId.D5));
            var d6 = daq.Add(new GpioOutput(GpioId.D6, false));
            var d7 = (GpioOutputPwm)daq.Add(new GpioOutputPwm(GpioId.D7));

            d0.State = true;
            d2.State = true;
            d4.State = true;
            d6.State = true;

            d1.DutyCycle = 0.22f;
            d3.DutyCycle = 0.22f;
            d5.DutyCycle = 0.22f;
            d7.DutyCycle = 0.22f;

            daq.Write();

            Thread.Sleep(2000);

            daq.Remove(d0);
            daq.Remove(d1);
            daq.Remove(d2);
            daq.Remove(d3);
            daq.Remove(d4);
            daq.Remove(d5);
            daq.Remove(d6);
            daq.Remove(d7);

            Log("MultiGpioWriteTest end");
        }
        public static void InterruptHandler(InterruptEvent intEvent) {
            Log("Interrupt on Gpio:" + intEvent.Id + ", Count: " + intEvent.Count + ", Rollover: " + intEvent.RollOverCount);
        }
        public static void DigitalInputInterruptTest() {
            Log("DigitalInputInterruptTest begin");

            var d0 = (GpioInputInterrupt)daq.Add(new GpioInputInterrupt(GpioId.D0));
            var d1 = (GpioInputInterrupt)daq.Add(new GpioInputInterrupt(GpioId.D1));
            var d2 = (GpioInputInterrupt)daq.Add(new GpioInputInterrupt(GpioId.D2));
            var d3 = (GpioInputInterrupt)daq.Add(new GpioInputInterrupt(GpioId.D3));
            var d5 = (GpioInputInterrupt)daq.Add(new GpioInputInterrupt(GpioId.D5));
            var d6 = (GpioInputInterrupt)daq.Add(new GpioInputInterrupt(GpioId.D6));
            var d7 = (GpioInputInterrupt)daq.Add(new GpioInputInterrupt(GpioId.D7));

            Log("Waiting for 15 seconds for inputs to be pulled down to fire events...");

            // Auto-monitoring of interrupt events
            daq.StartInterruptMonitor();
            Thread.Sleep(15000);
            daq.StopInterruptMonitor();

            // User-controlled interrupt event polling
            daq.PollInterruptCounters();

            daq.Remove(d0);
            daq.Remove(d1);
            daq.Remove(d2);
            daq.Remove(d3);
            daq.Remove(d5);
            daq.Remove(d6);
            daq.Remove(d7);

            Log("DigitalInputInterruptTest end");
        }
        public static void Log(string line) {
            daq.UsartPort.Write(Encoding.UTF8.GetBytes("DAQ: " + line + "\r\n"));
        }
        public static void I2cHIH613xTest() {
            Log("I2cHIH613xTest begin");
            try {
                var hih = new HIH613x(daq.I2cPort);
                var count = 3;
                while (count-- != 0) {
                    hih.Read();
                    Log("Temp: " + hih.TemperatureCelsius + " Celsius");
                    Log("Humidity: " + hih.RelativeHumidityPercent + " %");
                    Thread.Sleep(500);
                    Log("--------------------------------------------");
                }
            } catch (I2cException e) {
                Log("I2C transaction failed: " + e.Message);
            }
            Log("I2cHIH613xTest end");
        }
        public static void I2cBmp085Test() {
            Log("I2cBmp085Test begin");
            try {
                var bmp = new BoschBmp085(daq.I2cPort);
                var count = 3;
                while (count-- != 0) {
                    Log("Temp: " + bmp.ReadTemperatureCelsius() + " Celsius");
                    Log("Barometric Pressure: " + bmp.ReadPressurePascals() + " Pascals");
                    Log("Altitude: " + bmp.ReadAltitudeMeters() + " Meters");
                    Thread.Sleep(500);
                    Log("--------------------------------------------");
                }
            } catch (I2cException e) {
                Log("I2C transaction failed: " + e.Message);
            }
            Log("I2cBmp085Test end");
        }
        public static void I2cTSL2561Test() {
            Log("I2cTSL2561Test begin");
            try {
                var lightSensor = new TaosTSL256x(daq.I2cPort, TaosTSL256x.Address.AddressPinFloat);
                Log("Light sensor ID: " + lightSensor.GetSensorId());
                Log("--------------------------------------------");
                var count = 3;
                while (count-- != 0) {
                    Log("Raw Luminosity: " + lightSensor.Read());
                    Log("Full spectrum luminosity: " + lightSensor.FullSpectrum);
                    Log("Infrared spectrum luminosity: " + lightSensor.InfraredSpectrum);
                    Log("Visible spectrum luminosity: " + lightSensor.VisibleSpectrum);
                    Log("Lux: " + lightSensor.Lux);
                    Log("--------------------------------------------");
                    Thread.Sleep(500);
                }
            } catch (I2cException e) {
                Log("I2C transaction failed: " + e.Message);
            }
            Log("I2cTSL2561Test end");
        }
        public static void I2cDS1307RAMTest() {
            Log("I2cDS1307RAMTest begin");
            var buffer = new byte[57];
            // Fill the RAM with 0xA5
            for (var i = 0; i < buffer.Length; i++) {
                buffer[i] = 0xA5;
            }
            try {
                buffer[0] = 0x08; // device RAM address
                daq.I2cPort.Write(I2C.Speed.OneHundredkHz, 0x68, buffer); // write 56 bytes starting at address 0x08
                daq.I2cPort.Write(I2C.Speed.OneHundredkHz, 0x68, buffer, 0, 1); // reposition the address pointer
                // Erase the buffer
                for (var i = 0; i < buffer.Length; i++) {
                    buffer[i] = 0;
                }
                daq.I2cPort.Read(I2C.Speed.OneHundredkHz, 0x68, buffer, 0, 56); // read back 56 bytes
                // dump the data that was read from the RAM (should all be 0xA5 with the last byte being 0x00)
                for (var count = 0; count < buffer.Length; count++) {
                    Log("I2C: buffer[" + count + "] = " + buffer[count]);
                }
                Log("----------------------------------------------------------");

            } catch (I2cException e) {
                Log("I2C transaction failed: " + e.Message);
            }
            Log("I2cDS1307RAMTest end");
        }
        public static void UsartTest() {
            Log("UsartTest begin");
            
            var usartConfig = new UsartConfig();
            usartConfig.BaudRate = UsartBaudRate.Baud115200;
            daq.UsartPort.SetConfig(usartConfig);

            daq.UsartPort.Write("Enter text and press 'enter' or wait for the 5 sec. timeout: ");
            var text = daq.UsartPort.Read(totalTimeoutMs: 5000);
            if (text.Length != 0) {
                Log("User input: " + text);
            } else {
                Log("No user input");
            }
            Log("UsartTest end");
        }
        public static void RtcBackupRegistersTest() {
            Log("RtcBackupRegistersTest begin");

            var daqConfig = new DaqConfig(daq);

            Log("System config options:");
            Log("\tClockConfigured: " + daqConfig.IsOptionEnabled(DaqSetting.ClockConfigured));
            Log("\tClockTimeSet: " + daqConfig.IsOptionEnabled(DaqSetting.ClockTimeSet));
            Log("\tDiagnosticsToSerial: " + daqConfig.IsOptionEnabled(DaqSetting.DiagnosticsToSerial));
            Log("\tFirmataMode: " + daqConfig.IsOptionEnabled(DaqSetting.FirmataMode));

            Log("User register[1] = " + daqConfig.GetUserRegister(Register.User1));
            Log("User register[2] = " + daqConfig.GetUserRegister(Register.User2));
            Log("User register[3] = " + daqConfig.GetUserRegister(Register.User3));
            Log("User register[4] = " + daqConfig.GetUserRegister(Register.User4));

            daqConfig.SetUserRegister(Register.User1, 0xdeadbeef);
            daqConfig.SetUserRegister(Register.User2, 0x1337d00d);
            daqConfig.SetUserRegister(Register.User3, 0x0badf00d);
            daqConfig.SetUserRegister(Register.User4, 0xbabe1ab5);

            daqConfig.Commit(daq);

            Log("RtcBackupRegistersTest end");
        }
        public static void FileSystemTest() {
            Log("FileSystemTest begin");
            try {
                daq.FatFs.Mount();
                FileSystemCapacityTest(daq.FatFs);

                Log("SD status = " + daq.FatFs.GetDiskStatus());

                /* Optional tests: used to validate max file / dir handles
                // 1st pass: test opening 5 files and directories max.
                FileSystemCleanup(fatFs);
                FileSystemOpenAllFileAndDirectoryHandles(fatFs);
                
                // 2nd pass: test re-opening the same files
                FileSystemCleanup(fatFs);
                FileSystemOpenAllFileAndDirectoryHandles(fatFs);
                */

                FileSystemCleanup(daq.FatFs);
                FileSystemDirectoryTest(daq.FatFs);
                FileSystemStringIoTest(daq.FatFs, "StringIO.txt");
                FileSystemRenameTest(daq.FatFs);
                FileSystemCopyTest(daq.FatFs, "StringIO.txt", "dir0/sub0/sub1/Copy.txt");
                FileSystemAttributesTest(daq.FatFs, "StringIO.txt");
                FileSystemTimestampChangeTest(daq.FatFs, "StringIO.txt");
                FileSystemEnumerationTest(daq.FatFs);
            } catch (FatFsException e) {
                Log(e.Message);
            }
            daq.FatFs.UnMount();
            Log("FileSystemTest end");
        }
        public static void FileSystemCleanup(FileSystem fatFs) {
            Log("FileSystemCleanupBeforeTest begin");
            FileSystemUnLinkObject(fatFs, "StringIO.txt");
            FileSystemUnLinkObject(fatFs, "dir0/sub0/sub1/Copy.txt");
            FileSystemUnLinkObject(fatFs, "dir0/sub0/sub1");
            FileSystemUnLinkObject(fatFs, "dir0/sub0");
            FileSystemUnLinkObject(fatFs, "dir0");
            FileSystemUnLinkObject(fatFs, "dir1");
            FileSystemUnLinkObject(fatFs, "dir2");
            FileSystemUnLinkObject(fatFs, "dir3");
            FileSystemUnLinkObject(fatFs, "dir4");
            FileSystemUnLinkObject(fatFs, "dir5");
            FileSystemUnLinkObject(fatFs, "file0.txt");
            FileSystemUnLinkObject(fatFs, "file1.txt");
            FileSystemUnLinkObject(fatFs, "file2.txt");
            FileSystemUnLinkObject(fatFs, "file3.txt");
            FileSystemUnLinkObject(fatFs, "file4.txt");
            FileSystemUnLinkObject(fatFs, "file5.txt");
            Log("FileSystemCleanupBeforeTest end");
        }
        public static void FileSystemUnLinkObject(FileSystem fatFs, string objectName) {
            try {
                fatFs.Unlink(objectName);
            } catch (FatFsException e) {
                Log("Expected: Unlink failed for '" + objectName + "', msg: " + e.Message);
            }
        }
        public static void FileSystemOpenAllFileAndDirectoryHandles(FileSystem fatFs){
            Log("FileSystemOpenAllFileAndDirectoryHandles begin");

            fatFs.MakeDir("dir0");
            fatFs.MakeDir("dir1");
            fatFs.MakeDir("dir2");
            fatFs.MakeDir("dir3");
            fatFs.MakeDir("dir4");
            fatFs.MakeDir("dir5");

            var file0 = CreateAndOpenDummyFile(fatFs, "dummy0.txt");
            var file1 = CreateAndOpenDummyFile(fatFs, "dummy1.txt");
            var file2 = CreateAndOpenDummyFile(fatFs, "dummy2.txt");
            var file3 = CreateAndOpenDummyFile(fatFs, "dummy3.txt");
            var file4 = CreateAndOpenDummyFile(fatFs, "dummy4.txt");

            Log("5 files open simultaneously (max)");

            try {
                var file5 = CreateAndOpenDummyFile(fatFs, "dummy5.txt");
            } catch (FatFsException e) {
                Log("Expected. Failed to open new file: " + e.Message);
            }

            var dir0 = fatFs.OpenDir("dir0");
            var dir1 = fatFs.OpenDir("dir1");
            var dir2 = fatFs.OpenDir("dir2");
            var dir3 = fatFs.OpenDir("dir3");
            var dir4 = fatFs.OpenDir("dir4");

            Log("5 directories open simultaneously (max)");

            try {
                var dir5 = fatFs.OpenDir("dir5");
            } catch (FatFsException e) {
                Log("Expected. Failed to open a new directory: " + e.Message);
            }

            fatFs.CloseFile(file0);
            fatFs.CloseFile(file1);
            fatFs.CloseFile(file2);
            fatFs.CloseFile(file3);
            fatFs.CloseFile(file4);

            fatFs.CloseDir(dir0);
            fatFs.CloseDir(dir1);
            fatFs.CloseDir(dir2);
            fatFs.CloseDir(dir3);
            fatFs.CloseDir(dir4);

            Log("FileSystemOpenAllFileAndDirectoryHandles end");
        }
        public static UInt16 CreateAndOpenDummyFile(FileSystem fatFs, string filename) {
            return fatFs.OpenFile(filename, FileSystem.FileMode.CreateAlways);
        }
        public static void FileSystemEnumerationTest(FileSystem fatFs) {
            Log("FileSystemEnumerationTest begin");
            var dirId = fatFs.OpenDir("/"); // open the root directory
            while (true) {
                var fileInfo = fatFs.ReadDir(dirId);
                if (fileInfo.IsEndOfDirectory) break;
                Log("Attributes: " + fileInfo.Attribute);
                if (fileInfo.IsVolumeLabel) {
                    Log("Volume: " + fileInfo.Name);
                    continue;
                }
                if (fileInfo.IsDirectory) {
                    Log("Dir: " + fileInfo.Name);
                    continue;
                }
                Log("File: " + fileInfo.Name + ", bytes: " + fileInfo.Size);
            }
            fatFs.CloseDir(dirId);
            Log("FileSystemEnumerationTest end");
        }
        public static void FileSystemTimestampChangeTest(FileSystem fatFs, string filename) {
            Log("FileSystemTimestampChangeTest begin");
            fatFs.ChangeTime(filename, new DateTime(2012, 8, 17, 11, 18, 33));
            Log("FileSystemTimestampChangeTest end");
        }
        public static void FileSystemAttributesTest(FileSystem fatFs, string filename) {
            Log("FileSystemAttributesTest begin");
            // Make the file a hidden archive and clear the other attributes
            fatFs.ChangeAttribute(filename, FileAttribute.Archive | FileAttribute.Hidden, FileAttribute.Archive | FileAttribute.Hidden);
            Log("FileSystemAttributesTest end");
        }
        public static void FileSystemRenameTest(FileSystem fatFs) {
            fatFs.Rename("StringIO.txt", "foo.txt");
            fatFs.Rename("foo.txt", "StringIO.txt");
            fatFs.Rename("dir0", "bar0");
            fatFs.Rename("bar0", "dir0");
        }
        public static void FileSystemDirectoryTest(FileSystem fatFs) {
            Log("FileSystemDirectoryTest begin");
            fatFs.MakeDir("dir0");
            fatFs.MakeDir("dir0/sub0");
            fatFs.MakeDir("dir0/sub0/sub1");

            string rootDir = fatFs.GetCurrentDir();
            Log("Current directory (root): " + rootDir);

            Log("Changing current directory to /dir0/sub0/sub1");
            fatFs.ChangeDir("/dir0/sub0/sub1");

            string currentDir = fatFs.GetCurrentDir();
            Log("Current directory: " + currentDir);

            Log("Set directory back to root: " + rootDir);
            fatFs.ChangeDir(rootDir);

            Log("FileSystemDirectoryTest end");
        }
        public static void FileSystemCapacityTest(FileSystem fatFs) {
            Log("FileSystemCapacityTest begin");
            var spaceInfo = fatFs.GetFreeSpace();
            Log("Total disk space: " + spaceInfo.DiskCapacityKB + " KB");
            Log("Total free space: " + spaceInfo.FreeDiskSpaceKB + " KB");
            Log("FileSystemCapacityTest end");
        }
        public static void FileSystemStringIoTest(FileSystem fatFs, string filename) {
            Log("FileSystemStringIoTest begin");
            var maxLine = 0;
            var fileId = fatFs.OpenFile(filename, FileSystem.FileMode.CreateAlways | FileSystem.FileMode.Read | FileSystem.FileMode.Write);
            fatFs.WriteString(fileId, "Line #" + ++maxLine + ": Hello World!\r\n");
            fatFs.WriteString(fileId, "Line #" + ++maxLine + ": Violets are blue\r\n");
            fatFs.WriteString(fileId, "Line #" + ++maxLine + ": Roses are red\r\n");
            fatFs.WriteString(fileId, "Line #" + ++maxLine + ": This is only a test\r\n");
            
            // Record where we are before writing the last string so that we can remove it later on...
            var marker = fatFs.GetFilePosition(fileId);
            
            fatFs.WriteString(fileId, "Line #" + ++maxLine + ": End of the World!");
            fatFs.SyncFile(fileId);
            
            // Show some stats about the file
            Log("Lines written: " + maxLine);
            Log("File size: " + fatFs.GetFileSize(fileId));
            Log("File EOF (expect true): " + fatFs.GetEndOfFile(fileId));
            Log("File pointer (expect same as size): " + fatFs.GetFilePosition(fileId));
            Log("File error (expect false): " + fatFs.GetFileError(fileId));

            // Take out the last line
            Log("Removing last line");
            fatFs.SeekFile(fileId, marker);
            fatFs.TruncateFile(fileId);
            fatFs.SyncFile(fileId);

            // Rewind to the beginning of the file and print the lines..
            fatFs.SeekFile(fileId, 0);
            for (int line = 0; line < maxLine; line++) {
                var text = fatFs.ReadString(fileId);
                Log(text);
            }

            Log("File size: " + fatFs.GetFileSize(fileId));
            Log("File EOF (expect true): " + fatFs.GetEndOfFile(fileId));
            Log("File pointer (expect same as size): " + fatFs.GetFilePosition(fileId));
            Log("File error (expect false): " + fatFs.GetFileError(fileId));

            fatFs.CloseFile(fileId);
            Log("FileSystemStringIoTest end");
        }
        public static void FileSystemCopyTest(FileSystem fatFs, string source, string destination) {
            Log("FileSystemCopyTest begin");
            var sourceFile = fatFs.OpenFile(source, FileSystem.FileMode.OpenExisting | FileSystem.FileMode.Read);
            var destFile = fatFs.OpenFile(destination, FileSystem.FileMode.CreateAlways | FileSystem.FileMode.Write);
            var buffer = new byte[1024];
            while (true) {
                var bytesRead = fatFs.ReadFile(sourceFile, buffer, buffer.Length);
                var bytesWritten = fatFs.WriteFile(destFile, buffer, bytesRead);
                if (bytesRead != bytesWritten) {
                    throw new ApplicationException("should never happen unless storage is full or corrupted");
                }
                if (bytesRead < buffer.Length) {
                    break;
                }
            }
            fatFs.SyncFile(destFile);
            fatFs.CloseFile(sourceFile);
            fatFs.CloseFile(destFile);
            Log("FileSystemCopyTest end");
        }
        public static void DigitalOutputPwmTest() {
            Log("DigitalOutputPwmTest begin");

            // No need to do this unless actually needed...
            // Example: switch from the default 1kHz frequency to 500Hz
            //daq.SetGpioPwmTimeBase(GpioId.D0, 500);
            //daq.SetGpioPwmTimeBase(GpioId.D1, 500);
            //daq.SetGpioPwmTimeBase(GpioId.D2, 500);
            //daq.SetGpioPwmTimeBase(GpioId.D3, 500);
            //daq.SetGpioPwmTimeBase(GpioId.D4, 500);
            //daq.SetGpioPwmTimeBase(GpioId.D5, 500);
            //daq.SetGpioPwmTimeBase(GpioId.D6, 500);
            //daq.SetGpioPwmTimeBase(GpioId.D7, 500);

            var d0 = (GpioOutputPwm)daq.Add(new GpioOutputPwm(GpioId.D0));
            var d1 = (GpioOutputPwm)daq.Add(new GpioOutputPwm(GpioId.D1));
            var d2 = (GpioOutputPwm)daq.Add(new GpioOutputPwm(GpioId.D2));
            var d3 = (GpioOutputPwm)daq.Add(new GpioOutputPwm(GpioId.D3));
            var d4 = (GpioOutputPwm)daq.Add(new GpioOutputPwm(GpioId.D4_33v));
            var d5 = (GpioOutputPwm)daq.Add(new GpioOutputPwm(GpioId.D5));
            var d6 = (GpioOutputPwm)daq.Add(new GpioOutputPwm(GpioId.D6));
            var d7 = (GpioOutputPwm)daq.Add(new GpioOutputPwm(GpioId.D7));

            var count = 3;
            while (count-- != 0) {
                var dutyCycle = 0.0f;
                for (; dutyCycle <= 1.0f; dutyCycle += 0.1f) {
                    for (GpioId id = 0; id < (GpioId)NwazetDAQ.MaxGpios; id++) {
                        daq.Write((GpioOutputPwm)daq.GetGpio(id), dutyCycle);
                    }
                    Thread.Sleep(40);
                }
                for (; dutyCycle >= 0.1f; dutyCycle -= 0.1f) {
                    for (GpioId id = 0; id < (GpioId)NwazetDAQ.MaxGpios; id++) {
                        daq.Write((GpioOutputPwm)daq.GetGpio(id), dutyCycle);
                    }
                    Thread.Sleep(40);
                }
            }

            daq.Remove(d0);
            daq.Remove(d1);
            daq.Remove(d2);
            daq.Remove(d3);
            daq.Remove(d4);
            daq.Remove(d5);
            daq.Remove(d6);
            daq.Remove(d7);
            Log("DigitalOutputPwmTest end");
        }
        public static void DigitalInputTest() {
            Log("DigitalInputTest begin");
            var d0 = (GpioInput)daq.Add(new GpioInput(GpioId.D0));
            var d1 = (GpioInput)daq.Add(new GpioInput(GpioId.D1));
            var d2 = (GpioInput)daq.Add(new GpioInput(GpioId.D2));
            var d3 = (GpioInput)daq.Add(new GpioInput(GpioId.D3));
            var d4 = (GpioInput)daq.Add(new GpioInput(GpioId.D4_33v));
            var d5 = (GpioInput)daq.Add(new GpioInput(GpioId.D5));
            var d6 = (GpioInput)daq.Add(new GpioInput(GpioId.D6));
            var d7 = (GpioInput)daq.Add(new GpioInput(GpioId.D7));

            // Read all input states in one shot...
            daq.Read();

            for (var i = 0; i < NwazetDAQ.MaxGpios; i++) {
                var gpio = daq.GetGpio((GpioId)i);
                Log("Input state[D" + i + "] : " + gpio.State);
            }

            // or read just one input state...
            var state = daq.Read(d7);

            daq.Remove(d0);
            daq.Remove(d1);
            daq.Remove(d2);
            daq.Remove(d3);
            daq.Remove(d4);
            daq.Remove(d5);
            daq.Remove(d6);
            daq.Remove(d7);
            Log("DigitalInputTest end");
        }
        public static void DigitalOutputTest() {
            Log("DigitalOutputTest begin");
            var d0 = (GpioOutput)daq.Add(new GpioOutput(GpioId.D0, false));
            var d1 = (GpioOutput)daq.Add(new GpioOutput(GpioId.D1, false));
            var d2 = (GpioOutput)daq.Add(new GpioOutput(GpioId.D2, false));
            var d3 = (GpioOutput)daq.Add(new GpioOutput(GpioId.D3, false));
            var d4 = (GpioOutput)daq.Add(new GpioOutput(GpioId.D4_33v, false));
            var d5 = (GpioOutput)daq.Add(new GpioOutput(GpioId.D5, false));
            var d6 = (GpioOutput)daq.Add(new GpioOutput(GpioId.D6, false));
            var d7 = (GpioOutput)daq.Add(new GpioOutput(GpioId.D7, false));

            // Toggle the digital outputs one at a time with a 150 ms wait in between
            for (var i = 0; i < NwazetDAQ.MaxGpios; i++) {
                var gpio = (GpioOutput)daq.GetGpio((GpioId)i);
                daq.Write(gpio, true);
                Thread.Sleep(150);
                daq.Write(gpio, false);
                Thread.Sleep(150);
            }

            // Pulse D1 100 times w/o delay in between (observe pulses on o-scope / logic analyzer)
            daq.Write(d1, true);
            var count = 100;
            while (count-- != 0) {
                daq.Pulse(d1, false);
            }

            daq.Remove(d0);
            daq.Remove(d1);
            daq.Remove(d2);
            daq.Remove(d3);
            daq.Remove(d4);
            daq.Remove(d5);
            daq.Remove(d6);
            daq.Remove(d7);

            Log("DigitalOutputTest end");
        }
        public static void AdcTest() {
            Log("AdcTest begin");
            
            daq.SetLed(LedType.Analog, (uint)LedIndex.All, LedState.On);
            var sample = daq.ReadAnalogInputs();

            Log("Sample Time: " + sample.Time.ToString() + "." + sample.Time.Millisecond);
            Log("A0 = " + sample.Values[(int)ADC.A0] + " | " + sample.GetVoltage(ADC.A0).ToString() + "V");
            Log("A1 = " + sample.Values[(int)ADC.A1] + " | " + sample.GetVoltage(ADC.A1).ToString() + "V");
            Log("A2 = " + sample.Values[(int)ADC.A2] + " | " + sample.GetVoltage(ADC.A2).ToString() + "V");
            Log("A3 = " + sample.Values[(int)ADC.A3] + " | " + sample.GetVoltage(ADC.A3).ToString() + "V");
            Log("A4 = " + sample.Values[(int)ADC.A4] + " | " + sample.GetVoltage(ADC.A4).ToString() + "V");
            Log("A5 = " + sample.Values[(int)ADC.A5] + " | " + sample.GetVoltage(ADC.A5).ToString() + "V");
            Log("A6 = " + sample.Values[(int)ADC.A6] + " | " + sample.GetVoltage(ADC.A6).ToString() + "V");
            Log("A7 = " + sample.Values[(int)ADC.A7] + " | " + sample.GetVoltage(ADC.A7).ToString() + "V");
            Log("VRef = " + sample.Values[(int)ADC.VRef] + " | " + sample.GetVoltage(ADC.VRef).ToString() + "V");
            Log("VBat = " + sample.Values[(int)ADC.VBat] + " | " + sample.GetVoltage(ADC.VBat).ToString() + "V");

            daq.SetLed(LedType.Analog, (uint)LedIndex.All, LedState.Off);
            Log("AdcTest end");
        }
        public static void ClockTest() {
            Log("ClockTest begin");
            var state = daq.GetClockState();
            if (state == NwazetDAQ.ClockState.Invalid) {
                Log("setting a default time");
                daq.SetDateTime(new DateTime(2012, 05, 15, 12, 34, 56));
                ClockTest();
            } else {
                Log("RTC is initialized");
                for (var i = 0; i < 5; i++) {
                    var dt = daq.GetDateTime();
                    Log("Time: " + dt.ToString() + "." + dt.Millisecond);
                    Thread.Sleep(1000);
                }
            }
            Log("ClockTest end");
        }
        public static void SetLedStateTest() {
            Log("SetLedStateTest begin");
            daq.SetLed(LedType.I2C, 0, LedState.On);
            daq.SetLed(LedType.SD, 0, LedState.On);
            daq.SetLed(LedType.USART, 0, LedState.On);
            daq.SetLed(LedType.Analog, (uint)LedIndex.All, LedState.On);
            daq.SetLed(LedType.Digital, (uint)LedIndex.All, LedState.On);
            int count = 3;
            while (count != 0) {
                count--;
                for (int ledIndex = 0; ledIndex < 8; ledIndex++) {
                    daq.SetLed(LedType.Analog, (uint)ledIndex, LedState.On);
                    daq.SetLed(LedType.Digital, (uint)ledIndex, LedState.On);
                    Thread.Sleep(50);
                    daq.SetLed(LedType.Analog, (uint)ledIndex, LedState.Off);
                    daq.SetLed(LedType.Digital, (uint)ledIndex, LedState.Off);
                }
                for (int ledIndex = 7; ledIndex >= 0; ledIndex--) {
                    daq.SetLed(LedType.Analog, (uint)ledIndex, LedState.On);
                    daq.SetLed(LedType.Digital, (uint)ledIndex, LedState.On);
                    Thread.Sleep(50);
                    daq.SetLed(LedType.Analog, (uint)ledIndex, LedState.Off);
                    daq.SetLed(LedType.Digital, (uint)ledIndex, LedState.Off);
                }
            }
            daq.SetLed(LedType.I2C, 0, LedState.Off);
            daq.SetLed(LedType.SD, 0, LedState.Off);
            daq.SetLed(LedType.USART, 0, LedState.Off); 
            Log("SetLedStateTest end");
        }
    }
}

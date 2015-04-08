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
    public class StorageRegisters {
        public static NwazetDAQ daq = new NwazetDAQ();
        public static void Main() {
            daq.Initialize(GoSockets.Socket1);

            var daqConfig = new DaqConfig(daq);

            // DAQ system configuration register settings
            Debug.Print("System config options:");
            // Real Time Clock configured and enabled?
            Debug.Print("\tClockConfigured: " + daqConfig.IsOptionEnabled(DaqSetting.ClockConfigured));
            // Real Time Clock Date / Time set?
            Debug.Print("\tClockTimeSet: " + daqConfig.IsOptionEnabled(DaqSetting.ClockTimeSet));
            // Internal diagnostics output to USART enabled?
            Debug.Print("\tDiagnosticsToSerial: " + daqConfig.IsOptionEnabled(DaqSetting.DiagnosticsToSerial));

            // User configuration registers
            Debug.Print("User register[1] = " + daqConfig.GetUserRegister(Register.User1));
            Debug.Print("User register[2] = " + daqConfig.GetUserRegister(Register.User2));
            Debug.Print("User register[3] = " + daqConfig.GetUserRegister(Register.User3));
            Debug.Print("User register[4] = " + daqConfig.GetUserRegister(Register.User4));

            // Set arbitrary values into the user configuration registers
            daqConfig.SetUserRegister(Register.User1, 0xdeadbeef);
            daqConfig.SetUserRegister(Register.User2, 0x1337d00d);
            daqConfig.SetUserRegister(Register.User3, 0x0badf00d);
            daqConfig.SetUserRegister(Register.User4, 0xbabe1ab5);

            // Commit changes to all registers
            daqConfig.Commit(daq);
        }
    }
}

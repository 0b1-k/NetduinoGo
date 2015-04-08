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
    public class Boot {
        public static NwazetDAQ daq = new NwazetDAQ();
        public static void Main() {
            daq.Initialize(GoSockets.Socket1);

            daq.SetLed(LedType.I2C, 0, LedState.On);
            daq.SetLed(LedType.SD, 0, LedState.On);
            daq.SetLed(LedType.USART, 0, LedState.On);

            while (true) {
                Cylon();
            }
        }
        public static void Cylon() {
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
        }
    }
}

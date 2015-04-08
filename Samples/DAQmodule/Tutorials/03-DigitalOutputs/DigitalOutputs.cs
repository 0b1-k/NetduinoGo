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
    public class DigitalOutputs {
        public static NwazetDAQ daq = new NwazetDAQ();
        public static void Main() {
            daq.Initialize(GoSockets.Socket1);

            var d0 = (GpioOutput)daq.Add(new GpioOutput(GpioId.D0, false));
            var d1 = (GpioOutput)daq.Add(new GpioOutput(GpioId.D1, false));
            var d2 = (GpioOutput)daq.Add(new GpioOutput(GpioId.D2, false));
            var d3 = (GpioOutput)daq.Add(new GpioOutput(GpioId.D3, false));
            var d4 = (GpioOutput)daq.Add(new GpioOutput(GpioId.D4_33v, false));
            var d5 = (GpioOutput)daq.Add(new GpioOutput(GpioId.D5, false));
            var d6 = (GpioOutput)daq.Add(new GpioOutput(GpioId.D6, false));
            var d7 = (GpioOutput)daq.Add(new GpioOutput(GpioId.D7, false));

            // Changing the state of multiple digital outputs with one call
            d0.State = true;
            d2.State = true;
            d4.State = true;
            d6.State = true;

            daq.Write();

            Thread.Sleep(2000);

            // Addressing digital outputs by index, one at a time
            for (var i = 0; i < NwazetDAQ.MaxGpios; i++) {
                var gpio = (GpioOutput)daq.GetGpio((GpioId)i);
                daq.Write(gpio, true);
                Thread.Sleep(150);
                daq.Write(gpio, false);
                Thread.Sleep(150);
            }

            Thread.Sleep(2000);

            // Sending a pulse to a digital output (start High, followed by 100 'Low to High' transitions)
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
        }
    }
}

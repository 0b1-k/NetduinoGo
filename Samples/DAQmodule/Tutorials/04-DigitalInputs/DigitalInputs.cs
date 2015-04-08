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
    public class DigitalInputs {
        public static NwazetDAQ daq = new NwazetDAQ();
        public static void Main() {
            daq.Initialize(GoSockets.Socket1);

            var d0 = (GpioInput)daq.Add(new GpioInput(GpioId.D0));
            var d1 = (GpioInput)daq.Add(new GpioInput(GpioId.D1));
            var d2 = (GpioInput)daq.Add(new GpioInput(GpioId.D2));
            var d3 = (GpioInput)daq.Add(new GpioInput(GpioId.D3));
            var d4 = (GpioInput)daq.Add(new GpioInput(GpioId.D4_33v));
            var d5 = (GpioInput)daq.Add(new GpioInput(GpioId.D5));
            var d6 = (GpioInput)daq.Add(new GpioInput(GpioId.D6));
            var d7 = (GpioInput)daq.Add(new GpioInput(GpioId.D7));

            // Reading all input states with one call
            daq.Read();

            // Dump the state of the inputs
            for (var i = 0; i < NwazetDAQ.MaxGpios; i++) {
                var gpio = daq.GetGpio((GpioId)i);
                Debug.Print("Input state[D" + i + "] : " + gpio.State);
            }

            // Reading a single input at a time...
            var state = daq.Read(d7);

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

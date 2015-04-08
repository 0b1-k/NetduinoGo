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
    public class DigitalOutputsPWM {
        public static NwazetDAQ daq = new NwazetDAQ();
        public static void Main() {
            daq.Initialize(GoSockets.Socket1);

            var d0 = (GpioOutputPwm)daq.Add(new GpioOutputPwm(GpioId.D0));
            var d1 = (GpioOutputPwm)daq.Add(new GpioOutputPwm(GpioId.D1));
            var d2 = (GpioOutputPwm)daq.Add(new GpioOutputPwm(GpioId.D2));
            var d3 = (GpioOutputPwm)daq.Add(new GpioOutputPwm(GpioId.D3));
            var d4 = (GpioOutputPwm)daq.Add(new GpioOutputPwm(GpioId.D4_33v));
            var d5 = (GpioOutputPwm)daq.Add(new GpioOutputPwm(GpioId.D5));
            var d6 = (GpioOutputPwm)daq.Add(new GpioOutputPwm(GpioId.D6));
            var d7 = (GpioOutputPwm)daq.Add(new GpioOutputPwm(GpioId.D7));

            // Change the duty cyle of each PWM digital output one at a time
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

            Thread.Sleep(2000);

            // Change the duty cycle of multiple digital PWM outputs with one call
            d1.DutyCycle = 0.33f;
            d3.DutyCycle = 0.33f;
            d5.DutyCycle = 0.33f;
            d7.DutyCycle = 0.33f;

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
        }
    }
}

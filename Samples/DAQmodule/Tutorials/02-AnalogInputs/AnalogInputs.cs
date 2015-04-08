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
    public class AnalogInputs {
        public static NwazetDAQ daq = new NwazetDAQ();
        public static void Main() {
            daq.Initialize(GoSockets.Socket1);
            daq.SetLed(LedType.Analog, (uint)LedIndex.All, LedState.On);
            var sample = daq.ReadAnalogInputs();
            Debug.Print("Sample Time: " + sample.Time.ToString() + "." + sample.Time.Millisecond);
            Debug.Print("A0 = " + sample.Values[(int)ADC.A0] + " | " + sample.GetVoltage(ADC.A0).ToString() + "V");
            Debug.Print("A1 = " + sample.Values[(int)ADC.A1] + " | " + sample.GetVoltage(ADC.A1).ToString() + "V");
            Debug.Print("A2 = " + sample.Values[(int)ADC.A2] + " | " + sample.GetVoltage(ADC.A2).ToString() + "V");
            Debug.Print("A3 = " + sample.Values[(int)ADC.A3] + " | " + sample.GetVoltage(ADC.A3).ToString() + "V");
            Debug.Print("A4 = " + sample.Values[(int)ADC.A4] + " | " + sample.GetVoltage(ADC.A4).ToString() + "V");
            Debug.Print("A5 = " + sample.Values[(int)ADC.A5] + " | " + sample.GetVoltage(ADC.A5).ToString() + "V");
            Debug.Print("A6 = " + sample.Values[(int)ADC.A6] + " | " + sample.GetVoltage(ADC.A6).ToString() + "V");
            Debug.Print("A7 = " + sample.Values[(int)ADC.A7] + " | " + sample.GetVoltage(ADC.A7).ToString() + "V");
            Debug.Print("VRef = " + sample.Values[(int)ADC.VRef] + " | " + sample.GetVoltage(ADC.VRef).ToString() + "V");
            Debug.Print("VBat = " + sample.Values[(int)ADC.VBat] + " | " + sample.GetVoltage(ADC.VBat).ToString() + "V");
            daq.SetLed(LedType.Analog, (uint)LedIndex.All, LedState.Off);
        }
    }
}

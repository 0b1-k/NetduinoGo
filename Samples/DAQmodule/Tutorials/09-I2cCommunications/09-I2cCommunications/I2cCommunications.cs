﻿/*
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
using Honeywell;
namespace Tutorials {
    public class I2cCommunications {
        public static NwazetDAQ daq = new NwazetDAQ();
        public static void Main() {
            daq.Initialize(GoSockets.Socket1);
            try {
                var hih = new HIH613x(daq.I2cPort);
                hih.Read();
                Debug.Print("Temp: " + hih.TemperatureCelsius + " Celsius");
                Debug.Print("Humidity: " + hih.RelativeHumidityPercent + " %");

            } catch (I2cException e) {
                Debug.Print("I2C transaction failed: " + e.Message);
            }
        }
    }
}

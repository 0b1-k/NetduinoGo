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
using Microsoft.SPOT;
using Microsoft.SPOT.Hardware;
using GoBus;

namespace AdafruitMax6675 {
    public class Max6675 : GoModule {
        protected SPI Spi;
        public void Initialize(GoBus.GoSocket socket) {
            Cpu.Pin chipSelect;
            Cpu.Pin gpio;
            SPI.SPI_module spi;
            socket.GetPhysicalResources(out gpio, out spi, out chipSelect);
            Spi = new SPI(new SPI.Configuration(chipSelect, false, 1, 0, false, false, 1000, spi));
            BindSocket(socket, new Guid(new byte[] { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }));
        }
        public double Celsius {
            get {
                return RawSensorValue * 0.25;
            }
        }
        public double Farenheit {
            get {
                return ((Celsius * 9.0) / 5.0) + 32;
            }
        }
        protected UInt16 RawSensorValue;
        protected byte[] ReadBuffer = new byte[2];
        protected byte[] WriteBuffer = new byte[2];
        public void Read() {
            RawSensorValue = 0;
            Spi.WriteRead(WriteBuffer, ReadBuffer);
            RawSensorValue |= ReadBuffer[0];
            RawSensorValue <<= 8;
            RawSensorValue |= ReadBuffer[1];
            if ((RawSensorValue & 0x4) == 1) {
                throw new ApplicationException("No thermocouple attached.");
            }
            RawSensorValue >>= 3;
        }
        protected override void Dispose(bool disposing = true) {
            Spi.Dispose();
            base.Dispose(disposing);
        }
        ~Max6675() {
            Dispose();
        }
    }
}

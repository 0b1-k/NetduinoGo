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
using Nwazet.Go.DAQ;
namespace Bosch {
    // http://www.bosch-sensortec.com/content/language1/downloads/BST-BMP085-DS000-05.pdf
    public class BoschBmp085 : IDisposable {
        public const byte Address = 0x77;
        public enum Precision {
            UltraLow,
            Standard,
            High,
            UltraHigh
        }
        protected enum Calibration {
            AC1 = 0xAA,
            AC2 = 0xAC,
            AC3 = 0xAE,
            AC4 = 0xB0,
            AC5 = 0xB2,
            AC6 = 0xB4,
            B1 = 0xB6,
            B2 = 0xB8,
            MB = 0xBA,
            MC = 0xBC,
            MD = 0xBE
        }
        protected enum Register {
            Control = 0xF4,
            Result = 0xF6,
            ExtendedResult = 0xF8
        }
        protected enum Command {
            ReadRawTemperature = 0x2E,
            ReadRawPressure = 0x34
        }

        protected Int16 Ac1;
        protected Int16 Ac2;
        protected Int16 Ac3;
        protected Int16 B1;
        protected Int16 B2;
        protected Int16 Mb;
        protected Int16 Mc;
        protected Int16 Md;
        protected UInt16 Ac4;
        protected UInt16 Ac5;
        protected UInt16 Ac6;

        protected Precision PowerUsage = Precision.UltraHigh;

        protected I2C I2c;
        protected byte[] Buffer = new byte[2];

        public Precision Accuracy {
            get {
                return PowerUsage;
            }
            set {
                PowerUsage = value;
            }
        }
        public BoschBmp085(I2C i2c) {
            I2c = i2c;
            Ac1 = (Int16)Read16((Register)Calibration.AC1);
            Ac2 = (Int16)Read16((Register)Calibration.AC2);
            Ac3 = (Int16)Read16((Register)Calibration.AC3);
            Ac4 = Read16((Register)Calibration.AC4);
            Ac5 = Read16((Register)Calibration.AC5);
            Ac6 = Read16((Register)Calibration.AC6);
            B1 = (Int16)Read16((Register)Calibration.B1);
            B2 = (Int16)Read16((Register)Calibration.B2);
            Mb = (Int16)Read16((Register)Calibration.MB);
            Mc = (Int16)Read16((Register)Calibration.MC);
            Md = (Int16)Read16((Register)Calibration.MD);
        }
        ~BoschBmp085() {
            Dispose();
        }
        public void Dispose() {
            Buffer = null;
        }
        public float ReadTemperatureCelsius() {
            Int32 ut = ReadRawTemperature();
            Int32 x1 = ((ut - (Int32)Ac6) * (Int32)Ac5) >> 15;
            Int32 x2 = ((Int32)Mc << 11) / (x1 + (Int32)Md);
            Int32 b5 = x1 + x2;
            float temp = (b5 + 8) >> 4;
            return temp /= 10;
        }
        protected UInt16 ReadRawTemperature() {
            Write(Register.Control, (byte)Command.ReadRawTemperature);
            Thread.Sleep(5);
            var rawTemp = Read16(Register.Result);
            return rawTemp;
        }
        public Int32 ReadPressurePascals() {
            byte oversampling = (byte)PowerUsage;
            Int32 ut = ReadRawTemperature();
            Int32 up = (Int32)ReadRawPressure();
            Int32 x1 = ((ut - (Int32)Ac6) * (Int32)Ac5) >> 15;
            Int32 x2 = ((Int32)Mc << 11) - (x1 + (Int32)Md) / 2;
            x2 /= (x1 + Md);
            Int32 b5 = x1 + x2;
            Int32 b6 = b5 - 4000;
            x1 = ((Int32)B2 * ((b6 * b6) >> 12)) >> 11;
            x2 = ((Int32)Ac2 * b6) >> 11;
            Int32 x3 = x1 + x2;
            Int32 b3 = ((((Int32)Ac1 * 4 + x3) << oversampling) + 2) / 4;
            x1 = ((Int32)Ac3 * b6) >> 13;
            x2 = ((Int32)B1 * ((b6 * b6) >> 12)) >> 16;
            x3 = ((x1 + x2) + 2) >> 2;
            UInt32 b4 = ((UInt32)Ac4 * (UInt32)(x3 + 32768)) >> 15;
            UInt32 b7 = (UInt32)((UInt32)up - b3) * (UInt32)((UInt32)50000 >> oversampling);
            Int32 p = 0;
            if (b7 < 0x80000000) {
                p = (Int32)((b7 * 2) / b4);
            } else {
                p = (Int32)((b7 / b4) * 2);
            }
            x1 = (p >> 8) * (p >> 8);
            x1 = (x1 * 3038) >> 16;
            x2 = (-7357 * p) >> 16;
            p = p + ((x1 + x2 + (Int32)3791) >> 4);
            return p;
        }
        public const float SeaLevelPressurePascals = 101325.0f;
        public float ReadAltitudeMeters(float seaLevelPressure = SeaLevelPressurePascals) {
            float pressurePascals = ReadPressurePascals();
            float altitudeMeters = (float)(44330 * (1.0 - Math.Pow(pressurePascals / seaLevelPressure, 0.1903)));
            return altitudeMeters;
        }
        protected UInt32 ReadRawPressure() {
            var command = (byte)Command.ReadRawPressure;
            var oversampling = (byte)PowerUsage;
            oversampling <<= 6;
            command += oversampling;
            Write(Register.Control, command);
            switch (PowerUsage) {
                case Precision.UltraLow:
                    Thread.Sleep(5);
                    break;
                case Precision.Standard:
                    Thread.Sleep(8);
                    break;
                case Precision.High:
                    Thread.Sleep(14);
                    break;
                default: // Precision.UltraHigh:
                    Thread.Sleep(26);
                    break;
            }
            UInt32 rawPressure = Read16(Register.Result);
            rawPressure <<= 8;
            rawPressure |= Read8(Register.ExtendedResult);
            rawPressure >>= (8 - (byte)PowerUsage);
            return rawPressure;
        }
        protected void Write(Register register, byte data) {
            Buffer[0] = (byte)register;
            Buffer[1] = data;
            I2c.Write(I2C.Speed.FourHundredkHz, Address, Buffer);
        }
        protected byte Read8(Register register) {
            Buffer[0] = (byte)register;
            I2c.Write(I2C.Speed.FourHundredkHz, Address, Buffer, length: 1);
            I2c.Read(I2C.Speed.FourHundredkHz, Address, Buffer, length: 1);
            return Buffer[0];
        }
        protected UInt16 Read16(Register register) {
            UInt16 data = 0;
            Buffer[0] = (byte)register;
            I2c.Write(I2C.Speed.FourHundredkHz, Address, Buffer, length: 1);
            I2c.Read(I2C.Speed.FourHundredkHz, Address, Buffer);
            data = Buffer[0];
            data <<= 8;
            data |= Buffer[1];
            return data;
        }
    }
}

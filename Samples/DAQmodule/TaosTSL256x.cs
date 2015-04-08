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
namespace Taos {
    // http://www.taosinc.com/getfile.aspx?type=press&file=tsl2560-e58.pdf
    public class TaosTSL256x : IDisposable {
        public enum Address {
            AddressPinLow = 0x29,
            AddressPinFloat = 0x39,
            AddressPinHigh = 0x49
        }
        public enum IntegrationTimeMs {
            FastThirteenPointSeven = 0x00,      // 13.7ms
            MediumOneHundredOne = 0x01,         // 101ms
            SlowFourHundredTwo = 0x02           // 402ms
        }
        public enum Gain {
            NoGain,
            Gain16x = 0x10
        }
        protected enum Command {
            Block = 0x10,
            Word = 0x20,
            Clear = 0x40,
            Command = 0x80
        }
        protected enum Power {
            Off,
            On = 0x03
        }
        protected enum Register {
            Control = 0x00,
            Timing = 0x01,
            ThresholdLowLow = 0x02,
            ThresholdLowHigh = 0x03,
            ThresholdHighLow = 0x04,
            ThresholdHighHigh = 0x05,
            Interrupt = 0x06,
            Reserved0 = 0x07,
            Crc = 0x08,
            Reserved1 = 0x09,
            Id = 0x0A,
            Reserved2 = 0x0B,
            Channel0Low = 0x0C,
            Channel0High = 0x0D,
            Channel1Low = 0x0E,
            Channel1High = 0x0F
        }

        protected Address SensorAddress;
        protected IntegrationTimeMs IntegrationTime;
        protected Gain SensorGain;
        protected I2C I2c;
        protected byte[] Buffer = new byte[2];

        public TaosTSL256x(
            I2C i2c, 
            Address address = Address.AddressPinFloat, 
            IntegrationTimeMs integrationTimeMs = IntegrationTimeMs.FastThirteenPointSeven,
            Gain gain = Gain.Gain16x) {
            I2c = i2c;
            SensorAddress = address;

            SetTimingAndGain(integrationTimeMs, gain);
        }
        ~TaosTSL256x() {
            Dispose();
        }
        public void Dispose() {
            Buffer = null;
        }
        public byte GetSensorId() {
            Enable();
            byte id = 0;
            Write(Register.Id);
            id = Read8();
            Disable();
            return id;
        }
        public void SetTimingAndGain(IntegrationTimeMs timing, Gain gain) {
            Enable();
            Write((Register)((byte)Register.Timing | (byte)Command.Command), (byte)((byte)timing | (byte)gain));
            IntegrationTime = timing;
            SensorGain = gain;
            Disable();
        }
        protected UInt16 Channel0FullSpectrum;
        protected UInt16 Channel1Infrared;

        public UInt32 Read() {
            Enable();
            switch (IntegrationTime) {
                case IntegrationTimeMs.FastThirteenPointSeven:
                    Thread.Sleep(14);
                    break;
                case IntegrationTimeMs.MediumOneHundredOne:
                    Thread.Sleep(101);
                    break;
                case IntegrationTimeMs.SlowFourHundredTwo:
                    Thread.Sleep(402);
                    break;
            }
            Channel1Infrared = Read16((Register)((byte)Command.Command | (byte)Command.Word | (byte)Register.Channel1Low));
            Channel0FullSpectrum = Read16((Register)((byte)Command.Command | (byte)Command.Word | (byte)Register.Channel0Low));
            Disable();
            UInt32 luminosity = Channel1Infrared;
            luminosity <<= 16;
            luminosity |= Channel0FullSpectrum;
            return luminosity;
        }
        public UInt16 FullSpectrum {
            get {
                return Channel0FullSpectrum;
            }
        }
        public UInt16 InfraredSpectrum {
            get {
                return Channel1Infrared;
            }
        }
        public UInt16 VisibleSpectrum {
            get {
                return (UInt16)((int)FullSpectrum - (int)InfraredSpectrum);
            }
        }
        public UInt32 Lux {
            get {
                ulong channelScale = 0;
                ulong channel1Infrared = 0;
                ulong channel0FullSpectrum = 0;

                switch (IntegrationTime) {
                    case IntegrationTimeMs.FastThirteenPointSeven:
                        channelScale = LuxChannelScaleTint0;
                        break;
                    case IntegrationTimeMs.MediumOneHundredOne:
                        channelScale = LuxChannelScaleTint1;
                        break;
                    case IntegrationTimeMs.SlowFourHundredTwo:
                        channelScale = (LuxChannelScale << 1);
                        break;
                }
                if (SensorGain == 0) {
                    channelScale <<= 4;
                }
                channel0FullSpectrum = (Channel0FullSpectrum * channelScale) >> LuxChannelScale;
                channel1Infrared = (Channel1Infrared * channelScale) >> LuxChannelScale;

                ulong channelValuesRatio = 0;
                if (channel0FullSpectrum != 0) {
                    channelValuesRatio = (channel1Infrared << (LuxRatioScale + 1)) / channel0FullSpectrum;
                }
                ulong channelValuesRatioRounded = (channelValuesRatio + 1) >> 1;
                
                uint b = 0;
                uint m = 0;

                if ((channelValuesRatioRounded >= 0) && (channelValuesRatioRounded <= LuxK1T)) {
                    b = LuxB1T;
                    m = LuxM1T;
                } else if (channelValuesRatioRounded <= LuxK2T) {
                    b = LuxB2T;
                    m = LuxM2T;
                } else if (channelValuesRatioRounded <= LuxK3T) {
                    b = LuxB3T;
                    m = LuxM3T;
                } else if (channelValuesRatioRounded <= LuxK4T) {
                    b = LuxB4T;
                    m = LuxM4T;
                } else if (channelValuesRatioRounded <= LuxK5T) {
                    b = LuxB5T;
                    m = LuxM5T;
                } else if (channelValuesRatioRounded <= LuxK6T) {
                    b = LuxB6T;
                    m = LuxM6T;
                } else if (channelValuesRatioRounded <= LuxK7T) {
                    b = LuxB7T;
                    m = LuxM7T;
                } else if (channelValuesRatioRounded > LuxK8T) {
                    b = LuxB8T;
                    m = LuxM8T; 
                }

                ulong tempLux = ((channel0FullSpectrum * b) - (channel1Infrared * m));
                if (tempLux < 0) {
                    tempLux = 0;
                }
                tempLux += (1 << (LuxScale - 1));
                return (UInt32)(tempLux >> LuxScale);
            }
        }
        protected void Enable() {
            Write((Register)((byte)Register.Control | (byte)Command.Command), (byte)Power.On);
        }
        protected void Disable() {
            Write((Register)((byte)Register.Control | (byte)Command.Command), (byte)Power.Off);
        }
        protected void Write(Register register) {
            Buffer[0] = (byte)register;
            I2c.Write(I2C.Speed.OneHundredkHz, (UInt16)SensorAddress, Buffer, length: 1);
        }
        protected void Write(Register register, byte data) {
            Buffer[0] = (byte)register;
            Buffer[1] = data;
            I2c.Write(I2C.Speed.OneHundredkHz, (UInt16)SensorAddress, Buffer);
        }
        protected byte Read8() {
            I2c.Read(I2C.Speed.OneHundredkHz, (UInt16)SensorAddress, Buffer, length: 1);
            return Buffer[0];
        }
        protected UInt16 Read16(Register register) {
            UInt16 data = 0;
            Buffer[0] = (byte)register;
            I2c.Write(I2C.Speed.OneHundredkHz, (UInt16)SensorAddress, Buffer, length: 1);
            I2c.Read(I2C.Speed.OneHundredkHz, (UInt16)SensorAddress, Buffer);
            data = Buffer[1];
            data <<= 8;
            data |= Buffer[0];
            return data;
        }

        protected const UInt16 LuxScale = 14;
        protected const UInt16 LuxRatioScale = 9;
        protected const UInt16 LuxChannelScale = 10;
        protected const UInt16 LuxChannelScaleTint0 = 0x7517;
        protected const UInt16 LuxChannelScaleTint1 = 0x0FE7;

        // T, FN and CL package values
        protected const UInt16 LuxK1T = 0x0040;
        protected const UInt16 LuxB1T = 0x01f2;
        protected const UInt16 LuxM1T = 0x01be;
        protected const UInt16 LuxK2T = 0x0080;
        protected const UInt16 LuxB2T = 0x0214;
        protected const UInt16 LuxM2T = 0x02d1;
        protected const UInt16 LuxK3T = 0x00c0;
        protected const UInt16 LuxB3T = 0x023f;
        protected const UInt16 LuxM3T = 0x037b;
        protected const UInt16 LuxK4T = 0x0100;
        protected const UInt16 LuxB4T = 0x0270;
        protected const UInt16 LuxM4T = 0x03fe;
        protected const UInt16 LuxK5T = 0x0138;
        protected const UInt16 LuxB5T = 0x016f;
        protected const UInt16 LuxM5T = 0x01fc;
        protected const UInt16 LuxK6T = 0x019a;
        protected const UInt16 LuxB6T = 0x00d2;
        protected const UInt16 LuxM6T = 0x00fb;
        protected const UInt16 LuxK7T = 0x029a;
        protected const UInt16 LuxB7T = 0x0018;
        protected const UInt16 LuxM7T = 0x0012;
        protected const UInt16 LuxK8T = 0x029a;
        protected const UInt16 LuxB8T = 0x0000;
        protected const UInt16 LuxM8T = 0x0000;
    }
}

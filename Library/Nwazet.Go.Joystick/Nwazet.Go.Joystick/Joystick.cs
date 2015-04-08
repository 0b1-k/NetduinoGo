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
using System.IO;
using System.Threading;
using Microsoft.SPOT.Hardware;
using SecretLabs.NETMF.Hardware.NetduinoGo;
using Nwazet.Go.Helpers;
using GoBus;

namespace Nwazet.Go.Joystick {
    public class Joystick : GoModule {
        public void Initialize(GoBus.GoSocket socket, NativeEventHandler irqHandler, uint speedKHz = 2500) {
            if (speedKHz < 500 || speedKHz > 2500) throw new ArgumentOutOfRangeException("speedKHz");
            if (irqHandler == null) throw new ArgumentNullException("irqHandler");
            Cpu.Pin chipSelect;
            Cpu.Pin gpio;
            SPI.SPI_module spiModule;
            socket.GetPhysicalResources(out gpio, out spiModule, out chipSelect);

            SetSocketPowerState(false);
            Thread.Sleep(2000);
            SetSocketPowerState(true);
            Thread.Sleep(250);

            Spi = new SPI(new SPI.Configuration(
                SPI_mod: spiModule,
                ChipSelect_Port: chipSelect,
                ChipSelect_ActiveState: false,
                ChipSelect_SetupTime: 0,
                ChipSelect_HoldTime: 0,
                Clock_IdleState: false,
                Clock_Edge: false,
                Clock_RateKHz: speedKHz));

            BindSocket(socket);

            while (true) {
                Spi.WriteRead(SpiTxBuffer, SpiRxBuffer);
                if (SpiRxBuffer[1] == '[' &&
                    SpiRxBuffer[2] == 'n' &&
                    SpiRxBuffer[3] == 'w' &&
                    SpiRxBuffer[4] == 'a' &&
                    SpiRxBuffer[5] == 'z' &&
                    SpiRxBuffer[6] == 'e' &&
                    SpiRxBuffer[7] == 't' &&
                    SpiRxBuffer[8] == '.' &&
                    SpiRxBuffer[9] == 'j' &&
                    SpiRxBuffer[10] == 's' &&
                    SpiRxBuffer[11] == 't' &&
                    SpiRxBuffer[12] == 'k') {
                        break;
                }
                Thread.Sleep(100);
            }
            Irq = new InterruptPort(gpio, false, Port.ResistorMode.PullUp, Port.InterruptMode.InterruptEdgeLow);
            Irq.OnInterrupt += new NativeEventHandler(irqHandler);
        }
        public JoystickData Get() {
            SpiRxBuffer[0] = 0;
            SpiRxBuffer[1] = 0; 
            while (SpiRxBuffer[0] != 'O' && SpiRxBuffer[1] != 'K') {
                Spi.WriteRead(SpiTxBuffer, SpiRxBuffer);
                Thread.Sleep(10);
            }
            ReceiveContext.Bind(SpiRxBuffer, BasicTypeDeSerializerContext.BufferStartOffsetDefault);
            JoystickInput.Get(ReceiveContext);
            return JoystickInput;
        }
        protected override void Dispose(bool disposing = true) {
            Irq.DisableInterrupt();
            Irq.Dispose();
            Spi.Dispose();
            ReceiveContext.Dispose();
            base.Dispose(disposing);
            SetSocketPowerState(false);
        }
        ~Joystick() {
            Dispose();
        }
        protected SPI Spi;
        protected JoystickData JoystickInput = new JoystickData();
        protected BasicTypeDeSerializerContext ReceiveContext = new BasicTypeDeSerializerContext();
        private const int _maxSpiTxBufferSize = 18;
        private const int _maxSpiRxBufferSize = 18;
        protected byte[] SpiTxBuffer = new byte[_maxSpiTxBufferSize] { 0xFE, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF };
        public byte[] SpiRxBuffer = new byte[_maxSpiRxBufferSize];
        public InterruptPort Irq;
    }
}

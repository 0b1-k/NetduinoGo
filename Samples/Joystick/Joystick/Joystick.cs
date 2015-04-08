using System;
using System.IO;
using System.Threading;
using Microsoft.SPOT.Hardware;
using SecretLabs.NETMF.Hardware.NetduinoGo;
using Nwazet.Go.Helpers;
using GoBus;

namespace Nwazet.Go.Joystick {
    public class Joystick : GoModule {
        public InterruptPort Irq;
        public Joystick() {
            ReceiveContext = new BasicTypeDeSerializerContext();
        }
        public void Initialize(GoBus.GoSocket socket, NativeEventHandler irqHandler, uint speedKHz = 2500) {
            if (speedKHz < 500 || speedKHz > 10000) throw new ArgumentOutOfRangeException("speedKHz");
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
                Spi.WriteRead(spiTxBuffer, spiRxBuffer);
                if (spiRxBuffer[1] == '[' &&
                    spiRxBuffer[2] == 'n' &&
                    spiRxBuffer[3] == 'w' &&
                    spiRxBuffer[4] == 'a' &&
                    spiRxBuffer[5] == 'z' &&
                    spiRxBuffer[6] == 'e' &&
                    spiRxBuffer[7] == 't' &&
                    spiRxBuffer[8] == '.' &&
                    spiRxBuffer[9] == 'j' &&
                    spiRxBuffer[10] == 's' &&
                    spiRxBuffer[11] == 't' &&
                    spiRxBuffer[12] == 'k') {
                        break;
                }
                Thread.Sleep(100);
            }
            Irq = new InterruptPort(gpio, false, Port.ResistorMode.PullUp, Port.InterruptMode.InterruptEdgeLow);
            Irq.OnInterrupt += new NativeEventHandler(irqHandler);
        }
        public JoystickData Get() {
            spiRxBuffer[0] = 0;
            spiRxBuffer[1] = 0; 
            while (spiRxBuffer[0] != 'O' && spiRxBuffer[1] != 'K') {
                Spi.WriteRead(spiTxBuffer, spiRxBuffer);
                Thread.Sleep(10);
            }
            ReceiveContext.Bind(spiRxBuffer, BasicTypeDeSerializerContext.BufferStartOffsetDefault);
            var joystickData = new JoystickData();
            joystickData.Get(ReceiveContext);
            return joystickData;
        }
        protected override void Dispose(bool disposing = true) {
            Irq.DisableInterrupt();
            Spi.Dispose();
            ReceiveContext.Dispose();
            SetSocketPowerState(false);
            base.Dispose(disposing);
        }
        ~Joystick() {
            Dispose();
        }        
        protected SPI Spi;
        protected BasicTypeDeSerializerContext ReceiveContext;
        private const int _maxSpiTxBufferSize = 18;
        private const int _maxSpiRxBufferSize = 18;
        protected byte[] spiTxBuffer = new byte[_maxSpiTxBufferSize] { 0xFE, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF };
        protected byte[] spiRxBuffer = new byte[_maxSpiRxBufferSize];
    }
}

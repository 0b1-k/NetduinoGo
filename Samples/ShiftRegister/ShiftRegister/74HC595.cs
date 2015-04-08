using System;
using System.Threading;
using Microsoft.SPOT.Hardware;
namespace ShiftRegister {
    public class ShiftRegister74HC595 : IDisposable {
        protected SPI Spi;
        protected OutputPort ShiftRegisterClearPin;
        protected OutputPort OutputEnablePin;
        /// <summary>
        /// SPI clock (13) -> 74HC595 SCLK
        /// SPI data (11) -> 74HC595 SER
        /// latch pin (any) -> 74HC595 RCLK
        /// clearPin -> 74HC595 RCLR
        /// </summary>
        public ShiftRegister74HC595(Cpu.Pin shiftRegisterLatchPin, Cpu.Pin shiftRegisterClearPin, Cpu.Pin shiftRegisterOutputEnablePin) : this(shiftRegisterLatchPin, SPI.SPI_module.SPI1) {
            ShiftRegisterClearPin = new OutputPort(shiftRegisterClearPin, false);
            OutputEnablePin = new OutputPort(shiftRegisterOutputEnablePin, true);
        }
        public void OutputEnable(bool enable) {
            if (ShiftRegisterClearPin != null) {
                if (enable == false) {
                    ShiftRegisterClearPin.Write(false);
                    OutputEnablePin.Write(false);
                } else {
                    ShiftRegisterClearPin.Write(true);
                    OutputEnablePin.Write(false);
                }
            }
        }
        public void Clear() {
            if (ShiftRegisterClearPin != null) {
                ShiftRegisterClearPin.Write(true); 
                ShiftRegisterClearPin.Write(false);
                ShiftRegisterClearPin.Write(true);
            }
        }
        public ShiftRegister74HC595(Cpu.Pin latchPin, SPI.SPI_module spiModule, uint speedKHz = 10000) {
            var spiConfig = new SPI.Configuration(
                SPI_mod: spiModule,
                ChipSelect_Port: latchPin,
                ChipSelect_ActiveState: false,
                ChipSelect_SetupTime: 0,
                ChipSelect_HoldTime: 0,
                Clock_IdleState: false,
                Clock_Edge: true,
                Clock_RateKHz: speedKHz
                );
            Spi = new SPI(spiConfig);
        }
        public void Write(byte[] buffer) {
            var readBuffer = new byte[1];
            for(var i = 0; i<buffer.Length;i++) {
                Spi.WriteRead(buffer, i, 1, readBuffer, 0, 1, 0);
            }
        }
        public void Dispose() {
            Clear();
            OutputEnable(true);
            Spi.Dispose();
            ShiftRegisterClearPin.Dispose();
            OutputEnablePin.Dispose();
        }
        ~ShiftRegister74HC595() {
            Dispose();
        }
    }
}
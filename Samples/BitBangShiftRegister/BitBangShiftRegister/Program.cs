using System;
using System.Threading;
using Microsoft.SPOT;
using Microsoft.SPOT.Hardware;
using SecretLabs.NETMF.Hardware;
using SecretLabs.NETMF.Hardware.NetduinoPlus;
namespace ShiftRegister {
    public class Program {
        public static OutputPort SR_OUTPUT_ENABLE = new OutputPort(Pins.GPIO_PIN_D7, true);
        public static OutputPort SR_LATCH = new OutputPort(Pins.GPIO_PIN_D6, true);
        public static OutputPort SR_CLEAR = new OutputPort(Pins.GPIO_PIN_D5, true);
        public static OutputPort SR_DATA_IN = new OutputPort(Pins.GPIO_PIN_D11, false);
        public static OutputPort SR_CLOCK = new OutputPort(Pins.GPIO_PIN_D13, false);
        public static byte[] buffer = new byte[2];
        public enum GpioType {
            Analog,
            Digital
        }
        public static void Main() {
            buffer[0] = 0xff;
            buffer[1] = 0xff;
            while (true) {
                ShowGpioState(GpioType.Analog, 3, true);
                ShowGpioState(GpioType.Digital, 3, true);
                Thread.Sleep(100);
                ShowGpioState(GpioType.Analog, 3, false);
                ShowGpioState(GpioType.Digital, 3, false);
                Thread.Sleep(100);
            }
        }
        public static void ShowGpioState(GpioType type, int gpio, bool state) {
            byte gpioState = 1;
            gpioState <<= gpio;
            if (state) {
                buffer[(int)type] |= gpioState;
            } else {
                buffer[(int)type] &= (byte)~gpioState;
            }
            SR_OUTPUT_ENABLE.Write(true);
            ShiftOut(buffer[0]);
            PulsePin(SR_LATCH);
            ShiftOut(buffer[1]);
            PulsePin(SR_LATCH);
            SR_OUTPUT_ENABLE.Write(false);
        }
        public static void ShiftOut(byte data) {
            for(var bit = 0; bit < 8; bit++){
                byte bitValue = (byte)(data & (byte)0x80);
                SR_DATA_IN.Write((bitValue != 0) ? true : false);
                PulsePin(SR_CLOCK);
                data <<= 1;
            }
        }
        public static void PulsePin(OutputPort pin) {
            pin.Write(true); 
            pin.Write(false);
        }
    }
}

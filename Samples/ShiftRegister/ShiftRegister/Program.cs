using System;
using System.Threading;
using Microsoft.SPOT;
using Microsoft.SPOT.Hardware;
using SecretLabs.NETMF.Hardware;
using SecretLabs.NETMF.Hardware.NetduinoPlus;

namespace ShiftRegister {
    public class Program {
        public static void Main() {
            var enable = Pins.GPIO_PIN_D7;
            var latch = Pins.GPIO_PIN_D6;
            var clear = Pins.GPIO_PIN_D5;
            var reg = new ShiftRegister74HC595(latch, clear, enable);
            var buffer = new byte[] { 0, 0 };
            reg.OutputEnable(true);
            while (true) {
                for (var right = 0; right <= 255; right++) {
                    buffer[0] = (byte)right;
                    reg.Write(buffer);
                    Thread.Sleep(5);
                }
                for (var left = 0; left <= 255; left++) {
                    buffer[1] = (byte)left;
                    reg.Write(buffer);
                    Thread.Sleep(5);
                }
                buffer[0] = 0;
                buffer[1] = 0;
            }
        }
    }
}

using System;
using System.Threading;
using Microsoft.SPOT;
using Microsoft.SPOT.Hardware;
using Nwazet.Go.Helpers;
using SecretLabs.NETMF.Hardware.NetduinoGo;
using GoBus;

namespace Nwazet.Go.Joystick {
    public class Program {
        public static Joystick joystick = new Joystick();
        public static void Main() {
            joystick.Initialize(GoSockets.Socket6, GoBusIRQHandler);
            while (true) {
                var joystickData = joystick.Get();
                Debug.Print("X: " + joystickData.X + ", Y: " + joystickData.Y);
            }
        }
        public static void GoBusIRQHandler(uint data1, uint data2, DateTime time) {
            joystick.Irq.ClearInterrupt();
            Debug.Print("Click!");
        }
    }
}

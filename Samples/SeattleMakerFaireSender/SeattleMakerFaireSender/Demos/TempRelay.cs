using System;
using System.Threading;
using Microsoft.SPOT;
using Nwazet.Go.Fonts;
using Nwazet.Go.Relay;
using Nwazet.Go.Imaging;
using Nwazet.Go.Display.TouchScreen;
using AdafruitMax6675;
using SerialLib;
using SecretLabs.NETMF.Hardware.NetduinoGo;

namespace SeattleMakerFaireSender.Demos {
    public class TempRelay {
        public GoBus.GoSocket ThermoCoupleSocket = GoSockets.Socket7;
        public GoBus.GoSocket RelaySocket = GoSockets.Socket8;
        public Max6675 ThermoCouple = new Max6675();
        public Relay Relay = new Relay();
        public int LastTouchX;
        public int LastTouchY;
        public int LastTouchIsValid;
        public bool RelayStatus = false;
        public ushort BackgroundColor = ColorHelpers.GetRGB24toRGB565(255, 212, 42);

        public void Render(VirtualCanvas canvas) {
            Relay.Initialize(RelaySocket);
            ThermoCouple.Initialize(ThermoCoupleSocket);

            canvas.SetOrientation(Orientation.Landscape);
            canvas.DrawFill(BackgroundColor);
            canvas.DrawString(55, 4, 0, VerdanaBold14.ID, "Temperature & Relay");
            canvas.DrawString(32, 74, 0, VerdanaBold14.ID, "Celsius:");
            canvas.DrawString(32, 116, 0, VerdanaBold14.ID, "Fahrenheit:");
            canvas.DrawString(32, 154, 0, VerdanaBold14.ID, "Relay:");
            canvas.DrawButton(
                32, 197,
                250, 36,
                VerdanaBold14.ID, 20,
                ColorHelpers.GetRGB24toRGB565(0, 0, 0),
                ColorHelpers.GetRGB24toRGB565(255, 255, 255),
                ColorHelpers.GetRGB24toRGB565(0, 0, 0),
                "Click To Exit",
                RoundedCornerStyle.All);
            canvas.Execute();
            canvas.Touch += TouchEventHandler;
            while (LastTouchIsValid == 0) {
                ReadTemp();
                Thread.Sleep(100);
                canvas.DrawRectangleFilled(93, 153, 158, 176, BackgroundColor);
                canvas.DrawRectangleFilled( 105, 69, 175, 94, BackgroundColor);
                canvas.DrawRectangleFilled( 138, 112, 205, 137, BackgroundColor);
                canvas.DrawString(32, 74, 0, VerdanaBold14.ID, "Celsius: " + Shorten(ThermoCouple.Celsius.ToString()));
                canvas.DrawString(32, 116, 0, VerdanaBold14.ID, "Fahrenheit: "+ Shorten(ThermoCouple.Farenheit.ToString()));
                canvas.DrawString(32, 154, 0, VerdanaBold14.ID, "Relay: " + ((RelayStatus) ? "ON" : "OFF"));
                canvas.Execute();
                canvas.TouchscreenWaitForEvent(TouchScreenEventMode.NonBlocking);
            }
            canvas.Touch -= TouchEventHandler;
            Relay.Dispose();
            ThermoCouple.Dispose();
        }
        public void ReadTemp() {
            ThermoCouple.Read();
            if (ThermoCouple.Celsius >= 28) {
                RelayStatus = true;
            } else {
                RelayStatus = false;
            }
            Relay.Activate(RelayStatus);
        }
        protected string Shorten(string str) {
            if (str.Length > 5) {
                return str.Substring(0, 5);
            }
            return str;
        }
        protected void TouchEventHandler(VirtualCanvas canvas, TouchEvent touchEvent) {
            LastTouchX = touchEvent.X;
            LastTouchY = touchEvent.Y;
            LastTouchIsValid = touchEvent.IsValid;
        }
    }
}

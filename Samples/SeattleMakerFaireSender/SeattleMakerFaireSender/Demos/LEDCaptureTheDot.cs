using System;
using System.Threading;
using Nwazet.Go.Fonts;
using Nwazet.Go.Imaging;
using Nwazet.Go.Helpers;
using Nwazet.Go.Joystick;
using Nwazet.Go.Display.TouchScreen;
using SerialLib;
using RGBLedDisplay;
using Microsoft.SPOT;
using GoBus;
using SecretLabs.NETMF.Hardware.NetduinoGo;

namespace SeattleMakerFaireSender.Demos {
    public class LEDCaptureTheDot {
        public int LastTouchX;
        public int LastTouchY;
        public int LastTouchIsValid;

        public void Render(VirtualCanvas canvas, Joystick Joystick) {
            canvas.SetOrientation(Orientation.Landscape);
            canvas.DrawFill(ColorHelpers.GetRGB24toRGB565(255, 255, 255));
            canvas.DrawString(80, 4, ColorHelpers.GetRGB24toRGB565(0, 0, 0), VerdanaBold14.ID, "Capture The Dot");
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

            var SendContext = new BasicTypeSerializerContext();
            var ComPort = new SerialMessenger();

            ComPort.Messenger += OnMessage;
            ComPort.Initialize(bufferSize: 1024);

            int contentSize = 0;
            BasicTypeSerializer.Put(SendContext, (UInt16)RGBLedDisplayCommand.RunPong);
            var buffer = SendContext.GetBuffer(out contentSize);
            ComPort.Send(buffer, 0, (byte)contentSize);

            UInt16 LeftX = 0;
            UInt16 RightY = 0;
            UInt16 LastLeftX = 1;
            UInt16 LastRightY = 1;

            canvas.Touch += TouchEventHandler;
            while (LastTouchIsValid == 0) {
                var joystickData = Joystick.Get();
                //Debug.Print("RawX: " + joystickData.X + ", RawY: " + joystickData.Y);

                LeftX = (UInt16)MapRange(50, 950, 0, 15, joystickData.X);
                RightY = (UInt16)MapRange(50, 950, 0, 9, joystickData.Y);
                //Debug.Print("LeftX: " + LeftX + ", RightY: " + RightY);

                if (LeftX != LastLeftX || RightY != LastRightY) {
                    BasicTypeSerializer.Put(SendContext, (UInt16)RGBLedDisplayCommand.UpdateJoystick);
                    BasicTypeSerializer.Put(SendContext, (UInt16)LeftX);
                    BasicTypeSerializer.Put(SendContext, (UInt16)RightY);
                    buffer = SendContext.GetBuffer(out contentSize);
                    ComPort.Send(buffer, 0, (byte)contentSize);
                    Thread.Sleep(10);
                }
                canvas.TouchscreenWaitForEvent(TouchScreenEventMode.NonBlocking);
            }
            canvas.Touch -= TouchEventHandler;
            ComPort.Messenger -= OnMessage;

            BasicTypeSerializer.Put(SendContext, (UInt16)RGBLedDisplayCommand.Reset);
            buffer = SendContext.GetBuffer(out contentSize);
            ComPort.Send(buffer, 0, (byte)contentSize);

            ComPort.Dispose();
        }
        public float MapRange(float a1, float a2, float b1, float b2, float s) {
            return b1 + (s - a1) * (b2 - b1) / (a2 - a1);
        }
        public void OnMessage(SerialMessenger sender, byte[] message, int offset, int length) {
        }
        protected void TouchEventHandler(VirtualCanvas canvas, TouchEvent touchEvent) {
            LastTouchX = touchEvent.X;
            LastTouchY = touchEvent.Y;
            LastTouchIsValid = touchEvent.IsValid;
        }
    }
}

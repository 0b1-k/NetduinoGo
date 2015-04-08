using System;
using System.Threading;
using Nwazet.Go.Fonts;
using Nwazet.Go.Imaging;
using Nwazet.Go.Helpers;
using Nwazet.Go.Display.TouchScreen;
using SerialLib;
using RGBLedDisplay;
using Microsoft.SPOT;
using NetduinoGo;
using GoBus;
using SecretLabs.NETMF.Hardware.NetduinoGo;

namespace SeattleMakerFaireSender.Demos {
    public class RainbowPixel {
        public int LastTouchX;
        public int LastTouchY;
        public int LastTouchIsValid;
        public ushort LastColor = 255;
        public ManualResetEvent MessageReceivedEvent = new ManualResetEvent(false);
        public void Render(VirtualCanvas canvas) {
            var Pot = new NetduinoGo.Potentiometer(GoSockets.Socket1);
            var SendContext = new BasicTypeSerializerContext();
            var ComPort = new SerialMessenger();
            
            ComPort.Messenger += OnMessage;
            ComPort.Initialize();
            
            int contentSize = 0;
            BasicTypeSerializer.Put(SendContext, (UInt16)RGBLedDisplayCommand.Reset);
            var buffer = SendContext.GetBuffer(out contentSize);
            MessageReceivedEvent.Reset(); 
            ComPort.Send(buffer, 0, (byte)contentSize);
            MessageReceivedEvent.WaitOne(100,true);

            DrawPlayground(canvas);
            canvas.Touch += TouchEventHandler;
            var exit = false;
            while (!exit) {
                LastTouchIsValid = 0;
                canvas.TouchscreenWaitForEvent(TouchScreenEventMode.NonBlocking);
                ushort color = Wheel((ushort) MapRange(0, 1, 0, 384, Pot.GetValue()));
                if (color != LastColor) {
                    LastColor = color;
                    canvas.DrawRectangleFilled(184, 153, 213, 179, LastColor);
                    canvas.Execute();
                }
                if (LastTouchIsValid != 0) {
                    if (LastTouchX >= 80 && LastTouchY >= 42 && LastTouchX < 240 && LastTouchY <= 140) {
                        var x = (LastTouchX - 80) / 10;
                        var y = (LastTouchY - 42) / 10;
                        canvas.DrawRectangleFilled(80 + (x*10) + 1, 42 + (y*10) + 1, 80 + (x*10) + 1 + 8, 42 + (y*10) + 1 + 8, LastColor);
                        canvas.Execute();
                        BasicTypeSerializer.Put(SendContext, (UInt16)RGBLedDisplayCommand.SetPixel);
                        BasicTypeSerializer.Put(SendContext, (int)x);
                        BasicTypeSerializer.Put(SendContext, (int)y);
                        BasicTypeSerializer.Put(SendContext, (uint)LastColor);
                        buffer = SendContext.GetBuffer(out contentSize);

                        MessageReceivedEvent.Reset();
                        ComPort.Send(buffer, 0, (byte)contentSize);
                        MessageReceivedEvent.WaitOne(100, true);
                    } else if (LastTouchX >= 32 && LastTouchY >= 197 && LastTouchX <= 32 + 250 && LastTouchY <= 197 + 36) {
                        exit = true;
                    }
                }
            }
            canvas.Touch -= TouchEventHandler;
            ComPort.Dispose();
            Pot.Dispose();
        }
        public void OnMessage(SerialMessenger sender, byte[] message, int offset, int length) {
            MessageReceivedEvent.Set();
        }
        protected double MapRange(double a1, double a2, double b1, double b2, double s) {
            return b1 + (s - a1) * (b2 - b1) / (a2 - a1);
        }
        protected ushort Wheel(ushort wheelPosition) {
            byte r = 0, g = 0, b = 0;
            switch (wheelPosition / 128) {
                case 0:
                    r = (byte)(127 - (wheelPosition % 128));   //Red down
                    g = (byte)(wheelPosition % 128);      // Green up
                    b = 0;                  //blue off
                    break;
                case 1:
                    g = (byte)(127 - wheelPosition % 128);  //green down
                    b = (byte)(wheelPosition % 128);      //blue up
                    r = 0;                  //red off
                    break;
                case 2:
                    b = (byte)(127 - (wheelPosition % 128));  //blue down 
                    r = (byte)(wheelPosition % 128);      //red up
                    g = 0;                  //green off
                    break;
            }
            return ColorHelpers.GetRGB24toRGB565(r, g, b);
        }
        protected void TouchEventHandler(VirtualCanvas canvas, TouchEvent touchEvent) {
            LastTouchX = touchEvent.X;
            LastTouchY = touchEvent.Y;
            LastTouchIsValid = touchEvent.IsValid;
        }
        protected void DrawPlayground(VirtualCanvas canvas) {
            canvas.SetOrientation(Orientation.Landscape);
            canvas.DrawFill(ColorHelpers.GetRGB24toRGB565(255, 255, 255));
            canvas.DrawString(
                50, 4,
                ColorHelpers.GetRGB24toRGB565(0, 0, 0),
                VerdanaBold14.ID,
                "Rainbow Pixel Drawing");
            for (var i = 0; i < 110; i += 10) {
                canvas.DrawLine(
                    80, 42 + i, 240, 42 + i,
                    ColorHelpers.GetRGB24toRGB565(0, 0, 0));
            }
            for (var i = 0; i < 170; i += 10) {
                canvas.DrawLine(
                    80 + i, 42, 80 + i, 142,
                    ColorHelpers.GetRGB24toRGB565(0, 0, 0));
            }
            canvas.DrawString(
                110, 161,
                ColorHelpers.GetRGB24toRGB565(0, 0, 0),
                DejaVuSansBold9.ID,
                "Pixel Color");
            canvas.DrawRectangle(
                182, 151,
                215, 181,
                ColorHelpers.GetRGB24toRGB565(0, 0, 0));
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
        }
    }
}

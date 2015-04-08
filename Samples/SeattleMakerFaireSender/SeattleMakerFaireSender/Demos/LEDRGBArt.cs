using System;
using System.Threading;
using Nwazet.Go.Fonts;
using Nwazet.Go.Imaging;
using Nwazet.Go.Helpers;
using Nwazet.Go.Display.TouchScreen;
using SerialLib;
using RGBLedDisplay;
using Microsoft.SPOT;
using GoBus;
using SecretLabs.NETMF.Hardware.NetduinoGo;

namespace SeattleMakerFaireSender.Demos {
    public class LEDRGBArt {
        public int LastTouchX;
        public int LastTouchY;
        public int LastTouchIsValid;
        public int LastSquareX;
        public int LastSquareY;
        public ushort LastColor = ColorHelpers.GetRGB24toRGB565(255, 0, 0);
        public ManualResetEvent MessageReceivedEvent = new ManualResetEvent(false);
        public void Render(VirtualCanvas canvas) {
            var SendContext = new BasicTypeSerializerContext();
            var ComPort = new SerialMessenger();
            
            ComPort.Messenger += OnMessage;
            ComPort.Initialize(bufferSize: 1024);
            
            int contentSize = 0;

            BasicTypeSerializer.Put(SendContext, (UInt16)RGBLedDisplayCommand.Reset);
            var buffer = SendContext.GetBuffer(out contentSize);
            ComPort.Send(buffer, 0, (byte)contentSize);

            DrawPlayground(canvas);
            canvas.Touch += TouchEventHandler;
            var exit = false;
            ushort r=127,g=0,b=0;
            while (!exit) {
                LastTouchIsValid = 0;
                canvas.TouchscreenWaitForEvent(TouchScreenEventMode.NonBlocking);
                if (LastTouchIsValid != 0) {
                    if (LastTouchX >= 260 && LastTouchY >= 42 && LastTouchX <= 290 && LastTouchY <= 72) { // red
                        r = 127;
                        g = 0;
                        b = 0;
                        LastColor = ColorHelpers.GetRGB24toRGB565(255, 0, 0);
                    }
                    else if (LastTouchX >= 260 && LastTouchY >= 77 && LastTouchX <= 290 && LastTouchY <= 107) { // green
                        r = 0;
                        g = 127;
                        b = 0;
                        LastColor = ColorHelpers.GetRGB24toRGB565(0, 255, 0);
                    }
                    else if (LastTouchX >= 260 && LastTouchY >= 112 && LastTouchX <= 290 && LastTouchY <= 142) { // blue
                        r = 0;
                        g = 0;
                        b = 127;
                        LastColor = ColorHelpers.GetRGB24toRGB565(0, 0, 255);
                    }
                    else if (LastTouchX >= 80 && LastTouchY >= 150 && LastTouchX <= 80 + 160 && LastTouchY <= 170) { // reset
                        canvas.DrawRectangleFilled(
                            79, 40,
                            241, 141,
                            ColorHelpers.GetRGB24toRGB565(255, 255, 255)); 
                        DrawGrid(canvas);
                        BasicTypeSerializer.Put(SendContext, (UInt16)RGBLedDisplayCommand.Reset);
                        buffer = SendContext.GetBuffer(out contentSize);
                        ComPort.Send(buffer, 0, (byte)contentSize);
                        Thread.Sleep(250);
                    } 
                    else if (LastTouchX >= 80 && LastTouchY >= 42 && LastTouchX < 240 && LastTouchY < 140) { // canvas
                        var x = (LastTouchX - 80) / 10;
                        var y = (LastTouchY - 42) / 10;
                        if (LastSquareX != x || LastSquareY != y) {
                            LastSquareX = x;
                            LastSquareY = y;
                            canvas.DrawRectangleFilled(80 + (x * 10) + 1, 42 + (y * 10) + 1, 80 + (x * 10) + 1 + 8, 42 + (y * 10) + 1 + 8, LastColor);
                            canvas.Execute();
                            BasicTypeSerializer.Put(SendContext, (UInt16)RGBLedDisplayCommand.SetPixel);
                            BasicTypeSerializer.Put(SendContext, (ushort)x);
                            BasicTypeSerializer.Put(SendContext, (ushort)y);
                            BasicTypeSerializer.Put(SendContext, r);
                            BasicTypeSerializer.Put(SendContext, g);
                            BasicTypeSerializer.Put(SendContext, b);
                            buffer = SendContext.GetBuffer(out contentSize);
                            ComPort.Send(buffer, 0, (byte)contentSize);
                        }
                    }
                    else if (LastTouchX >= 32 && LastTouchY >= 197 && LastTouchX <= 32 + 250 && LastTouchY <= 197 + 36) { // exit
                        exit = true;
                    }
                }
            }
            canvas.Touch -= TouchEventHandler;
            ComPort.Dispose();
        }
        public void OnMessage(SerialMessenger sender, byte[] message, int offset, int length) {
            MessageReceivedEvent.Set();
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
                "RGB Pixel Drawing");
            DrawGrid(canvas);
            // red
            canvas.DrawRectangleFilled(
                260, 42,
                290, 72,
                ColorHelpers.GetRGB24toRGB565(255, 0, 0));
            // green
            canvas.DrawRectangleFilled(
                260, 77,
                290, 107,
                ColorHelpers.GetRGB24toRGB565(0, 250, 0));
            // blue
            canvas.DrawRectangleFilled(
                260, 112,
                290, 142,
                ColorHelpers.GetRGB24toRGB565(0, 0, 255));
            // Reset
            canvas.DrawButton(
                80, 150,
                160, 20,
                Verdana9.ID, 12,
                ColorHelpers.GetRGB24toRGB565(0, 0, 0),
                ColorHelpers.GetRGB24toRGB565(255, 255, 255),
                ColorHelpers.GetRGB24toRGB565(0, 0, 0),
                "Reset Drawing",
                RoundedCornerStyle.All);
            // Exit
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
        public void DrawGrid(VirtualCanvas canvas) {
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
        }
    }
}

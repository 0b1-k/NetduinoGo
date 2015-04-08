using System;
using System.Text;
using Microsoft.SPOT;
using Nwazet.Go.Fonts;
using Nwazet.Go.Imaging;
using Nwazet.Go.Joystick;
using Nwazet.Go.Helpers;
using SerialLib;
using SecretLabs.NETMF.Hardware.NetduinoGo;
using RGBLedDisplay;

namespace SeattleMakerFaireSender.Demos {
    class LEDPong {
        public Joystick Joystick = new Joystick();
        public BasicTypeSerializerContext SendContext = new BasicTypeSerializerContext();
        public SerialMessenger ComPort = new SerialMessenger();
        public UTF8Encoding Encoder = new UTF8Encoding();

        public GoBus.GoSocket JoystickSocket = GoSockets.Socket3;

        public void Render(VirtualCanvas canvas, SerialMessenger M) {
            ComPort.Initialize();
            Joystick.Initialize(JoystickSocket, JoystickIRQHandler);
            canvas.SetOrientation(Orientation.Landscape);
            canvas.DrawFill((ushort)BasicColor.White);
            while (true) {
                var input = Joystick.Get();
                SendJoystickInput(input);
                DisplayJoystickInput(canvas, input.X, input.Y);
            }
        }
        public void SendJoystickInput(JoystickData input) {
            int contentSize = 0;
            BasicTypeSerializer.Put(SendContext, (UInt16)RGBLedDisplayCommand.UpdateJoystick);
            BasicTypeSerializer.Put(SendContext, (ushort)input.X);
            BasicTypeSerializer.Put(SendContext, (ushort)input.Y);
            var buffer = SendContext.GetBuffer(out contentSize);
            ComPort.Send(buffer, 0, (byte)contentSize);
        }
        public void DisplayJoystickInput(VirtualCanvas canvas, ushort x, ushort y) {
            string text = "X: " + x / 100 + ", " + " Y: " + y / 100;
            canvas.DrawRectangleFilled(0, 110, canvas.Width, 110 + 20, (ushort)BasicColor.White);
            canvas.DrawString(20, 110, (ushort)BasicColor.Black, VerdanaBold14.ID, text);
            canvas.Execute();
        }
        public void JoystickIRQHandler(uint data1, uint data2, DateTime time) {
            Joystick.Irq.ClearInterrupt();
            Debug.Print("Click!");
        }
        public void OnMessage(SerialMessenger sender, byte[] message, int length) {
            var str = new string(Encoder.GetChars(message, 0, length));
            Debug.Print(str);
        }
    }
}

using System;
using System.Threading;
using Nwazet.Go.Imaging;
using Nwazet.Go.Helpers;
using SerialLib;
using RGBLedDisplay;
using Microsoft.SPOT;

namespace SeattleMakerFaireSender.Demos {
    public class LEDMarquee {
        public void Render(VirtualCanvas canvas) {
            var SendContext = new BasicTypeSerializerContext();
            var ComPort = new SerialMessenger();
            ComPort.Initialize();
            canvas.SetOrientation(Orientation.Landscape);
            var text = canvas.TouchscreenShowDialog(DialogType.Alphanumeric);
            Debug.Print("User text: " + text);
            int contentSize = 0;
            BasicTypeSerializer.Put(SendContext, (UInt16)RGBLedDisplayCommand.DisplayMarquee);
            BasicTypeSerializer.Put(SendContext, text, true);
            var buffer = SendContext.GetBuffer(out contentSize);
            ComPort.Send(buffer, 0, (byte)contentSize);
            Thread.Sleep(1000);
            ComPort.Dispose();
        }
    }
}

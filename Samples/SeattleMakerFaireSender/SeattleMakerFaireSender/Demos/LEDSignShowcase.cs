using System;
using System.Threading;
using Nwazet.Go.Imaging;
using Nwazet.Go.Helpers;
using SerialLib;
using RGBLedDisplay;
using Microsoft.SPOT;

namespace SeattleMakerFaireSender.Demos {
    public class LEDSignShowcase {
        public void Render() {
            var SendContext = new BasicTypeSerializerContext();
            var ComPort = new SerialMessenger();
            ComPort.Initialize();
            int contentSize = 0;
            BasicTypeSerializer.Put(SendContext, (UInt16)RGBLedDisplayCommand.RunDemo);
            var buffer = SendContext.GetBuffer(out contentSize);
            ComPort.Send(buffer, 0, (byte)contentSize);
            Thread.Sleep(1000);
            ComPort.Dispose();
        }
    }
}

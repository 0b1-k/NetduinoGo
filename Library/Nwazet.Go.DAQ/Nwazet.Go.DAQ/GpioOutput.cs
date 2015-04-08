using System;
using Nwazet.Go.Helpers;
namespace Nwazet.Go.DAQ {
    public class GpioOutput : Gpio {
        public GpioOutput(GpioId Id, bool state = false) {
            Identifier = Id;
            Mode = GpioMode.Output;
            Type = GpioType.PushPull;
            ResistorType = GpioResistorType.None;
            AlternateFunction = GpioAlternateFunction.None;
            State = state;
        }
        public void SerializeState(BasicTypeSerializerContext context) {
            BasicTypeSerializer.Put(context, (byte)Identifier);
            BasicTypeSerializer.Put(context, (byte)((State == true) ? (byte)1 : (byte)0));
        }
    }
}

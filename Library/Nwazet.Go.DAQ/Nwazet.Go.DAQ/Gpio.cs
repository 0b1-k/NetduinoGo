using System;
using Microsoft.SPOT;
using Nwazet.Go.Helpers;
namespace Nwazet.Go.DAQ {
    public class Gpio {
        public GpioId Identifier { get; set; }
        public GpioMode Mode { get; set; }
        public GpioType Type { get; set; }
        public GpioResistorType ResistorType { get; set; }
        public GpioAlternateFunction AlternateFunction { get; set; }
        public virtual bool State { get; set; }

        public virtual void SerializeDefinition(BasicTypeSerializerContext context) {
            BasicTypeSerializer.Put(context, (byte)Identifier);
            BasicTypeSerializer.Put(context, (byte)Mode);
            BasicTypeSerializer.Put(context, (byte)Type);
            BasicTypeSerializer.Put(context, (byte)ResistorType);
            BasicTypeSerializer.Put(context, (byte)AlternateFunction);
            BasicTypeSerializer.Put(context, (byte)((State == true) ? (byte)1 : (byte)0));
        }
        public virtual void Reset(){
            if (Mode == GpioMode.Output) {
                State = false;
            }
            Mode = GpioMode.Output;
            Type = GpioType.PushPull;
            ResistorType = GpioResistorType.None;
            AlternateFunction = GpioAlternateFunction.None;
        }
    }
}

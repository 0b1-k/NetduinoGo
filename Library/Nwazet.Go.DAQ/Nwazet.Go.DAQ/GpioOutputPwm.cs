using System;
using Nwazet.Go.Helpers;
namespace Nwazet.Go.DAQ {
    public class GpioOutputPwm : Gpio {
        public GpioOutputPwm(GpioId Id) {
            Identifier = Id;
            Mode = GpioMode.Output;
            Type = GpioType.PushPull;
            ResistorType = GpioResistorType.PullUp;
            AlternateFunction = GpioAlternateFunction.OutputPwmMode;
            dutyCycle = 0.0f;
        }
        private float dutyCycle;
        public float DutyCycle {
            get {
                return dutyCycle;
            }
            set {
                if(value<0.0f && value > 1.0f) throw new ArgumentOutOfRangeException("value must be between 0 and 1");
                dutyCycle = value;
            }
        }
        public void SerializeDutyCycle(BasicTypeSerializerContext context, GpioTimeBase timer) {
            UInt32 pulseLength = (UInt32)(dutyCycle * (float)(timer.TimerPeriod - 1));
            BasicTypeSerializer.Put(context, (byte)Identifier);
            BasicTypeSerializer.Put(context, pulseLength);
        }
    }
}

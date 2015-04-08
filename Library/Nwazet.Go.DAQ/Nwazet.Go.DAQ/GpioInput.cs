using System;
namespace Nwazet.Go.DAQ {
    public class GpioInput : Gpio {
        public GpioInput(GpioId Id) {
            Identifier = Id;
            Mode = GpioMode.Input;
            Type = GpioType.PushPull;
            ResistorType = GpioResistorType.PullUp;
            AlternateFunction = GpioAlternateFunction.None;
            State = false;
        }
    }
}

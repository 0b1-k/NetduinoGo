using System;
namespace Nwazet.Go.DAQ {
    public delegate void InterruptEventEventHandler(InterruptEvent interruptEvent);
    public class InterruptEvent {
        public GpioId Id;
        public UInt32 Count;
        public UInt32 RollOverCount;
    }
}

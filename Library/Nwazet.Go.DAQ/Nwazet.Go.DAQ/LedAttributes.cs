using System;
namespace Nwazet.Go.DAQ {
    public enum LedType {
        Analog,
        Digital,
        I2C,
        SD,
        USART
    }
    public enum LedState {
        Off,
        On
    }
    public enum LedIndex {
        All = 0xff
    }
}

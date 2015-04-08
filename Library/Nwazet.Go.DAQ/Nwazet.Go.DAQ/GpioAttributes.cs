using System;
namespace Nwazet.Go.DAQ {
    public enum GpioId {
        D0,
        D1,
        D2,
        D3,
        D4_33v,
        D5,
        D6,
        D7,
        GpioCount
    }
    public enum GpioTimerId {
        D0Timer = 17,
        D1Timer = 16,
        D2Timer = 2,
        D3Timer = 2,
        D4Timer = 14,
        D5Timer = 2,
        D6Timer = 3,
        D7Timer = 3
    }
    public enum GpioMode {
        Input = 0x00,
        Output = 0x01
    }
    public enum GpioType {
        PushPull = 0x00,
        OpenDrain = 0x01
    }
    public enum GpioResistorType {
        None = 0x00,
        PullUp = 0x01,
        PullDown = 0x02
    }
    public enum GpioAlternateFunction {
        None,
        InputCaptureMode,
        InputEncoderMode,
        InputPwmMode,
        InputInterrupt,
        OutputPwmMode
    }
    public enum GpioInterruptTrigger {
        Rising = 0x08,
        Falling = 0x0C,
        RisingFalling = 0x10
    }
}

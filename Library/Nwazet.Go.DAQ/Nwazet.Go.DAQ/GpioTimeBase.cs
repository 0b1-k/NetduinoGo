using System;
using Nwazet.Go.Helpers;
namespace Nwazet.Go.DAQ {
    public class GpioTimeBase {
        public const ulong SystemCoreClockFrequencyHz = 48000000;
        public const ulong HseFrequencyHz = 16000000;
        public const ulong DefaultFrequencykHz = 1000; // 1 kHz
        public const ulong TimerCoreClockRatio = SystemCoreClockFrequencyHz / HseFrequencyHz;

        public GpioTimeBase(GpioTimerId timer, GpioId gpioId) {
            if (timer == GpioTimerId.D0Timer ||
            timer == GpioTimerId.D1Timer ||
            timer == GpioTimerId.D2Timer ||
            timer == GpioTimerId.D3Timer ||
            timer == GpioTimerId.D4Timer ||
            timer == GpioTimerId.D5Timer ||
            timer == GpioTimerId.D6Timer ||
            timer == GpioTimerId.D7Timer) {
                TimerId = timer;
                GpioPinId = gpioId;
            } else throw new ArgumentOutOfRangeException("timer");
            SetTimerFrequency(DefaultFrequencykHz);
        }
        public GpioTimerId Timer {
            get {
                return TimerId;
            }
        }
        public GpioId Gpio {
            get {
                return GpioPinId;
            }
        }
        public ulong TimerFrequencykHz {
            get {
                return FrequencykHz;
            }
        }
        public ulong TimerPeriod {
            get {
                return Period;
            }
        }
        public ulong TimerPrescaler {
            get {
                return Prescaler;
            }
        }
        public void SetTimerFrequency(ulong frequencykHz, UInt16 prescaler = 0) {
            ulong period = ((SystemCoreClockFrequencyHz / (frequencykHz * TimerCoreClockRatio))) - 1;
            if (period <= 65535) {
                FrequencykHz = frequencykHz;
                Period = period;
                Prescaler = prescaler;
            } else throw new ArgumentOutOfRangeException("frequencykHz");
        }
        public void SerializeTimeBase(BasicTypeSerializerContext context) {
            BasicTypeSerializer.Put(context, (byte)GpioPinId);
            BasicTypeSerializer.Put(context, (UInt16)Period);
            BasicTypeSerializer.Put(context, (UInt16)Prescaler);
        }
        protected GpioTimerId TimerId;
        protected GpioId GpioPinId;
        protected ulong FrequencykHz;
        protected ulong Period;
        protected ulong Prescaler;
    }
}

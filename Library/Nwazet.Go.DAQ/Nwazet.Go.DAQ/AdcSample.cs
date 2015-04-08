using System;
using Nwazet.Go.Helpers;
namespace Nwazet.Go.DAQ {
    public class AdcSample : IDisposable {
        public DateTime Time;
        public ushort[] Values;
        public void Get(BasicTypeDeSerializerContext context) {
            Time = TimeStamp.Get(context);
            Values = BasicTypeDeSerializer.Get(context, Values);
        }
        public void Dispose() {
            Values = null;
        }
        public float GetVoltage(ADC channel) {
            return ((Values[(int)channel] * 3300.00f) / 4095.0f) / 1000.0f;
        }
    }
}

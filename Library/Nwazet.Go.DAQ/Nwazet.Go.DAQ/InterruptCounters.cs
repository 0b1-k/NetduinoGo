using System;
using Nwazet.Go.Helpers;
namespace Nwazet.Go.DAQ {
    public class InterruptCounters : IDisposable {
        public UInt32[] Counts = new UInt32[(int)GpioId.GpioCount];
        public UInt32[] RollOverCounts = new UInt32[(int)GpioId.GpioCount];
        public UInt16[] Updated = new UInt16[(int)GpioId.GpioCount];
        ~InterruptCounters() {
            Dispose();
        }
        public void Get(BasicTypeDeSerializerContext context) {
            byte count = BasicTypeDeSerializer.Get(context);
            if ((GpioId)count != GpioId.GpioCount) {
                throw new IndexOutOfRangeException("count");
            }
            for (var i = 0; i < count; i++) {
                Counts[i] = BasicTypeDeSerializer.Get(context, Counts[i]);
                RollOverCounts[i] = BasicTypeDeSerializer.Get(context, RollOverCounts[i]);
                Updated[i] = BasicTypeDeSerializer.Get(context, Updated[i]);
            }
        }
        public void Dispose() {
            Counts = null;
            RollOverCounts = null;
        }
    }
}

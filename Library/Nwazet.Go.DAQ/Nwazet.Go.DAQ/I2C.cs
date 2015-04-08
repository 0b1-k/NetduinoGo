using System;
using Nwazet.Go.Helpers;
namespace Nwazet.Go.DAQ {
    public class I2C : IDisposable {
        protected enum Command {
            Write,
            Read,
            IsBusBusy,
            BusReset
        }
        public enum Speed {
          TenkHz,
          OneHundredkHz,
          FourHundredkHz
        }

        protected enum Result {
            OK,
            Nack,
            BusError,
            ArbitrationLost,
            OverOrUnderRun,
            Timeout,
            InvalidBusSpeed,
            InvalidByteCount
        }

        protected string[] ResultStrings = {
          "OK",
          "Nak",
          "Bus error",
          "Arbitration lost",
          "Over/Under run",
          "Timeout",
          "Invalid bus speed",
          "Invalid byte count"
        };

        protected NwazetDAQ Daq;
        protected BasicTypeSerializerContext SendContext;
        protected BasicTypeDeSerializerContext ReceiveContext;
        protected const int MaxByteChunk = 255;

        public I2C(
            NwazetDAQ daq,
            BasicTypeSerializerContext sendContext,
            BasicTypeDeSerializerContext receiveContext){
            Daq = daq;
            SendContext = sendContext;
            ReceiveContext = receiveContext;
        }
        ~I2C() {
            Dispose();
        }
        public void Write(Speed speed, UInt16 address, byte[] buffer, int offset = 0, UInt16 length = 0) {
            lock (Daq) {
                int byteCount = buffer.Length;
                if (length != 0 && length < buffer.Length) {
                    byteCount = length;
                }
                if (byteCount > offset) {
                    byteCount -= offset;
                }
                while (byteCount != 0) {
                    UInt16 byteChunk = (UInt16)Math.Min(MaxByteChunk, byteCount);
                    BasicTypeSerializer.Put(SendContext, (byte)NwazetDAQ.Command.I2c);
                    BasicTypeSerializer.Put(SendContext, (byte)Command.Write);
                    BasicTypeSerializer.Put(SendContext, (byte)speed);
                    BasicTypeSerializer.Put(SendContext, (UInt16)address);
                    BasicTypeSerializer.Put(SendContext, byteChunk);
                    SendContext.Store(buffer, offset, byteChunk);
                    Daq.Execute();
                    Daq.Receive();
                    CheckResult();
                    offset += byteChunk;
                    byteCount -= byteChunk;
                }
            }
        }
        public void Read(Speed speed, UInt16 address, byte[] buffer, int offset = 0, UInt16 length = 0) {
            lock (Daq) {
                int byteCount = buffer.Length;
                if (length != 0 && length < buffer.Length) {
                    byteCount = length;
                }
                if (byteCount > offset) {
                    byteCount -= offset;
                }
                while (byteCount != 0) {
                    UInt16 byteChunk = (UInt16)Math.Min(MaxByteChunk, byteCount);
                    BasicTypeSerializer.Put(SendContext, (byte)NwazetDAQ.Command.I2c);
                    BasicTypeSerializer.Put(SendContext, (byte)Command.Read);
                    BasicTypeSerializer.Put(SendContext, (byte)speed);
                    BasicTypeSerializer.Put(SendContext, (UInt16)address);
                    BasicTypeSerializer.Put(SendContext, byteChunk);
                    Daq.Execute();
                    Daq.Receive();
                    CheckResult();
                    ReceiveContext.CopyBytesFromInternalBuffer(buffer, offset, byteChunk);
                    offset += byteChunk;
                    byteCount -= byteChunk;
                }
            }
        }
        public bool IsBusBusy() {
            lock (Daq) {
                BasicTypeSerializer.Put(SendContext, (byte)NwazetDAQ.Command.I2c);
                BasicTypeSerializer.Put(SendContext, (byte)Command.IsBusBusy);
                Daq.Execute();
                Daq.Receive();
                CheckResult();
                byte busBusy = BasicTypeDeSerializer.Get(ReceiveContext);
                return (busBusy == 0) ? false : true;
            }
        }
        public void BusReset() {
            lock (Daq) {
                BasicTypeSerializer.Put(SendContext, (byte)NwazetDAQ.Command.I2c);
                BasicTypeSerializer.Put(SendContext, (byte)Command.BusReset);
                Daq.Execute();
            }
        }
        protected void CheckResult() {
            byte result = BasicTypeDeSerializer.Get(ReceiveContext);
            if (result != 0) {
                throw new I2cException((int)result, ResultStrings[result]);
            }
        }
        public void Dispose() {
        }
    }
}

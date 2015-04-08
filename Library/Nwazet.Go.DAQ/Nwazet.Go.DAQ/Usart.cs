using System;
using System.Text;
using Nwazet.Go.Helpers;
namespace Nwazet.Go.DAQ {
    public class Usart : IDisposable {
        protected enum Command {
            SetConfig,
            Write,
            Read
        }
        protected enum Result {
            OK,
            Timeout,
            TerminatorReceived,
            InvalidParameter
        }
        protected string[] ResultStrings = {
          "OK",
          "Timeout",
          "Terminator",
          "Invalid parameter"
        };

        protected NwazetDAQ Daq;
        protected BasicTypeSerializerContext SendContext;
        protected BasicTypeDeSerializerContext ReceiveContext;
        protected const int MaxByteChunk = 512;

        public Usart(
            NwazetDAQ daq,
            BasicTypeSerializerContext sendContext,
            BasicTypeDeSerializerContext receiveContext){
            Daq = daq;
            SendContext = sendContext;
            ReceiveContext = receiveContext;
        }
        ~Usart() {
            Dispose();
        }
        public void SetConfig(UsartConfig config) {
            lock (Daq) {
                BasicTypeSerializer.Put(SendContext, (byte)NwazetDAQ.Command.Usart);
                BasicTypeSerializer.Put(SendContext, (byte)Command.SetConfig);
                config.Put(SendContext);
                Daq.Execute();
                Daq.Receive();
                config.CheckResult(BasicTypeDeSerializer.Get(ReceiveContext));
            }
        }
        public UInt16 Write(string text) {
            return Write(Encoding.UTF8.GetBytes(text), 0, 0);
        }
        public UInt16 Write(byte[] buffer, int offset = 0, UInt16 length = 0) {
            lock (Daq) {
                int byteCount = buffer.Length;
                if (length != 0 && length < buffer.Length) {
                    byteCount = length;
                }
                if (byteCount > offset) {
                    byteCount -= offset;
                }
                UInt16 bytesWritten = 0;
                UInt16 totalBytesWritten = 0;
                while (byteCount != 0) {
                    UInt16 byteChunk = (UInt16)Math.Min(MaxByteChunk, byteCount);
                    BasicTypeSerializer.Put(SendContext, (byte)NwazetDAQ.Command.Usart);
                    BasicTypeSerializer.Put(SendContext, (byte)Command.Write);
                    BasicTypeSerializer.Put(SendContext, byteChunk);
                    SendContext.Store(buffer, offset, byteChunk);
                    Daq.Execute();
                    Daq.Receive();
                    CheckResult();
                    bytesWritten = BasicTypeDeSerializer.Get(ReceiveContext, bytesWritten);
                    totalBytesWritten += bytesWritten;
                    offset += bytesWritten;
                    byteCount -= bytesWritten;
                }
                return totalBytesWritten;
            }
        }
        protected void CheckResult() {
            byte result = BasicTypeDeSerializer.Get(ReceiveContext);
            if (result != 0) {
                throw new UsartException((int)result, ResultStrings[result]);
            }
        }
        public string Read(int length = MaxByteChunk, UInt32 totalTimeoutMs = 0, bool useTerminator = true, byte terminator = (byte)'\r') {
            byte[] buffer = new byte[length];
            var result = Read(buffer, 0, length, totalTimeoutMs, useTerminator, terminator);
            if (result.TotalBytesRead != 0) {
                return result.ToString(buffer);
            }
            return "";
        }
        public UsartReadResult Read(byte[] buffer, int offset = 0, int length = 0, UInt32 totalTimeoutMs = 0, bool useTerminator = false, byte terminator = 0) {
            lock (Daq) {
                var result = new UsartReadResult();
                int byteCount = buffer.Length;
                if (length != 0 && length < buffer.Length) {
                    byteCount = length;
                }
                if (byteCount > offset) {
                    byteCount -= offset;
                }
                while (byteCount != 0) {
                    UInt16 byteChunk = (UInt16)Math.Min(MaxByteChunk, byteCount);
                    BasicTypeSerializer.Put(SendContext, (byte)NwazetDAQ.Command.Usart);
                    BasicTypeSerializer.Put(SendContext, (byte)Command.Read);
                    BasicTypeSerializer.Put(SendContext, byteChunk);
                    BasicTypeSerializer.Put(SendContext, totalTimeoutMs);
                    BasicTypeSerializer.Put(SendContext, (byte)((useTerminator) ? 1 : 0));
                    BasicTypeSerializer.Put(SendContext, (byte)terminator);
                    Daq.Execute();
                    Daq.Receive();
                    byte returnCode = BasicTypeDeSerializer.Get(ReceiveContext);
                    UInt16 bytesRead = 0;
                    bytesRead = BasicTypeDeSerializer.Get(ReceiveContext, bytesRead);
                    ReceiveContext.CopyBytesFromInternalBuffer(buffer, offset, bytesRead);
                    result.TotalBytesRead += bytesRead;
                    offset += bytesRead;
                    byteCount -= bytesRead;
                    switch ((Result)returnCode) {
                        case Result.InvalidParameter:
                            throw new UsartException((int)returnCode, ResultStrings[returnCode]);
                        case Result.TerminatorReceived:
                            result.TerminatorReceived = true;
                            return result;
                        case Result.Timeout:
                            result.TimedOut = true;
                            return result;
                    }
                }
                return result;
            }
        }
        public void Dispose() {
        }
    }
}

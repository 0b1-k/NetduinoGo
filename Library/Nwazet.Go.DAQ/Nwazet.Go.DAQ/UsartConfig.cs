using System;
using Nwazet.Go.Helpers;
namespace Nwazet.Go.DAQ {
    public class UsartConfig {
        public UInt32 BaudRate { get; set; }
        public byte DataBits { get; set; }
        public byte StopBits { get; set; }
        public byte Parity { get; set; }
        public byte HardwareFlowControl { get; set; }
        public UInt16 TransmitTimeoutMs { get; set; }
        public UInt16 ReceiveTimeoutMs { get; set; }

        public const UInt16 TransmitTimeoutMsDefault = 20;
        public const UInt16 ReceiveTimeoutMsDefault = 20;

        public const UInt16 MinRxTxTimeout = 1;
        public const UInt16 MaxRxTxTimeout = 10000;

        public UsartConfig() {
            BaudRate = UsartBaudRate.Baud115200;
            DataBits = UsartDataBits.Eight;
            StopBits = UsartStopBits.One;
            Parity = UsartParity.None;
            HardwareFlowControl = UsartHardwareFlowControl.Disabled;
            TransmitTimeoutMs = TransmitTimeoutMsDefault;
            ReceiveTimeoutMs = ReceiveTimeoutMsDefault;
        }
        public void Put(BasicTypeSerializerContext context) {
            if (DataBits == UsartDataBits.Nine) {
                throw new NotImplementedException("DataBits");
            }
            BasicTypeSerializer.Put(context, BaudRate);
            BasicTypeSerializer.Put(context, DataBits);
            BasicTypeSerializer.Put(context, StopBits);
            BasicTypeSerializer.Put(context, Parity);
            BasicTypeSerializer.Put(context, HardwareFlowControl);
            BasicTypeSerializer.Put(context, TransmitTimeoutMs);
            BasicTypeSerializer.Put(context, ReceiveTimeoutMs);
        }
        protected string[] ResultStrings = {
          "success",
          "baud rate",
          "data bits",
          "stop bits",
          "parity",
          "h/w flow ctrl",
          "tx timeout",
          "rx timeout"
        };
        public void CheckResult(byte result) {
            if (result != 0) {
                throw new UsartException(result, "Invalid parameter: " + ResultStrings[result]);
            }
        }
    }
}

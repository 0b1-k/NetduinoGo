using System;
using System.Text;
using Nwazet.Go.Helpers;
namespace Nwazet.Go.DAQ {
    public class UsartReadResult {
        public UInt16 TotalBytesRead { get; set; }
        public bool TimedOut { get; set; }
        public bool TerminatorReceived { get; set; }
        public string ToString(byte[] buffer) {
            return new string(Encoding.UTF8.GetChars(buffer, 0, TotalBytesRead));
        }
    }
}

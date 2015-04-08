using System;
namespace Nwazet.Go.DAQ {
    public class I2cException : Exception {
        public int Result {
            get {
                return m_HResult;
            }
        }
        public I2cException(int result, string message) : base(message) {
            m_HResult = result;
        }
    }
}

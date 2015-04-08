using System;
namespace Nwazet.Go.DAQ {
    public class UsartException : Exception {
        public int Result {
            get {
                return m_HResult;
            }
        }
        public UsartException(int result, string message) : base(message) {
            m_HResult = result;
        }
    }
}

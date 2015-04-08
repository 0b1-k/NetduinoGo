using System;
namespace Nwazet.Go.DAQ {
    public class FatFsException : Exception {
        public int Result {
            get {
                return m_HResult;
            }
        }
        public FatFsException(int result, string message) : base(message){
            m_HResult = result;
        }
    }
}

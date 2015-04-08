using System;
namespace Nwazet.Go.DAQ {
    public static class DaqSetting {
        public const UInt32 ClockConfigured = 0x00000001;
        public const UInt32 ClockTimeSet = 0x00000002;
        public const UInt32 DiagnosticsToSerial = 0x00000004;
        public const UInt32 FirmataMode = 0x00000008;
    }
}

using System;
using Nwazet.Go.Helpers;
namespace Nwazet.Go.DAQ {
    public static class FileAttribute {
        public const byte ReadOnly = 0x01;
        public const byte Hidden = 0x02;
        public const byte System = 0x04;
        public const byte VolumeLabel = 0x08;
        public const byte LongFilename = 0x0F;
        public const byte Directory = 0x10;
        public const byte Archive = 0x20;
    }
}
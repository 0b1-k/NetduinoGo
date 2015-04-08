using System;
using Nwazet.Go.Helpers;
namespace Nwazet.Go.DAQ {
    public class DiskSpaceInfo {
        public UInt32 DiskCapacityKB;
        public UInt32 FreeDiskSpaceKB;
        public void Get(BasicTypeDeSerializerContext context) {
            DiskCapacityKB = BasicTypeDeSerializer.Get(context, DiskCapacityKB);
            FreeDiskSpaceKB = BasicTypeDeSerializer.Get(context, FreeDiskSpaceKB);
        }
    }
}

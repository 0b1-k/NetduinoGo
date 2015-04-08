using System;
using Nwazet.Go.Helpers;
namespace Nwazet.Go.DAQ {
    public class FileInfo {
	    public UInt32   Size;		    // File size
        public DateTime LastModified;	// Last modified date / time
        public byte Attribute;          // File attributes
        public string Name;             // Short file name (8.3 format)

        public void Get(BasicTypeDeSerializerContext context) {
            UInt16 date = 0;
            UInt16 time = 0;
            Size = BasicTypeDeSerializer.Get(context, Size);
            date = BasicTypeDeSerializer.Get(context, date);
            time = BasicTypeDeSerializer.Get(context, time);
            Attribute = BasicTypeDeSerializer.Get(context);
            Name = BasicTypeDeSerializer.Get(context, Name);

            /*
             * FAT filestamp format:
             * [31:25] - year - 1980
             * [24:21] - month 1..12
             * [20:16] - day 1..31
             * [15:11] - hour 0..23
             * [10:5]  - minute 0..59
             * [4:0]   - second/2 0..29
             * so... midnight 2009 is 0x3a000000
             */
        }
        public bool IsEndOfDirectory {
            get {
                return (Name == null || Name.Length == 0) ? true : false;
            }
        }
        public bool IsReadOnly {
            get {
                return ((Attribute & FileAttribute.ReadOnly) != 0) ? true : false;
            }
        }
        public bool IsHidden {
            get {
                return ((Attribute & FileAttribute.Hidden) != 0) ? true : false;
            }
        }
        public bool IsSystem {
            get {
                return ((Attribute & FileAttribute.System) != 0) ? true : false;
            }
        }
        public bool IsVolumeLabel {
            get {
                return ((Attribute & FileAttribute.VolumeLabel) != 0) ? true : false;
            }
        }
        public bool IsLongFilenameEntry {
            get {
                return ((Attribute & FileAttribute.LongFilename) != 0) ? true : false;
            }
        }
        public bool IsDirectory {
            get {
                return ((Attribute & FileAttribute.Directory) != 0) ? true : false;
            }
        }
        public bool IsArchive {
            get {
                return ((Attribute & FileAttribute.Archive) != 0) ? true : false;
            }
        }
    }
}

using System;
namespace Nwazet.Go.DAQ {
    public static class DaqUtility {
        // See http://rosettacode.org/wiki/Map_range#C for details
        public static double MapRange(double lowRange1, double highRange1, double lowRange2, double highRange2, double data) {
            return lowRange2 + (data - lowRange1) * (highRange2 - lowRange2) / (highRange1 - lowRange1);
        }
    }
}

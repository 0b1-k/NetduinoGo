using System;
using Nwazet.Go.Helpers;
namespace Nwazet.Go.DAQ {
    public class TimeStamp {
        public static DateTime Get(BasicTypeDeSerializerContext context) {
            byte year;
            byte month;
            byte day;
            byte weekDay;
            byte hour;
            byte minute;
            byte second;
            int subSecond = 0;

            year = BasicTypeDeSerializer.Get(context);
            month = BasicTypeDeSerializer.Get(context);
            day = BasicTypeDeSerializer.Get(context);
            weekDay = BasicTypeDeSerializer.Get(context);

            hour = BasicTypeDeSerializer.Get(context);
            minute = BasicTypeDeSerializer.Get(context);
            second = BasicTypeDeSerializer.Get(context);
            subSecond = BasicTypeDeSerializer.Get(context, subSecond);

            return new DateTime((int)2000 + year, (int)month, (int)day, (int)hour, (int)minute, (int)second, (int)subSecond);
        }
        public static void Put(BasicTypeSerializerContext context, DateTime dt) {
            BasicTypeSerializer.Put(context, (byte)(dt.Year - 2000));
            BasicTypeSerializer.Put(context, (byte)dt.Month);
            BasicTypeSerializer.Put(context, (byte)dt.Day);
            BasicTypeSerializer.Put(context, (byte)dt.DayOfWeek);
            BasicTypeSerializer.Put(context, (byte)dt.Hour);
            BasicTypeSerializer.Put(context, (byte)dt.Minute);
            BasicTypeSerializer.Put(context, (byte)dt.Second);
        }
    }
}

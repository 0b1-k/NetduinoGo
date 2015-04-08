using System;
using Nwazet.Go.Helpers;
namespace Nwazet.Go.Joystick {
    public class JoystickData {
        public UInt16 X;
        public UInt16 Y;
        public void Get(BasicTypeDeSerializerContext context) {
            X = BasicTypeDeSerializer.Get(context, X);
            Y = BasicTypeDeSerializer.Get(context, Y);
        }
    }
}

using System;
using Nwazet.Go.Helpers;
namespace Nwazet.Go.DAQ {
    public class GpioInputInterrupt : Gpio {
        public const UInt32 DefaultInterruptCountRollOverLimit = 0xFFFFFFFF;
        public GpioInterruptTrigger Trigger { get; set; }
        public UInt32 RollOverCountLimit { get; set; }
        public bool Enable { get; set; }
        public override bool State {
            set{
                throw new InvalidOperationException("Unsupported");
            } 
        }
        public GpioInputInterrupt(
            GpioId Id, 
            GpioResistorType resistorType = GpioResistorType.PullUp, 
            GpioInterruptTrigger trigger = GpioInterruptTrigger.Falling,
            UInt32 rolloverLimit = DefaultInterruptCountRollOverLimit,
            bool enable = true) {
                if (Id == GpioId.D4_33v) {
                    throw new NotSupportedException("GpioId.GpioD4_33v");
                }
            Identifier = Id;
            Mode = GpioMode.Input;
            Type = GpioType.PushPull;
            ResistorType = resistorType;
            AlternateFunction = GpioAlternateFunction.InputInterrupt;
            Trigger = trigger;
            Enable = enable;
            RollOverCountLimit = rolloverLimit;
        }
        public override void SerializeDefinition(BasicTypeSerializerContext context) {
            base.SerializeDefinition(context);
            BasicTypeSerializer.Put(context, (byte)((Enable == true) ? (byte)1 : (byte)0));
            BasicTypeSerializer.Put(context, (byte)Trigger);
            BasicTypeSerializer.Put(context, (UInt32)RollOverCountLimit);
        }
    }
}

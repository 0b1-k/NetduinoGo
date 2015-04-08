using System;
namespace Nwazet.Go.DAQ {
    public class DaqConfig : IDisposable {
        protected UInt32[] Registers;
        public DaqConfig(NwazetDAQ daq) {
            Registers = daq.GetSystemRegisters();
        }
        public bool IsOptionEnabled(UInt32 option) {
            return ((Registers[(int)Register.System] & option) != 0) ? true : false;
        }
        public void SetOption(UInt32 option, bool state) {
            if (state) {
                Registers[(int)Register.System] |= option;
            } else {
                Registers[(int)Register.System] &= ~option;
            }
        }
        public UInt32 GetSystemRegister() {
            return Registers[(int)Register.System];
        }
        public UInt32 GetUserRegister(Register register) {
            if (register > Register.System && register <= Register.User4) {
                return Registers[(int)register];
            } else {
                throw new ArgumentOutOfRangeException("register");
            }
        }
        public void SetUserRegister(Register register, UInt32 data) {
            if (register > Register.System && register <= Register.User4) {
                Registers[(int)register] = data;
            } else {
                throw new ArgumentOutOfRangeException("register");
            }
        }
        public void Commit(NwazetDAQ daq) {
            daq.SetSystemRegisters(Registers);
        }
        public void Dispose() {
            Registers = null;
        }
    }
}

/*
[nwazet Open Source Software & Open Source Hardware
Authors: Fabien Royer
Software License Agreement (BSD License)

Copyright (c) 2010-2012, Nwazet, LLC. All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

* Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
* Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
* Neither the name of Nwazet, LLC. nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.
* The names '[nwazet', 'nwazet', the ASCII hazelnut in the [nwazet logo and the color of the logo are Trademarks of nwazet, LLC. and cannot be used to endorse or promote products derived from this software or any hardware designs without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/
using GoBus;
using System;
using System.Threading;
using System.Collections;
using Nwazet.Go.Helpers;
using Microsoft.SPOT;
using Microsoft.SPOT.Hardware;
namespace Nwazet.Go.DAQ {
    public class NwazetDAQ : GoModule {
        public enum Command {
            SetDateTime,
            GetDateTime,
            GetClockState,
            DefineDigitalGpio,
            SetDigitalOutputState,
            PulseDigitalOutputState,
            GetDigitalInputState,
            SetPwmOutputDutyCycle,
            SetLedState,
            GetAdcSample,
            Reboot,
            FileSystem,
            GetBackupRegisters,
            SetBackupRegisters,
            Usart,
            I2c,
            SetPwmTimeBase,
            UndefineDigitalGpio,
            GetInterruptCounters
        }
        public const int MaxSpiTxBufferSize = 512+64;
        public const int MaxSpiRxBufferSize = 512+64;
        public const int MaxGpios = 8;

        protected BasicTypeSerializerContext SendContext;
        protected BasicTypeDeSerializerContext ReceiveContext;
        protected SPI Spi;
        protected InterruptPort GoBusIrqPort;
        protected ManualResetEvent GoBusIrqEvent;
        protected Gpio[] Gpios = new Gpio[MaxGpios];
        protected ArrayList Timers = new ArrayList();
        private byte[] _spiRxBuffer;
        private bool _moduleReady;

        public readonly FileSystem FatFs;
        public readonly Usart UsartPort;
        public readonly I2C I2cPort;
        
        protected AdcSample AnalogChannels = new AdcSample();
        protected InterruptCounters Counters = new InterruptCounters();
        protected Thread InterruptMonitorThread;

        public event InterruptEventEventHandler Interrupts;

        public object GlobalLock = new object();

        public NwazetDAQ() {
            _spiRxBuffer = new byte[MaxSpiRxBufferSize];
            SendContext = new BasicTypeSerializerContext(MaxSpiTxBufferSize);
            ReceiveContext = new BasicTypeDeSerializerContext();
            GoBusIrqEvent = new ManualResetEvent(false);
            RegisterTimer(new GpioTimeBase(GpioTimerId.D0Timer, GpioId.D0));
            RegisterTimer(new GpioTimeBase(GpioTimerId.D1Timer, GpioId.D1));
            RegisterTimer(new GpioTimeBase(GpioTimerId.D2Timer, GpioId.D2));
            RegisterTimer(new GpioTimeBase(GpioTimerId.D3Timer, GpioId.D3));
            RegisterTimer(new GpioTimeBase(GpioTimerId.D4Timer, GpioId.D4_33v));
            RegisterTimer(new GpioTimeBase(GpioTimerId.D5Timer, GpioId.D5));
            RegisterTimer(new GpioTimeBase(GpioTimerId.D6Timer, GpioId.D6));
            RegisterTimer(new GpioTimeBase(GpioTimerId.D7Timer, GpioId.D7));

            FatFs = new FileSystem(this, SendContext, ReceiveContext);
            UsartPort = new Usart(this, SendContext, ReceiveContext);
            I2cPort = new I2C(this, SendContext, ReceiveContext);
        }
        ~NwazetDAQ() {
            Dispose(true);
        }
        public void Initialize(GoSocket socket, uint speedKHz = 16000) {
            if (speedKHz < 5000 || speedKHz > 16000) throw new ArgumentException("speedKHz");
            if (socket == null) throw new ArgumentNullException("socket");

            SPI.SPI_module spiModule;
            Cpu.Pin chipSelect;
            Cpu.Pin gpioGoBus;
            socket.GetPhysicalResources(out gpioGoBus, out spiModule, out chipSelect);
            if (!BindSocket(socket)) throw new InvalidOperationException("socket already bound");

            SetSocketPowerState(false);
            Thread.Sleep(2000);
            SetSocketPowerState(true);
            Thread.Sleep(250);

            Spi = new SPI(new SPI.Configuration(
                SPI_mod: spiModule,
                ChipSelect_Port: chipSelect,
                ChipSelect_ActiveState: false,
                ChipSelect_SetupTime: 0,
                ChipSelect_HoldTime: 0,
                Clock_IdleState: false,
                Clock_Edge: false,
                Clock_RateKHz: speedKHz));

            GoBusIrqPort = new InterruptPort(gpioGoBus, false, Port.ResistorMode.PullUp, Port.InterruptMode.InterruptEdgeLow);
            GoBusIrqPort.OnInterrupt += OnGoBusIrq;

            WaitUntilModuleIsInitialized();
        }
        protected void RegisterTimer(GpioTimeBase timer) {
            if (!Timers.Contains(timer.Timer)) {
                Timers.Add(timer);
            }
        }
        public void Execute() {
            lock (this) {
                int contentSize;
                var spiTxBuffer = SendContext.GetBuffer(out contentSize);
                if (contentSize >= MaxSpiTxBufferSize) {
                    throw new ApplicationException("contentSize");
                }
                GoBusIrqEvent.Reset();
                Spi.WriteRead(spiTxBuffer, 0, MaxSpiTxBufferSize, _spiRxBuffer, 0, MaxSpiRxBufferSize, 0);
                WaitUntilGoBusIrqIsAsserted();
            }
        }
        public void Receive() {
            lock (this) {
                SendContext.Wipe();
                Execute();
                ReceiveContext.Bind(_spiRxBuffer, BasicTypeDeSerializerContext.BufferStartOffsetDefault);
            }
        }
        protected void WaitUntilGoBusIrqIsAsserted() {
            GoBusIrqEvent.WaitOne();
        }
        private const byte _identifier8bitCrc = 0x4e;
        public void WaitUntilModuleIsInitialized() {
            while (!_moduleReady) {
                Execute();
                if (_spiRxBuffer[0] == 0x80 &&
                    _spiRxBuffer[1] == '[' &&
                    _spiRxBuffer[2] == 'n' &&
                    _spiRxBuffer[3] == 'w' &&
                    _spiRxBuffer[4] == 'a' &&
                    _spiRxBuffer[5] == 'z' &&
                    _spiRxBuffer[6] == 'e' &&
                    _spiRxBuffer[7] == 't' &&
                    _spiRxBuffer[8] == '.' &&
                    _spiRxBuffer[9] == 'd' &&
                    _spiRxBuffer[10] == 'a' &&
                    _spiRxBuffer[11] == 'q' &&
                    _spiRxBuffer[12] == 'L') {
                    if (_spiRxBuffer[17] != _identifier8bitCrc) throw new ApplicationException("SPI data corruption");
                    _moduleReady = true;
                    return;
                }
                Thread.Sleep(200);
            }
        }
        private void OnGoBusIrq(UInt32 data1, UInt32 data2, DateTime timestamp) {
            GoBusIrqPort.ClearInterrupt();
            GoBusIrqEvent.Set();
        }
        protected override void Dispose(bool disposing) {
            // high level objects
            FatFs.Dispose();
            UsartPort.Dispose();
            I2cPort.Dispose();
            AnalogChannels.Dispose();
            AnalogChannels = null;
            Counters.Dispose();
            Counters = null;
            // lower level objects
            StopInterruptMonitor();
            Spi.Dispose();
            GoBusIrqPort.Dispose();
            SendContext.Dispose();
            ReceiveContext.Dispose();
            Timers.Clear();
            Timers = null;
            Gpios = null;
            _spiRxBuffer = null;
            GoBusIrqEvent = null;
            InterruptCounterMonitorTerminateEvent = null;
            SetSocketPowerState(false);
            base.Dispose(disposing);
        }
        public void SetDateTime(DateTime dt){
            lock (this) {
                BasicTypeSerializer.Put(SendContext, (byte)Command.SetDateTime);
                TimeStamp.Put(SendContext, dt);
                Execute();
            }
        }
        public DateTime GetDateTime(){
            lock (this) {
                BasicTypeSerializer.Put(SendContext, (byte)Command.GetDateTime);
                Execute();
                Receive();
                return TimeStamp.Get(ReceiveContext);
            }
        }
        public enum ClockState {
            Invalid,
            Valid
        }
        public ClockState GetClockState() {
            lock (this) {
                BasicTypeSerializer.Put(SendContext, (byte)Command.GetClockState);
                Execute();
                Receive();
                ClockState state = (ClockState)BasicTypeDeSerializer.Get(ReceiveContext);
                return state;
            }
        }
        public AdcSample ReadAnalogInputs() {
            lock (this) {
                BasicTypeSerializer.Put(SendContext, (byte)Command.GetAdcSample);
                Execute();
                Receive();
                AnalogChannels.Get(ReceiveContext);
                return AnalogChannels;
            }
        }
        public void SetLed(LedType ledType, uint ledIndex, LedState state, bool autoExecute = true) {
            if ((uint)ledIndex > 7 && (uint)ledIndex != 0xff) throw new IndexOutOfRangeException("ledIndex");
            lock (this) {
                BasicTypeSerializer.Put(SendContext, (byte)Command.SetLedState);
                BasicTypeSerializer.Put(SendContext, (byte)ledType);
                BasicTypeSerializer.Put(SendContext, (byte)ledIndex);
                BasicTypeSerializer.Put(SendContext, (byte)state);
                if (autoExecute) {
                    Execute();
                }
            }
        }
        public void Reboot() {
            lock (this) {
                BasicTypeSerializer.Put(SendContext, (byte)Command.Reboot);
                Execute();
            }
        }
        public Gpio Add(Gpio gpio) {
            if (GetGpio(gpio.Identifier) != null) throw new ArgumentException("Gpio already defined");
            lock (this) {
                Gpios[(int)gpio.Identifier] = gpio;
                BasicTypeSerializer.Put(SendContext, (byte)Command.DefineDigitalGpio);
                gpio.SerializeDefinition(SendContext);
                SetLed(LedType.Digital, (uint)gpio.Identifier, LedState.On, false);
                Execute();
                return gpio;
            }
        }
        public void Remove(Gpio gpio) {
            if (GetGpio(gpio.Identifier) == null) throw new ArgumentException("Gpio not found");
            lock (this) {
                BasicTypeSerializer.Put(SendContext, (byte)Command.UndefineDigitalGpio);
                BasicTypeSerializer.Put(SendContext, (byte)gpio.Identifier);
                SetLed(LedType.Digital, (uint)gpio.Identifier, LedState.Off, false);
                Execute();
                Gpios[(int)gpio.Identifier] = null;
            }
        }
        protected void Serialize(GpioOutput gpio) {
            BasicTypeSerializer.Put(SendContext, (byte)Command.SetDigitalOutputState);
            gpio.SerializeState(SendContext);
        }
        public void Write(GpioOutput gpio) {
            lock (this) {
                Serialize(gpio);
                Execute();
            }
        }
        public void Write(GpioOutput gpio, bool state) {
            lock (this) {
                gpio.State = state;
                Write(gpio);
            }
        }
        protected void Serialize(GpioOutputPwm gpio) {
            BasicTypeSerializer.Put(SendContext, (byte)Command.SetPwmOutputDutyCycle);
            gpio.SerializeDutyCycle(SendContext, (GpioTimeBase)Timers[(int)gpio.Identifier]);
        }
        public void Write(GpioOutputPwm gpio) {
            lock (this) {
                Serialize(gpio);
                Execute();
            }
        }
        public void Write(GpioOutputPwm gpio, float dutyCycle) {
            lock (this) {
                gpio.DutyCycle = dutyCycle;
                Write(gpio);
            }
        }
        public void Write() {
            lock (this) {
                foreach (var gpio in Gpios) {
                    if (gpio != null) {
                        if (gpio.Mode == GpioMode.Output && gpio.AlternateFunction == GpioAlternateFunction.None) {
                            Serialize((GpioOutput)gpio);
                        }
                        if (gpio.Mode == GpioMode.Output && gpio.AlternateFunction == GpioAlternateFunction.OutputPwmMode) {
                            Serialize((GpioOutputPwm)gpio);
                        }
                    }
                }
                Execute();
            }
        }
        public bool Read(GpioInput gpio) {
            lock (this) {
                BasicTypeSerializer.Put(SendContext, (byte)Command.GetDigitalInputState);
                BasicTypeSerializer.Put(SendContext, (byte)gpio.Identifier);
                Execute();
                Receive();
                byte state = BasicTypeDeSerializer.Get(ReceiveContext);
                if (state == 255) throw new ApplicationException("Invalid Gpio type or Identifier");
                return (state == 1) ? true : false;
            }
        }
        public void Read() {
            lock (this) {
                BasicTypeSerializer.Put(SendContext, (byte)Command.GetDigitalInputState);
                BasicTypeSerializer.Put(SendContext, (byte)GpioId.GpioCount);
                Execute();
                Receive();
                GpioId id = GpioId.GpioCount;
                while (true) {
                    id = (GpioId)BasicTypeDeSerializer.Get(ReceiveContext);
                    if (id == GpioId.GpioCount) {
                        break;
                    }
                    byte state = BasicTypeDeSerializer.Get(ReceiveContext);
                    Gpios[(int)id].State = (state == 0) ? false : true;
                }
            }
        }
        public void Pulse(GpioOutput gpio, bool pulseStartState = true) {
            lock (this) {
                gpio.State = pulseStartState;
                BasicTypeSerializer.Put(SendContext, (byte)Command.PulseDigitalOutputState);
                gpio.SerializeState(SendContext);
                Execute();
            }
        }
        public Gpio GetGpio(GpioId Id) {
            lock (this) {
                return Gpios[(int)Id];
            }
        }
        public const int MaxRtcBackupRegisters = 5;
        public UInt32[] GetSystemRegisters() {
            lock (this) {
                var registers = new UInt32[MaxRtcBackupRegisters];
                BasicTypeSerializer.Put(SendContext, (byte)Command.GetBackupRegisters);
                Execute();
                Receive();
                UInt32 register = 0;
                registers[(int)Register.System] = BasicTypeDeSerializer.Get(ReceiveContext, register);
                registers[(int)Register.User1] = BasicTypeDeSerializer.Get(ReceiveContext, register);
                registers[(int)Register.User2] = BasicTypeDeSerializer.Get(ReceiveContext, register);
                registers[(int)Register.User3] = BasicTypeDeSerializer.Get(ReceiveContext, register);
                registers[(int)Register.User4] = BasicTypeDeSerializer.Get(ReceiveContext, register);
                return registers;
            }
        }
        public void SetSystemRegisters(UInt32[] registers) {
            lock (this) {
                BasicTypeSerializer.Put(SendContext, (byte)Command.SetBackupRegisters);
                BasicTypeSerializer.Put(SendContext, registers[(int)Register.System]);
                BasicTypeSerializer.Put(SendContext, registers[(int)Register.User1]);
                BasicTypeSerializer.Put(SendContext, registers[(int)Register.User2]);
                BasicTypeSerializer.Put(SendContext, registers[(int)Register.User3]);
                BasicTypeSerializer.Put(SendContext, registers[(int)Register.User4]);
                Execute();
            }
        }
        public void SetGpioPwmTimeBase(GpioId gpioId, ulong frequencykHz, UInt16 prescaler = 0) {
            lock (this) {
                var timeBase = (GpioTimeBase)Timers[(int)gpioId];
                timeBase.SetTimerFrequency(frequencykHz, prescaler);
                BasicTypeSerializer.Put(SendContext, (byte)Command.SetPwmTimeBase);
                timeBase.SerializeTimeBase(SendContext);
                Execute();
            }
        }
        
        public bool InterruptCountersResetAfterReading { get; set; }
        public int InterruptCountersSamplingPeriodMs { get; set; }
        protected ManualResetEvent InterruptCounterMonitorTerminateEvent = new ManualResetEvent(false);

        public void StartInterruptMonitor(bool resetAfterReadingCounters = false, int periodMs = 5) {
            if (Interrupts == null) throw new ArgumentOutOfRangeException("undefined interrupt handler");
            if (periodMs < 1 || periodMs > 1000) throw new ArgumentOutOfRangeException("periodMs");
            if(InterruptMonitorThread == null){
                InterruptCountersResetAfterReading = resetAfterReadingCounters;
                InterruptCountersSamplingPeriodMs = periodMs;
                InterruptCounterMonitorTerminateEvent.Reset();
                InterruptMonitorThread = new Thread(InterruptMonitor);
                InterruptMonitorThread.Start();
            }
        }
        public void StopInterruptMonitor() {
            if (InterruptMonitorThread != null) {
                InterruptCounterMonitorTerminateEvent.Set();
                InterruptMonitorThread.Join();
                InterruptMonitorThread = null;
            }
        }
        protected void InterruptMonitor() {
            while (InterruptCounterMonitorTerminateEvent.WaitOne(InterruptCountersSamplingPeriodMs, false) == false) {
                PollInterruptCounters();
            }
        }
        public void PollInterruptCounters() {
            lock (this) {
                if (Interrupts != null) {
                    BasicTypeSerializer.Put(SendContext, (byte)Command.GetInterruptCounters);
                    BasicTypeSerializer.Put(SendContext, (byte)((InterruptCountersResetAfterReading == true) ? (byte)1 : (byte)0));
                    Execute();
                    Receive();
                    Counters.Get(ReceiveContext);
                    DispatchInterruptEvents();
                }
            }
        }
        protected void DispatchInterruptEvents() {
            var intEvent = new InterruptEvent();
            var maxGpioCount = (int)GpioId.GpioCount;
            for(var i = 0; i < maxGpioCount; i++) {
                if (Counters.Updated[i] != 0) {
                    intEvent.Count = Counters.Counts[i];
                    intEvent.RollOverCount = Counters.RollOverCounts[i];
                    intEvent.Id = (GpioId)i;
                    Interrupts(intEvent);
                }
            }
        }
    }
}
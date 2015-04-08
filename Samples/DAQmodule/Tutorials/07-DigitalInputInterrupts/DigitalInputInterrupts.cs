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
using System;
using System.Threading;
using Microsoft.SPOT;
using Microsoft.SPOT.Hardware;
using SecretLabs.NETMF.Hardware.NetduinoGo;
using Nwazet.Go.DAQ;
namespace Tutorials {
    public class DigitalInputInterrupts {
        public static NwazetDAQ daq = new NwazetDAQ();
        public static void Main() {
            daq.Initialize(GoSockets.Socket1);

            // Register an interrupt handler
            daq.Interrupts += new InterruptEventEventHandler(InterruptHandler);

            // Instantiate digital inputs with interrupt support
            var d0 = new GpioInputInterrupt(GpioId.D0);
            var d1 = new GpioInputInterrupt(GpioId.D1);
            var d2 = new GpioInputInterrupt(GpioId.D2);
            var d3 = new GpioInputInterrupt(GpioId.D3);
            var d5 = new GpioInputInterrupt(GpioId.D5);
            var d6 = new GpioInputInterrupt(GpioId.D6);
            var d7 = new GpioInputInterrupt(GpioId.D7);
            
            // Set some arbitrary roll-over count limits...
            d0.RollOverCountLimit = 100;
            d1.RollOverCountLimit = 50;
            d2.RollOverCountLimit = 10000;
            d3.RollOverCountLimit = 255;
            d5.RollOverCountLimit = 500;
            d6.RollOverCountLimit = 300;
            d7.RollOverCountLimit = 20;

            // Add them to the DAQ
            daq.Add(d0);
            daq.Add(d1);
            daq.Add(d2);
            daq.Add(d3);
            daq.Add(d5);
            daq.Add(d6);
            daq.Add(d7);

            Debug.Print("Asynchronous interrupt polling for 15 seconds");

            // Auto-monitoring of interrupt events
            daq.StartInterruptMonitor();

            // Continue working with the application and the DAQ...
            // until StopInterruptMonitor() gets called. Instead, just sleep in this example...
            Thread.Sleep(15000);

            daq.StopInterruptMonitor();

            Debug.Print("Synchronous interrupt polling 1000 times");

            // Synchronous user-controlled interrupt event polling...
            var count = 1000;
            while (count-- != 0) {
                daq.PollInterruptCounters();
            }

            daq.Remove(d0);
            daq.Remove(d1);
            daq.Remove(d2);
            daq.Remove(d3);
            daq.Remove(d5);
            daq.Remove(d6);
            daq.Remove(d7);
        }
        public static void InterruptHandler(InterruptEvent intEvent) {
            Debug.Print("Interrupt on Gpio:" + intEvent.Id + ", Count: " + intEvent.Count + ", Rollover: " + intEvent.RollOverCount);
        }
    }
}

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
using Microsoft.SPOT;
using System.Text;
using System.IO.Ports;
namespace SerialLib {
    public delegate void SerialMessengerHandler(SerialMessenger sender, byte[] message, int offset, int length);

    public class SerialMessenger : IDisposable {
        protected SerialPort Uart;
        protected UTF8Encoding Encoder = new UTF8Encoding();
        protected byte[] Incoming;
        protected int IncomingIndex;

        public event SerialMessengerHandler Messenger;

        public void Initialize(string port = "COM1", int baudRate = 57600, Parity parity = Parity.None, int dataBits = 8, StopBits stopBits = StopBits.One, int bufferSize = 80) {
            Incoming = new byte[bufferSize];
            Uart = new SerialPort(port, baudRate, parity, dataBits, stopBits);
            Uart.Open();
            Uart.DataReceived += new SerialDataReceivedEventHandler(DataReceivedHandler);
            IncomingIndex = 0;
        }
        ~SerialMessenger() {
            Dispose();
        }
        public void Dispose() {
            Uart.Flush();
            Uart.DataReceived -= DataReceivedHandler;
            Uart.Close();
            Uart.Dispose();
        }
        public void Send(string line) {
            Send(Encoder.GetBytes(line),0, (byte)line.Length);
        }
        public void SendAck() {
            Send(Ack, 0, (byte)Ack.Length);
        }
        public void SendNack() {
            Send(Nack, 0, (byte)Nack.Length);
        }

        protected byte[] FrameStart = new byte[] { 0x7e, 0x00 };
        protected byte[] FrameStop = new byte[] { 0x7e };
        protected byte[] Ack = new byte[] { 0xaa };
        protected byte[] Nack = new byte[] { 0xff };

        public void Send(byte[] data, int offset, byte count){
            FrameStart[1] = (byte)(count + FrameStart.Length + FrameStop.Length);
            Uart.Write(FrameStart, 0, FrameStart.Length);
            Uart.Flush();
            Uart.Write(data, offset, count);
            Uart.Flush();
            Uart.Write(FrameStop, 0, FrameStop.Length);
            Uart.Flush();
        }
        protected byte FrameComplete;
        protected byte FrameLength;
        protected bool CaptureFrameLength = false;
        protected int ReceivedByteCount;
        
        public int TotalByteReceived;

        private void DataReceivedHandler(object sender, SerialDataReceivedEventArgs e) {
            var sp = (SerialPort)sender;
            var readCount = sp.BytesToRead;
            while (readCount > 0) {
                if (IncomingIndex < Incoming.Length) {
                    sp.Read(Incoming, IncomingIndex++, 1);
                    readCount--;
                    ReceivedByteCount++;
                    TotalByteReceived++;
                    if (CaptureFrameLength) {
                        FrameLength = Incoming[IncomingIndex - 1];
                        CaptureFrameLength = false;
                    }
                    if (Incoming[IncomingIndex - 1] == 0x7e) {
                        switch (FrameComplete) {
                            case 0: // start of a frame
                                FrameComplete = 1;
                                FrameLength = 0;
                                CaptureFrameLength = true; // capture the frame length in the next byte
                                break;
                            case 1: // end of a frame marker
                                if ((IncomingIndex >= FrameLength) && Incoming[IncomingIndex - FrameLength + 1] == FrameLength) { // end of frame if the whole frame length has been received only
                                    if (Messenger != null) { // process the payload embedded in the frame
                                        Messenger(this, Incoming, (IncomingIndex - FrameLength) + FrameStart.Length, FrameLength - (FrameStart.Length + FrameStop.Length));
                                        FrameComplete = 0; // Get ready for the next frame
                                        ReceivedByteCount = 0;
                                    } else {
                                        // no message handler, discard everything
                                        FrameComplete = 0;
                                        IncomingIndex = 0;
                                        ReceivedByteCount = 0;
                                        sp.DiscardInBuffer();
                                        SendNack();
                                    }
                                } else {
                                    // something went wrong with the transmission: reset everything.
                                    FrameComplete = 0;
                                    IncomingIndex = 0;
                                    ReceivedByteCount = 0;
                                    sp.DiscardInBuffer();
                                    SendNack();
                                }
                            break;
                        }
                    }
                } else {
                    // case where the buffer rolls over: move the existing data to the beginning of the buffer
                    Array.Copy(Incoming, Incoming.Length - ReceivedByteCount, Incoming, 0, ReceivedByteCount);
                    IncomingIndex = ReceivedByteCount;
                }
            }
        }
    }
}

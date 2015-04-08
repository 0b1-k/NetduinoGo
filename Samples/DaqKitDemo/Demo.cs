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
using Komodex.NETMF;
using GoBus;
using Nwazet.Go.Imaging;
using Nwazet.Go.Fonts;
using NetduinoGo;
using Honeywell;
using Bosch;
using Taos;
namespace Tutorials {
    public class Demo {
        public static NwazetDAQ daq = new NwazetDAQ();
        public static VirtualCanvas canvas = new VirtualCanvas(null, null);
        public static SevenSegmentDisplay seg = new SevenSegmentDisplay(GoSockets.Socket3);
        public static Potentiometer pot = new Potentiometer(GoSockets.Socket6);
        public static void Main() {
            daq.Initialize(GoSockets.Socket4);
            
            var state = daq.GetClockState();
            if (state == NwazetDAQ.ClockState.Invalid) {
                daq.SetDateTime(new DateTime(2012, 10, 05, 20, 39, 00));
            }

            var usartConfig = new UsartConfig();
            usartConfig.BaudRate = UsartBaudRate.Baud57600;
            daq.UsartPort.SetConfig(usartConfig);

            daq.UsartPort.Write("\r\n\r\nNwazet DAQ Pro Kit - demo\r\n");

            canvas.Initialize(GoSockets.Socket5);
            canvas.SetOrientation(Orientation.Landscape);

            InitDisplay();

            seg.SetBrightness(.1f);
            seg.SetColon(true);

            while (true) {
                var hih = new HIH613x(daq.I2cPort);
                var bmp085 = new BoschBmp085(daq.I2cPort);
                var taos = new TaosTSL256x(daq.I2cPort);
                
                var pressure = 0;
                AdcSample sample = null;
                try {
                    while (true) {
                        try {
                            seg.SetBrightness(pot.GetValue());
                            sample = daq.ReadAnalogInputs();
                            ShowTime(sample.Time);
                            hih.Read();
                            pressure = bmp085.ReadPressurePascals();
                            taos.Read();
                        } catch (I2cException e) {
                            Debug.Print("I2C transaction failed: " + e.Message);
                            daq.I2cPort.BusReset();
                        }
                        UpdateDisplay(hih.TemperatureCelsius, hih.RelativeHumidityPercent, pressure, taos.Lux, taos.InfraredSpectrum, sample);

                        daq.UsartPort.Write(
                            sample.Time.ToString() + "," + 
                            hih.TemperatureCelsius + "," +
                            hih.RelativeHumidityPercent + "," +
                            pressure + "," +
                            taos.Lux + "," +
                            taos.InfraredSpectrum + "," +
                            sample.Values[(int)ADC.A0].ToString()
                             + "\r\n");
                    }
                } catch(Exception e) {
                    Debug.Print("Other exception: " + e.Message);
                    hih.Dispose();
                    bmp085.Dispose();
                    taos.Dispose();
                }
            }
        }
        const UInt16 TextAlignmentX = 184;

        public static void UpdateDisplay(ushort temp, ushort humidity, Int32 pressurePa, UInt32 lux, UInt16 infraredSpectrum, AdcSample sample) {
            canvas.DrawRectangleFilled(
                180, 30,
                300, 162,
                ColorHelpers.GetRGB24toRGB565(0, 0, 0));
            canvas.DrawString(
                TextAlignmentX, 40,
                ColorHelpers.GetRGB24toRGB565(255, 255, 255),
                VerdanaBold14.ID,
                temp.ToString());
            canvas.DrawString(
                TextAlignmentX, 65,
                ColorHelpers.GetRGB24toRGB565(255, 255, 255),
                VerdanaBold14.ID,
                humidity.ToString());
            canvas.DrawString(
                TextAlignmentX, 90,
                ColorHelpers.GetRGB24toRGB565(255, 255, 255),
                VerdanaBold14.ID,
                lux.ToString());
            canvas.DrawString(
                TextAlignmentX, 115,
                ColorHelpers.GetRGB24toRGB565(255, 255, 255),
                VerdanaBold14.ID,
                infraredSpectrum.ToString());
            canvas.DrawString(
                TextAlignmentX, 140,
                ColorHelpers.GetRGB24toRGB565(255, 255, 255),
                VerdanaBold14.ID,
                pressurePa.ToString());
            canvas.DrawProgressBar(
                148, 171,
                154, 10,
                RoundedCornerStyle.All,
                RoundedCornerStyle.All,
                ColorHelpers.GetRGB24toRGB565(214, 214, 214),
                ColorHelpers.GetRGB24toRGB565(108, 224, 217),
                ColorHelpers.GetRGB24toRGB565(255, 255, 255),
                ColorHelpers.GetRGB24toRGB565(233, 15, 7),
                (int)DaqUtility.MapRange(0, 4095, 0, 100, (double)sample.Values[(int)ADC.A0]));
            canvas.Execute();
        }
        public static void ShowTime(DateTime dt) {
            seg.SetValue(dt, false, false);
        }
        public static void InitDisplay() {
            canvas.DrawFill(ColorHelpers.GetRGB24toRGB565(0, 0, 0));
            canvas.DrawString(
                68, 3,
                ColorHelpers.GetRGB24toRGB565(255, 255, 255),
                VerdanaBold14.ID,
                "Nwazet Go Pro Kit");
            canvas.DrawString(
                10, 40,
                ColorHelpers.GetRGB24toRGB565(255, 255, 255),
                VerdanaBold14.ID,
                "Temp (C)...............:");
            canvas.DrawString(
                10, 65,
                ColorHelpers.GetRGB24toRGB565(255, 255, 255),
                VerdanaBold14.ID,
                "Humidity (%).......:");
            canvas.DrawString(
                10, 90,
                ColorHelpers.GetRGB24toRGB565(255, 255, 255),
                VerdanaBold14.ID,
                "Luminosity (Lux):");
            canvas.DrawString(
                10, 115,
                ColorHelpers.GetRGB24toRGB565(255, 255, 255),
                VerdanaBold14.ID,
                "Luminosity (IR)...:");
            canvas.DrawString(
                10, 140,
                ColorHelpers.GetRGB24toRGB565(255, 255, 255),
                VerdanaBold14.ID,
                "Pressure (Pa).......:");
            canvas.DrawString(
                10, 165,
                ColorHelpers.GetRGB24toRGB565(255, 255, 255),
                VerdanaBold14.ID,
                "Water Level: ");
            canvas.DrawProgressBar(
                148, 171,
                154, 10,
                RoundedCornerStyle.All,
                RoundedCornerStyle.All,
                ColorHelpers.GetRGB24toRGB565(214, 214, 214),
                ColorHelpers.GetRGB24toRGB565(108, 224, 217),
                ColorHelpers.GetRGB24toRGB565(255, 255, 255),
                ColorHelpers.GetRGB24toRGB565(233, 15, 7),
                0);
            canvas.Execute();
        }
    }
}

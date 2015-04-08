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
using Nwazet.Go.Imaging;
using Nwazet.Go.Fonts;
using Nwazet.Go.Display.TouchScreen;
using SecretLabs.NETMF.Hardware.NetduinoGo;
using SeattleMakerFaireSender.Demos;
using Nwazet.Go.Joystick;

namespace SeattleMakerFaireSender {
    public class MainMenu {

        public VerdanaBold14 fontInfo = new VerdanaBold14();
        public ButtonWidget TemperatureAndRelayButton;
        public ButtonWidget ImageGalleryButton;
        public ButtonWidget BasicUIButton;
        public ButtonWidget LEDMarqueeButton;
        public ButtonWidget LEDRGBArtButton;
        public ButtonWidget LEDCaptureTheDotButton;
        public ButtonWidget LEDSignShowcaseButton;

        public Joystick Joystick = new Joystick();
        
        public MainMenu(VirtualCanvas canvas) {
            TemperatureAndRelayButton = new ButtonWidget(10, 22, 250, 27, fontInfo.GetFontInfo(), "Temperature & Relay");
            ImageGalleryButton = new ButtonWidget(10, 53, 250, 27, fontInfo.GetFontInfo(), "Image Gallery");            
            BasicUIButton = new ButtonWidget(10, 83, 250, 27, fontInfo.GetFontInfo(), "Basic UI");
            LEDMarqueeButton = new ButtonWidget(10, 113, 250, 27, fontInfo.GetFontInfo(), "LED Marquee");
            LEDRGBArtButton = new ButtonWidget(10, 143, 250, 27, fontInfo.GetFontInfo(), "LED RGB Art");
            LEDCaptureTheDotButton = new ButtonWidget(10, 173, 250, 27, fontInfo.GetFontInfo(), "LED Capture The Dot");
            LEDSignShowcaseButton = new ButtonWidget(10, 203, 250, 27, fontInfo.GetFontInfo(), "LED Sign Showcase");

            canvas.RegisterWidget(TemperatureAndRelayButton);
            canvas.RegisterWidget(ImageGalleryButton);
            canvas.RegisterWidget(BasicUIButton);
            canvas.RegisterWidget(LEDMarqueeButton);
            canvas.RegisterWidget(LEDRGBArtButton);
            canvas.RegisterWidget(LEDCaptureTheDotButton);
            canvas.RegisterWidget(LEDSignShowcaseButton);
        }
        public void OnWidgetClicked(VirtualCanvas canvas, Widget widget, TouchEvent touchEvent) {
        }
        public void OnTouchEvent(VirtualCanvas canvas, TouchEvent touchEvent) {
        }
        public void GoBusIRQHandler(uint data1, uint data2, DateTime time) {
        }
        public void Render(VirtualCanvas canvas, GoBus.GoSocket sdSocket) {
            Joystick.Initialize(GoSockets.Socket3, GoBusIRQHandler);
            Joystick.Get();
            while (true) {
                canvas.SetOrientation(Orientation.Landscape);
                canvas.DrawFill(ColorHelpers.GetRGB24toRGB565(255, 212, 42));
                canvas.DrawString(
                    10, 1,
                    ColorHelpers.GetRGB24toRGB565(0, 0, 0),
                    VerdanaBold14.ID,
                    "[Nwazet Playground");
                canvas.RenderWidgets(Nwazet.Go.Imaging.Render.All);

                canvas.WidgetClicked += new WidgetClickedHandler(OnWidgetClicked);
                canvas.TouchscreenWaitForEvent();
                canvas.WidgetClicked -= new WidgetClickedHandler(OnWidgetClicked);

                canvas.RenderWidgets(Nwazet.Go.Imaging.Render.Dirty);

                if (TemperatureAndRelayButton.Clicked) {
                    var tempRelay = new TempRelay();
                    tempRelay.Render(canvas);
                } else if (ImageGalleryButton.Clicked) {
                    var imageGallery = new ImageGallery();
                    imageGallery.Render(canvas, sdSocket);
                } else if (BasicUIButton.Clicked) {
                    var basicUI = new BasicUI();
                    basicUI.Render(canvas);
                } else if (LEDMarqueeButton.Clicked) {
                    var ledMarquee = new LEDMarquee();
                    ledMarquee.Render(canvas);
                } else if (LEDRGBArtButton.Clicked) {
                    var ledRgbArt = new LEDRGBArt();
                    ledRgbArt.Render(canvas);
                } else if (LEDCaptureTheDotButton.Clicked) {
                    var ledCaptureTheDot = new LEDCaptureTheDot();
                    ledCaptureTheDot.Render(canvas, Joystick);
                } else if (LEDSignShowcaseButton.Clicked) {
                    var ledSignShowcase = new LEDSignShowcase();
                    ledSignShowcase.Render();
                }
                canvas.ActivateWidgets(true);
            }
        }
    }
}

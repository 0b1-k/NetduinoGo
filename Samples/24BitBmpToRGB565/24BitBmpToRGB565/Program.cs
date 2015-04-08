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
using System.Drawing;
using System.IO;
using System.Text;

namespace _24BitBmpToRGB565 {
    class Program {
        static void Main(string[] args) {
            var list = Directory.GetFiles(Directory.GetCurrentDirectory(), "*.bmp");
            foreach (var file in list) {
                ProcessBitmap(file);
            }
        }
        public static void ProcessBitmap(string path) {
            using (var bmp = new Bitmap(path)) {
                if (bmp.PixelFormat != System.Drawing.Imaging.PixelFormat.Format24bppRgb) {
                    Console.WriteLine("Please provide a 24-bit depth bitmap to convert.");
                    return;
                }

                var periodPosition = path.LastIndexOf('.');
                var filename = path.Substring(0, periodPosition);

                FileStream bin = new FileStream((string)(filename + ".bin"), FileMode.Create, FileAccess.Write);

                for (int row = 0; row < bmp.Height; row++) {
                    for (int column = 0; column < bmp.Width; column++) {
                        var pixel = bmp.GetPixel(column, row);
                        
                        // Convert from 888 to 565 format
                        ushort pixelOut = (byte) (pixel.R >> 3);
                        pixelOut <<= 6;
                        pixelOut |= (byte) (pixel.G >> 2);
                        pixelOut <<= 5;
                        pixelOut |= (byte) (pixel.B >> 3);

                        // Write out the pixel in Little Endian format
                        bin.WriteByte((byte) pixelOut);
                        bin.WriteByte((byte)(pixelOut >> 8));

                        Console.Write("0x{0:x},", (byte)pixelOut);
                        Console.Write("0x{0:x},", (byte)(pixelOut >> 8));
                    }
                    Console.Write("\r\n");
                }
                bin.Close();
            }
        }
    }
}

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
using System.IO;
using System.Text;
using Microsoft.SPOT.Hardware;

namespace Nwazet.Go.Helpers {
    public class BasicTypeDeSerializerContext : IDisposable {
        public int ContentSize {
            get {
                return _contentSize;
            }
        }
        public bool MoreData {
            get {
                return (_currentIndex < _contentSize);
            }
        }
        public const int BufferStartOffsetDefault = 2;

        private int _bufferStartOffset;

        public BasicTypeDeSerializerContext() {
            _bufferStartOffset = BufferStartOffsetDefault;
        }
        public BasicTypeDeSerializerContext(byte[] buffer, int BufferStartOffset = BufferStartOffsetDefault) {
            Bind(buffer, BufferStartOffset);
        }
        public BasicTypeDeSerializerContext(FileStream file, int BufferStartOffset = BufferStartOffsetDefault) {
            Bind(file, BufferStartOffset);
        }
        public void Bind(byte[] buffer, int BufferStartOffset) {
            if (buffer == null) throw new ArgumentNullException("buffer");
            _buffer = buffer;
            _readFunction = ReadFromBuffer;
            ReadHeader(BufferStartOffset);
        }
        public void Bind(FileStream file, int BufferStartOffset) {
            if (file == null) throw new ArgumentNullException("file");
            _file = file;
            _readFunction = ReadFromFile;
            _file.Seek(_bufferStartOffset, SeekOrigin.Begin); 
            ReadHeader(BufferStartOffset);
        }
        private void ReadHeader(int BufferStartOffset) {
            _bufferStartOffset = BufferStartOffset;
            _currentIndex = _bufferStartOffset;
            _headerVersion = Retrieve();
            if (_headerVersion != 3) throw new ApplicationException("_headerVersion");
            if (_buffer != null) {
                _contentSize = (int)(Retrieve() << 8);
                _contentSize |= (int)Retrieve();
            } else {
                // Skip the 'content size' part of the header when dealing with a file stream
                Retrieve();
                Retrieve();
                _contentSize = (int)_file.Length;
            }
            if (_contentSize == 0) throw new ApplicationException("_contentSize");
        }
        public byte Retrieve() {
            return _readFunction();
        }
        private byte ReadFromBuffer() {
            return _buffer[_currentIndex++];
        }
        private byte ReadFromFile() {
            _currentIndex++;
            return (byte)_file.ReadByte();
        }
        public bool IsLittleEndian {
            get {
                return _isLittleEndian;
            }
        }
        public void Wipe() {
            Array.Clear(_buffer, 0, _buffer.Length);
        }
        public void CopyBytesFromInternalBuffer(byte[] bytes, int offset, int length) {
            if (length == 0) return;
            Array.Copy(_buffer, _currentIndex, bytes, offset, length);
            _currentIndex += length;
        }
        public void Dispose() {
            _encoding = null;
            _file = null;
            _buffer = null;
            _readFunction = null;
        }
        private delegate byte ReadByte();
        private UTF8Encoding _encoding = new UTF8Encoding();
        private FileStream _file;
        private byte[] _buffer;
        private int _currentIndex;
        private int _contentSize;
        private byte _headerVersion;
        private bool _isLittleEndian = Utility.ExtractValueFromArray(new byte[] { 0xBE, 0xEF }, 0, 2) == 0xEFBE;
        private ReadByte _readFunction;
    }

    public static class BasicTypeDeSerializer {
        public static UInt16 Get(BasicTypeDeSerializerContext context, UInt16 data) {
            data = Get(context);
            data <<= 8;
            data |= Get(context);
            return data;
        }
        public static Int16 Get(BasicTypeDeSerializerContext context, Int16 data) {
            UInt16 temp;
            temp = Get(context);
            temp <<= 8;
            temp |= Get(context);
            return (Int16)temp;
        }
        public static UInt32 Get(BasicTypeDeSerializerContext context, UInt32 data) {
            data = Get(context);
            data <<= 8;
            data |= Get(context);
            data <<= 8;
            data |= Get(context);
            data <<= 8;
            data |= Get(context);
            return data;
        }
        public static unsafe float Get(BasicTypeDeSerializerContext context, float data) {
            var temp = new byte[4];
            if (context.IsLittleEndian) {
                // Reverse the buffer going from Big Endian (network byte order) to Little Endian
                temp[3] = context.Retrieve();
                temp[2] = context.Retrieve();
                temp[1] = context.Retrieve();
                temp[0] = context.Retrieve();
            } else { // Already in Big Endian format
                temp[0] = context.Retrieve();
                temp[1] = context.Retrieve();
                temp[2] = context.Retrieve();
                temp[3] = context.Retrieve();
            }
            UInt32 value = Utility.ExtractValueFromArray(temp, 0, 4);
            return *((float*)&value);
        }
        public static Int32 Get(BasicTypeDeSerializerContext context, Int32 data) {
            data = Get(context);
            data <<= 8;
            data |= Get(context);
            data <<= 8;
            data |= Get(context);
            data <<= 8;
            data |= Get(context);
            return data;
        }
        public static UInt64 Get(BasicTypeDeSerializerContext context, UInt64 data) {
            data = Get(context);
            data <<= 8;
            data |= Get(context);
            data <<= 8;
            data |= Get(context);
            data <<= 8;
            data |= Get(context);
            data <<= 8;
            data |= Get(context);
            data <<= 8;
            data |= Get(context);
            data <<= 8;
            data |= Get(context);
            data <<= 8;
            data |= Get(context);
            return data;
        }
        public static Int64 Get(BasicTypeDeSerializerContext context, Int64 data) {
            return (Int64)Get(context, (UInt64)data);
        }
        public static string Get(BasicTypeDeSerializerContext context, string text) {
            byte IsASCII = 0;
            IsASCII = Get(context);
            ushort length = 0;
            length = Get(context, length);
            if (length != 0) {
                if (IsASCII == 1) {
                    var bytes = new byte[length];
                    var index = 0;
                    while (length-- != 0) {
                        bytes[index++] = Get(context);
                    }
                    Get(context); // Skip null byte terminator
                    return new string(Encoding.UTF8.GetChars(bytes));
                } else {
                    var unicodeChars = new char[length];
                    var index = 0;
                    ushort unicodeChar = 0;
                    while (length-- != 0) {
                        unicodeChars[index++] = (char)Get(context, unicodeChar);
                    }
                    Get(context, unicodeChar); // Skip null character terminator
                    return new string(unicodeChars);
                }
            }
            Get(context); // Skip null character terminator
            return "";
        }
        public static byte[] Get(BasicTypeDeSerializerContext context, byte[] bytes) {
            ushort length = 0;
            length = Get(context, length);
            if (length != 0) {
                var buffer = new byte[length];
                var index = 0;
                while (length-- != 0) {
                    buffer[index++] = Get(context);
                }
                return buffer;
            }
            return null;
        }
        public static ushort[] Get(BasicTypeDeSerializerContext context, ushort[] array) {
            ushort length = 0;
            length = Get(context, length);
            if (length != 0) {
                var buffer = new ushort[length];
                var index = 0;
                UInt16 data = 0;
                while (length-- != 0) {
                    buffer[index++] = Get(context, data);
                }
                return buffer;
            }
            return null;
        }
        public static UInt32[] Get(BasicTypeDeSerializerContext context, UInt32[] array) {
            ushort length = 0;
            length = Get(context, length);
            if (length != 0) {
                var buffer = new UInt32[length];
                var index = 0;
                UInt32 data = 0;
                while (length-- != 0) {
                    buffer[index++] = Get(context, data);
                }
                return buffer;
            }
            return null;
        }
        public static UInt64[] Get(BasicTypeDeSerializerContext context, UInt64[] array) {
            ushort length = 0;
            length = Get(context, length);
            if (length != 0) {
                var buffer = new UInt64[length];
                var index = 0;
                UInt64 data = 0;
                while (length-- != 0) {
                    buffer[index++] = Get(context, data);
                }
                return buffer;
            }
            return null;
        }
        public static byte Get(BasicTypeDeSerializerContext context) {
            return context.Retrieve();
        }
    }
}

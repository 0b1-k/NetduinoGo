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
#include "vt100.h"
#include <stdio.h>

void vt100TextColor(int foregroundColor, int backgroundColor){
  // http://www.termsys.demon.co.uk/vtansi.htm
  printf("%c[%i;%im", 0x1B, foregroundColor+30, backgroundColor+40);
}
void vt100TextDefault(){
  printf("%c[%i;%im", 0x1B, VT100_White+30, VT100_Black+40);
}

void PrintNeutralMessage(const char* msg){
  vt100TextDefault();
  printf("%s\r\n",msg);
}
void PrintErrorMessage(const char* msg){
  vt100TextColor(VT100_Red, VT100_Black);
  printf("%s\r\n",msg);
  vt100TextDefault();
}
void PrintSuccessMessage(const char* msg){
  vt100TextColor(VT100_Green, VT100_Black);
  printf("%s\r\n",msg);
  vt100TextDefault();
}
void PrintAnomalyMessage(const char* msg){
  vt100TextColor(VT100_Cyan, VT100_Black);
  printf("%s\r\n",msg);
  vt100TextDefault();
}
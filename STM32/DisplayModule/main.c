#include "main.h"
#include "systick.h"
#include "gpio.h"
#include "spi.h"
#include "ILI9341.h"
#include "Nwazet.h"
#include "VirtualCanvas.h"
#include "touchscreen.h"
#include "drawing.h"
#include "verdanabold14.h"
#include "nwazetLogo.h"

void ProcessMessage(unsigned char* message, unsigned short messageBufferSize, unsigned char* responseBuffer, unsigned short responseBufferSize){
  SerializerContext receiverContext;
  unsigned short payloadLength = InitializeReceiveBuffer(&receiverContext, message, messageBufferSize);
  VirtualCanvasReplay(&receiverContext, responseBuffer, responseBufferSize);
}

void printString(char* string, unsigned short y){
  drawString(
    (240 - drawGetStringWidth(&verdanabold14ptFontInfo, string)) / 2,
    y,
    0,
    &verdanabold14ptFontInfo,
    string);
}

#define nwazetYellowRed   0xFF
#define nwazetYellowGreen 0xD4
#define nwazetYellowBlue  0x2A

void welcomeScreen(){
  drawFill(drawRGB24toRGB565(nwazetYellowRed, nwazetYellowGreen, nwazetYellowBlue));
  lcdSetWindow(
    49, 49 + (142 - 1),
    82, 82 + (156 - 1));
  unsigned short imageSize = 44304;
  unsigned short* imagePixel = (unsigned short*) &nwazetLogo[0];
  while(imageSize){
    lcdWriteData(*imagePixel);
    imagePixel++;
    imageSize -= 2;
  }
  unsigned short y = 10;
  unsigned short spaceBetweenLines = 2;
  printString("Touch Display", y);
  y+=verdanabold14ptFontInfo.height+spaceBetweenLines;
  printString("By [nwazet", y);
  y = 295;
  printString("Raspberry Pi v0.1", y);
}

#ifdef DEBUG
void ledInit(){
  GPIO_InitTypeDef  GPIO_InitStructure;
  GPIO_StructInit(&GPIO_InitStructure);
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_Init(GPIOC, &GPIO_InitStructure);
}
#endif

int main()
{
  while(true){
    Reset();
    systickInit();
    gpioInit();
    adcInit();
    lcdInit();
    tsInit();
    welcomeScreen();
    #ifdef DEBUG
      ledInit();
    #endif
    spiInit();
    while(!IsRebootRequired()){
      if(IsSynchronizationRequired()){
        AssertGoBusIRQ();
      }
      #ifdef DEBUG
        GPIO_ToggleBits(GPIOC, GPIO_Pin_13);
        delay(5);
      #endif
    }
  }
}

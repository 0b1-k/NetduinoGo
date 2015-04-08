#include "VirtualCanvas.h"
#include "drawing.h"
#include "touchscreen.h"
#include "alphanumeric.h"
#include "dejavusans9.h"
#include "dejavusansbold9.h"
#include "dejavusanscondensed9.h"
#include "dejavusansmono8.h"
#include "dejavusansmonobold8.h"
#include "verdana14.h"
#include "verdana9.h"
#include "verdanabold14.h"
#include "ILI9341.h"
#include "Nwazet.h"
#include "spi.h"
#include "string.h"

bool touchscreenCalibrated = false;

SerializerContext touchscreenContext;

#define DejaVuSans9ID           30367
#define DejaVuSansBold9ID       49983
#define DejaVuSansCondensed9ID  41259
#define DejaVuSansMono8ID       1149
#define DejaVuSansMonoBold8ID   6147
#define Verdana14ID             37581
#define Verdana9ID              56903
#define VerdanaBold14ID         11480

const FONT_INFO* FontInfoLookUp(unsigned short fontInfoID) {
  switch(fontInfoID){
  case DejaVuSans9ID:
    return &dejaVuSans9ptFontInfo;
    break;
  case DejaVuSansBold9ID:
    return &dejaVuSansBold9ptFontInfo;
    break;
  case DejaVuSansCondensed9ID:
    return &dejaVuSansCondensed9ptFontInfo;
    break;
  case DejaVuSansMono8ID:
    return &dejaVuSansMono8ptFontInfo;
    break;
  case DejaVuSansMonoBold8ID:
    return &dejaVuSansMonoBold8ptFontInfo;
    break;
  case Verdana14ID:
    return &verdana14ptFontInfo;
    break;
  case Verdana9ID:
    return &verdana9ptFontInfo;
    break;
  case VerdanaBold14ID:
  default:
    return &verdanabold14ptFontInfo;
    break;
  }
}
void GetDrawPixel(SerializerContext* context){
  unsigned short x = 0;
  unsigned short y = 0;
  unsigned short color = 0;
  Get(context, (void*)&x, sizeof(x));
  Get(context, (void*)&y, sizeof(y));
  Get(context, (void*)&color, sizeof(color));
  drawPixel(x, y, color);
}
void GetDrawFill(SerializerContext* context){
  unsigned short color = 0;
  Get(context, (void*)&color, sizeof(color));
  drawFill(color);
}
void GetDrawLine(SerializerContext* context){
  unsigned short x0 = 0;
  unsigned short y0 = 0;
  unsigned short x1 = 0;
  unsigned short y1 = 0;
  unsigned short color = 0;
  Get(context, (void*)&x0, sizeof(x0));
  Get(context, (void*)&y0, sizeof(y0));
  Get(context, (void*)&x1, sizeof(x1));
  Get(context, (void*)&y1, sizeof(y1));
  Get(context, (void*)&color, sizeof(color));
  drawLine(x0, y0, x1, y1, color);
}
void GetDrawLineDotted(SerializerContext* context){
  unsigned short x0 = 0;
  unsigned short y0 = 0;
  unsigned short x1 = 0;
  unsigned short y1 = 0;
  unsigned short empty = 0;
  unsigned short solid = 0;
  unsigned short color = 0;
  Get(context, (void*)&x0, sizeof(x0));
  Get(context, (void*)&y0, sizeof(y0));
  Get(context, (void*)&x1, sizeof(x1));
  Get(context, (void*)&y1, sizeof(y1));
  Get(context, (void*)&color, sizeof(color));
  Get(context, (void*)&empty, sizeof(empty));
  Get(context, (void*)&solid, sizeof(solid));
  drawLineDotted(x0, y0, x1, y1, empty, solid, color);
}
void GetDrawCircle(SerializerContext* context){
  unsigned short xCenter = 0;
  unsigned short yCenter = 0;
  unsigned short radius = 0;
  unsigned short color = 0;
  Get(context, (void*)&xCenter, sizeof(xCenter));
  Get(context, (void*)&yCenter, sizeof(yCenter));
  Get(context, (void*)&radius, sizeof(radius));
  Get(context, (void*)&color, sizeof(color));
  drawCircle(xCenter, yCenter, radius, color);
}
void GetDrawCircleFilled(SerializerContext* context){
  unsigned short xCenter = 0;
  unsigned short yCenter = 0;
  unsigned short radius = 0;
  unsigned short color = 0;
  Get(context, (void*)&xCenter, sizeof(xCenter));
  Get(context, (void*)&yCenter, sizeof(yCenter));
  Get(context, (void*)&radius, sizeof(radius));
  Get(context, (void*)&color, sizeof(color));
  drawCircleFilled(xCenter, yCenter, radius, color);
}
void GetDrawCornerFilled(SerializerContext* context){
  unsigned short xCenter = 0;
  unsigned short yCenter = 0;
  unsigned short radius = 0;
  unsigned short position = 0;
  unsigned short color = 0;
  Get(context, (void*)&xCenter, sizeof(xCenter));
  Get(context, (void*)&yCenter, sizeof(yCenter));
  Get(context, (void*)&radius, sizeof(radius));
  Get(context, (void*)&position, sizeof(position));
  Get(context, (void*)&color, sizeof(color));
  drawCornerFilled(xCenter, yCenter, radius, (drawCornerPosition_t) position, color);
}
void GetDrawArrow(SerializerContext* context){
  unsigned short x = 0;
  unsigned short y = 0;
  unsigned short size = 0;
  unsigned short direction = 0;
  unsigned short color = 0;
  Get(context, (void*)&x, sizeof(x));
  Get(context, (void*)&y, sizeof(y));
  Get(context, (void*)&size, sizeof(size));
  Get(context, (void*)&direction, sizeof(direction));
  Get(context, (void*)&color, sizeof(color));
  drawArrow(x, y, size, (drawDirection_t) direction, color);
}
void GetDrawRectangle(SerializerContext* context){
  unsigned short x0 = 0;
  unsigned short y0 = 0;
  unsigned short x1 = 0;
  unsigned short y1 = 0;
  unsigned short color = 0;
  Get(context, (void*)&x0, sizeof(x0));
  Get(context, (void*)&y0, sizeof(y0));
  Get(context, (void*)&x1, sizeof(x1));
  Get(context, (void*)&y1, sizeof(y1));
  Get(context, (void*)&color, sizeof(color));
  drawRectangle(x0, y0, x1, y1, color);
}
void GetDrawRectangleFilled(SerializerContext* context){
  unsigned short x0 = 0;
  unsigned short y0 = 0;
  unsigned short x1 = 0;
  unsigned short y1 = 0;
  unsigned short color = 0;
  Get(context, (void*)&x0, sizeof(x0));
  Get(context, (void*)&y0, sizeof(y0));
  Get(context, (void*)&x1, sizeof(x1));
  Get(context, (void*)&y1, sizeof(y1));
  Get(context, (void*)&color, sizeof(color));
  drawRectangleFilled(x0, y0, x1, y1, color);
}
void GetDrawRectangleRounded(SerializerContext* context){
  unsigned short x0 = 0;
  unsigned short y0 = 0;
  unsigned short x1 = 0;
  unsigned short y1 = 0;
  unsigned short color = 0;
  unsigned short radius = 0;
  unsigned short corners = 0;
  Get(context, (void*)&x0, sizeof(x0));
  Get(context, (void*)&y0, sizeof(y0));
  Get(context, (void*)&x1, sizeof(x1));
  Get(context, (void*)&y1, sizeof(y1));
  Get(context, (void*)&color, sizeof(color));
  Get(context, (void*)&radius, sizeof(radius));
  Get(context, (void*)&corners, sizeof(corners));
  drawRectangleRounded(x0, y0, x1, y1, color, radius, (drawRoundedCorners_t) corners);
}
void GetDrawTriangle(SerializerContext* context){
  unsigned short x0 = 0;
  unsigned short y0 = 0;
  unsigned short x1 = 0;
  unsigned short y1 = 0;
  unsigned short x2 = 0;
  unsigned short y2 = 0;
  unsigned short color = 0;
  Get(context, (void*)&x0, sizeof(x0));
  Get(context, (void*)&y0, sizeof(y0));
  Get(context, (void*)&x1, sizeof(x1));
  Get(context, (void*)&y1, sizeof(y1));
  Get(context, (void*)&x2, sizeof(x2));
  Get(context, (void*)&y2, sizeof(y2));
  Get(context, (void*)&color, sizeof(color));
  drawTriangle(x0, y0, x1, y1, x2, y2, color);
}
void GetDrawTriangleFilled(SerializerContext* context){
  unsigned short x0 = 0;
  unsigned short y0 = 0;
  unsigned short x1 = 0;
  unsigned short y1 = 0;
  unsigned short x2 = 0;
  unsigned short y2 = 0;
  unsigned short color = 0;
  Get(context, (void*)&x0, sizeof(x0));
  Get(context, (void*)&y0, sizeof(y0));
  Get(context, (void*)&x1, sizeof(x1));
  Get(context, (void*)&y1, sizeof(y1));
  Get(context, (void*)&x2, sizeof(x2));
  Get(context, (void*)&y2, sizeof(y2));
  Get(context, (void*)&color, sizeof(color));
  drawTriangleFilled(x0, y0, x1, y1, x2, y2, color);
}
void GetDrawProgressBar(SerializerContext* context){
  unsigned short x = 0;
  unsigned short y = 0;
  unsigned short width = 0;
  unsigned short height = 0;
  unsigned short borderCorners = 0;
  unsigned short progressCorners = 0;
  unsigned short borderColor = 0;
  unsigned short borderFillColor = 0;
  unsigned short progressBorderColor = 0;
  unsigned short progressFillColor = 0;
  unsigned short progress = 0;
  Get(context, (void*)&x, sizeof(x));
  Get(context, (void*)&y, sizeof(y));
  Get(context, (void*)&width, sizeof(width));
  Get(context, (void*)&height, sizeof(height));
  Get(context, (void*)&borderCorners, sizeof(borderCorners));
  Get(context, (void*)&progressCorners, sizeof(progressCorners));
  Get(context, (void*)&borderColor, sizeof(borderColor));
  Get(context, (void*)&borderFillColor, sizeof(borderFillColor));
  Get(context, (void*)&progressBorderColor, sizeof(progressBorderColor));
  Get(context, (void*)&progressFillColor, sizeof(progressFillColor));
  Get(context, (void*)&progress, sizeof(progress));
  drawProgressBar(
      x, y,
      width, height,
      (drawRoundedCorners_t) borderCorners, (drawRoundedCorners_t) progressCorners,
      borderColor, borderFillColor,
      progressBorderColor, progressFillColor,
      progress);
}
void GetDrawButton(SerializerContext* context){
  unsigned short x=0;
  unsigned short y=0;
  unsigned short width=0;
  unsigned short height=0;
  unsigned short fontInfoID=0;
  unsigned short fontHeight=0;
  unsigned short borderColor=0;
  unsigned short fillColor=0;
  unsigned short fontColor=0;
  unsigned short cornerStyle=0;

  Get(context, (void*)&x, sizeof(x));
  Get(context, (void*)&y, sizeof(y));
  Get(context, (void*)&width, sizeof(width));
  Get(context, (void*)&height, sizeof(height));
  Get(context, (void*)&fontInfoID, sizeof(fontInfoID));
  Get(context, (void*)&fontHeight, sizeof(fontHeight));
  Get(context, (void*)&borderColor, sizeof(borderColor));
  Get(context, (void*)&fillColor, sizeof(fillColor));
  Get(context, (void*)&fontColor, sizeof(fontColor));

  unsigned short textLength = 0;
  unsigned char isTextASCII = 0;
  void* text = GetStringReference(context, &textLength, &isTextASCII);

  Get(context, (void*)&cornerStyle, sizeof(cornerStyle));

  drawButton(
      x, y,
      width, height,
      FontInfoLookUp(fontInfoID), fontHeight,
      borderColor, fillColor, fontColor,
      (char*)text);
}
void GetDrawIcon16(SerializerContext* context){
  unsigned short x = 0;
  unsigned short y = 0;
  unsigned short color = 0;

  Get(context, (void*)&x, sizeof(x));
  Get(context, (void*)&y, sizeof(y));
  Get(context, (void*)&color, sizeof(color));

  unsigned short arrayLength = 0;
  void* icon = GetArrayReference(context, &arrayLength, sizeof(unsigned short));

  drawIcon16(x, y, color, (unsigned short*) icon);
}
void GetDrawString(SerializerContext* context){
  unsigned short x = 0;
  unsigned short y = 0;
  unsigned short color = 0;
  unsigned short fontInfoID = 0;

  Get(context, (void*)&x, sizeof(x));
  Get(context, (void*)&y, sizeof(y));
  Get(context, (void*)&color, sizeof(color));
  Get(context, (void*)&fontInfoID, sizeof(fontInfoID));

  unsigned short textLength = 0;
  unsigned char isTextASCII = 0;
  void* text = GetStringReference(context, &textLength, &isTextASCII);

  drawString(x, y, color, FontInfoLookUp(fontInfoID), (char*)text);
}

typedef struct {
  unsigned short x;
  unsigned short y;
  unsigned short width;
  unsigned short height;
  unsigned short bytesPerPixel;
} DrawImageSetup;

DrawImageSetup imageSetup;

void GetDrawBitmapImageInitialize(SerializerContext* context){
  Get(context, (void*)&imageSetup.x, sizeof(imageSetup.x));
  Get(context, (void*)&imageSetup.y, sizeof(imageSetup.y));
  Get(context, (void*)&imageSetup.width, sizeof(imageSetup.width));
  Get(context, (void*)&imageSetup.height, sizeof(imageSetup.height));
  Get(context, (void*)&imageSetup.bytesPerPixel, sizeof(imageSetup.bytesPerPixel));
  lcdSetWindow(
    imageSetup.x, imageSetup.x + (imageSetup.width - 1),
    imageSetup.y, imageSetup.y + (imageSetup.height - 1));
}
void GetDrawBitmapImageData(SerializerContext* context){
  unsigned short imageChunkSize = 0;
  Get(context, (void*)&imageChunkSize, sizeof(imageChunkSize));
  if(imageSetup.bytesPerPixel == 2){ // raw 16-bit / pixel bitmap display
    unsigned short* imagePixel = (unsigned short*) &context->Buffer[context->CurrentIndex];
    context->CurrentIndex += imageChunkSize;
    while(imageChunkSize){
      lcdWriteData(*imagePixel);
      imagePixel++;
      imageChunkSize -= 2;
    }
  } else { // 24-bit -> 16-bit .bmp display with big endian conversion
    unsigned short rowLengthInBytes = (imageSetup.width * 3);
    unsigned char* imageBuffer = &context->Buffer[context->CurrentIndex];
    unsigned char* imageCurrentRow = (imageBuffer + imageChunkSize) - rowLengthInBytes;
    context->CurrentIndex += imageChunkSize;
    while(imageCurrentRow >= imageBuffer){
      unsigned char* imagePixel = imageCurrentRow;
      unsigned short byteCount = rowLengthInBytes;
      while(byteCount){
        lcdWriteData(drawRGB24toRGB565(*(imagePixel+2), *(imagePixel+1), *imagePixel));
        imagePixel += 3;
        byteCount -= 3;
      }
      imageCurrentRow -= rowLengthInBytes;
    }
  }
}
void GetSetOrientation(SerializerContext* context){
  unsigned short orientation = 0;
  Get(context, (void*)&orientation, sizeof(orientation));
  lcdSetOrientation((lcdOrientation_t) orientation);
}

bool volatile synchronicity = false;

void GetSynchronicity(SerializerContext* context){
  synchronicity = false;
  Get(context, (void*)&synchronicity, sizeof(synchronicity));
}
bool IsSynchronizationRequired(){
  return synchronicity;
}
void CalibrateTouchScreen(){
  tsCalibrate();
  touchscreenCalibrated = true;
}
void GetTouchscreenCalibration(SerializerContext* context){
  CalibrateTouchScreen();
}

enum {
  Alphanumeric
} DialogType;

enum {
  String,
  TouchEvent,
  CalibrationMatrix
} TouchScreenDataType;

void GetTouchscreenShowDialog(SerializerContext* context, unsigned char* responseBuffer, unsigned short responseBufferSize){
  unsigned short dialogType;
  unsigned char* textInput;
  unsigned char dataType;
  bool isASCII;
  Get(context, (void*)&dialogType, sizeof(dialogType));
  InitializeSendBuffer(&touchscreenContext, responseBuffer, responseBufferSize);
  switch(dialogType){
  case Alphanumeric:
    dataType = String;
    isASCII = true;
    if(!touchscreenCalibrated) {
      CalibrateTouchScreen();
    }
    textInput = (unsigned char*) alphaShowDialogue();
    Put(&touchscreenContext, (void*) &dataType, sizeof(dataType), 1);
    PutStringReference(&touchscreenContext, textInput, strlen((const char*)textInput), isASCII);
    break;
  default:
    break;
  }
  FinalizeSendBuffer(&touchscreenContext);
}

enum {
  Blocking,
  NonBlocking
} TouchScreenEventMode;

void GetTouchscreenWaitForEvent(SerializerContext* context, unsigned char* responseBuffer, unsigned short responseBufferSize){
  tsTouchData_t touchInput;
  unsigned char dataType = TouchEvent;
  unsigned char touchScreenEventMode;
  Get(context, (void*)&touchScreenEventMode, sizeof(touchScreenEventMode));
  if(!touchscreenCalibrated) {
    CalibrateTouchScreen();
  }
  tsWaitForEvent(&touchInput, (uint32_t)touchScreenEventMode);
  InitializeSendBuffer(&touchscreenContext, responseBuffer, responseBufferSize);
  Put(&touchscreenContext, (void*)&dataType, sizeof(unsigned char), 1);
  Put(&touchscreenContext, (void*)&touchInput.xlcd, sizeof(uint16_t), 1);
  Put(&touchscreenContext, (void*)&touchInput.ylcd, sizeof(uint16_t), 1);
  Put(&touchscreenContext, (void*)&touchInput.z1, sizeof(uint32_t), 1);
  Put(&touchscreenContext, (void*)&touchInput.valid, sizeof(bool), 1);
  FinalizeSendBuffer(&touchscreenContext);
}
void GetCalibrationMatrix(SerializerContext* context, unsigned char* responseBuffer, unsigned short responseBufferSize){
  if(!touchscreenCalibrated) {
    CalibrateTouchScreen();
  }
  unsigned char dataType = CalibrationMatrix;
  tsMatrix_t* matrix = tsGetCalibrationMatrix();
  InitializeSendBuffer(&touchscreenContext, responseBuffer, responseBufferSize);
  Put(&touchscreenContext, (void*)&dataType, sizeof(unsigned char), 1);
  Put(&touchscreenContext, (void*)&matrix->An, sizeof(int32_t), 1);
  Put(&touchscreenContext, (void*)&matrix->Bn, sizeof(int32_t), 1);
  Put(&touchscreenContext, (void*)&matrix->Cn, sizeof(int32_t), 1);
  Put(&touchscreenContext, (void*)&matrix->Dn, sizeof(int32_t), 1);
  Put(&touchscreenContext, (void*)&matrix->En, sizeof(int32_t), 1);
  Put(&touchscreenContext, (void*)&matrix->Fn, sizeof(int32_t), 1);
  Put(&touchscreenContext, (void*)&matrix->Divider, sizeof(int32_t), 1);
  FinalizeSendBuffer(&touchscreenContext);
}
void SetCalibrationMatrix(SerializerContext* context){
  tsMatrix_t* matrix = tsGetCalibrationMatrix();
  Get(context, (void*)&matrix->An, sizeof(matrix->An));
  Get(context, (void*)&matrix->Bn, sizeof(matrix->Bn));
  Get(context, (void*)&matrix->Cn, sizeof(matrix->Cn));
  Get(context, (void*)&matrix->Dn, sizeof(matrix->Dn));
  Get(context, (void*)&matrix->En, sizeof(matrix->En));
  Get(context, (void*)&matrix->Fn, sizeof(matrix->Fn));
  Get(context, (void*)&matrix->Divider, sizeof(matrix->Divider));
  touchscreenCalibrated = true;
}

enum {
    DrawTestPattern = 'A',
    DrawPixel = 'B',
    DrawFill = 'C',
    DrawLine = 'D',
    DrawLineDotted = 'E',
    DrawCircle = 'F',
    DrawCircleFilled = 'G',
    DrawCornerFilled = 'H',
    DrawArrow = 'I',
    DrawRectangle = 'J',
    DrawRectangleFilled = 'K',
    DrawRectangleRounded = 'L',
    DrawTriangle = 'M',
    DrawTriangleFilled = 'N',
    DrawProgressBar = 'O',
    DrawButton = 'P',
    DrawIcon16 = 'Q',
    DrawString = 'R',
    DrawImageInitialize = 'S',
    DrawImageData = 'T',
    SetOrientation = 'U',
    Synchronicity = 'V',
    TouchscreenCalibration = 'W',
    TouchscreenShowDialog = 'X',
    TouchscreenWaitForEvent = 'Y',
    Reboot = 'Z',
    TouchscreenGetCalibrationMatrix = 'a',
    TouchscreenSetCalibrationMatrix = 'b'
};

void OnUnknownCommand(unsigned char command){
  drawFill(drawRGB24toRGB565(0, 0, 255));
  char* message = "Invalid command!\0";
  drawString(
    (240 - drawGetStringWidth(&verdanabold14ptFontInfo, message)) / 2,
    150,
    drawRGB24toRGB565(255, 255, 255),
    &verdanabold14ptFontInfo,
    message);
}

static volatile bool rebootRequired = false;

void Reset(){
  rebootRequired = false;
}
bool IsRebootRequired(){
  return rebootRequired;
}
void SetRebootFlag(){
  rebootRequired = true;
}

void VirtualCanvasReplay(SerializerContext* context, unsigned char* responseBuffer, unsigned short responseBufferSize){
  if(!context) return;
  if(!context->PayloadLength) return;
  if(!context->IsValidPacket) return;

  unsigned char command = 0;
  while(MoreData(context)){
    Get(context, (void*)(&command),sizeof(command));
    switch (command) {
    case DrawTestPattern:
      drawTestPattern();
      break;
    case DrawPixel:
      GetDrawPixel(context);
      break;
    case DrawFill:
      GetDrawFill(context);
      break;
    case DrawLine:
      GetDrawLine(context);
      break;
    case DrawLineDotted:
      GetDrawLineDotted(context);
      break;
    case DrawCircle:
      GetDrawCircle(context);
      break;
    case DrawCircleFilled:
      GetDrawCircleFilled(context);
      break;
    case DrawCornerFilled:
      GetDrawCornerFilled(context);
      break;
    case DrawArrow:
      GetDrawArrow(context);
      break;
    case DrawRectangle:
      GetDrawRectangle(context);
      break;
    case DrawRectangleFilled:
      GetDrawRectangleFilled(context);
      break;
    case DrawRectangleRounded:
      GetDrawRectangleRounded(context);
      break;
    case DrawTriangle:
      GetDrawTriangle(context);
      break;
    case DrawTriangleFilled:
      GetDrawTriangleFilled(context);
      break;
    case DrawProgressBar:
      GetDrawProgressBar(context);
      break;
    case DrawButton:
      GetDrawButton(context);
      break;
    case DrawIcon16:
      GetDrawIcon16(context);
      break;
    case DrawString:
      GetDrawString(context);
      break;
    case DrawImageInitialize:
      GetDrawBitmapImageInitialize(context);
      break;
    case DrawImageData:
      GetDrawBitmapImageData(context);
      break;
    case SetOrientation:
      GetSetOrientation(context);
      break;
    case Synchronicity:
      GetSynchronicity(context);
      break;
    case TouchscreenCalibration:
      GetTouchscreenCalibration(context);
      break;
    case TouchscreenShowDialog:
      GetTouchscreenShowDialog(context, responseBuffer, responseBufferSize);
      break;
    case TouchscreenWaitForEvent:
      GetTouchscreenWaitForEvent(context, responseBuffer, responseBufferSize);
      break;
    case Reboot:
      SetRebootFlag();
      break;
    case TouchscreenGetCalibrationMatrix:
      GetCalibrationMatrix(context, responseBuffer, responseBufferSize);
      break;
    case TouchscreenSetCalibrationMatrix:
      SetCalibrationMatrix(context);
      break;
    default:
      OnUnknownCommand(command);
      return;
      break;
    }
  }
}

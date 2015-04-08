#ifndef __ILI9341_H__
  #define __ILI9341_H__

#include "projectconfig.h"
#include "lcd.h"

void              lcdInit(void);
void              lcdTest(void);
uint16_t          lcdGetPixel(uint16_t x, uint16_t y);
void              lcdFillRGB(uint16_t color);
void              lcdDrawPixel(uint16_t x, uint16_t y, uint16_t color);
void              lcdDrawPixels(uint16_t x, uint16_t y, uint16_t *data, uint32_t len);
void              lcdDrawHLine(uint16_t x0, uint16_t x1, uint16_t y, uint16_t color);
void              lcdDrawVLine(uint16_t x, uint16_t y0, uint16_t y1, uint16_t color);
void              lcdScroll(int16_t pixels, uint16_t fillColor);
uint16_t          lcdGetWidth(void);
uint16_t          lcdGetHeight(void);
void              lcdSetOrientation(lcdOrientation_t orientation);
uint16_t          lcdGetControllerID(void);
lcdOrientation_t  lcdGetOrientation(void);
lcdProperties_t   lcdGetProperties(void);
void              lcdReset(void);
void              lcdWriteCommand(unsigned char command);
void              lcdWriteData(unsigned short data);
void              lcdWrite(unsigned short data);
void              lcdSetCursor(unsigned short x, unsigned short y);
void              lcdSetWindow(unsigned short x0, unsigned short y0, unsigned short x1, unsigned short y1);
void              lcdHome(void);
unsigned char     lcdBuildMemoryAccessControlConfig(
                                bool rowAddressOrder,
                                bool columnAddressOrder,
                                bool rowColumnExchange,
                                bool verticalRefreshOrder,
                                bool colorOrder,
                                bool horizontalRefreshOrder);

void              lcdSetWindow(unsigned short x0, unsigned short y0, unsigned short x1, unsigned short y1);
#endif

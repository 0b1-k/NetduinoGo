#ifndef __LCD_H__
#define __LCD_H__

#include "projectconfig.h"
#include "colors.h"

// Any LCD needs to implement these common methods, which allow the low-level
// initialisation and pixel-setting details to be abstracted away from the
// higher level drawing and graphics code.

typedef enum 
{
  LCD_ORIENTATION_PORTRAIT = 0,
  LCD_ORIENTATION_LANDSCAPE = 1
} lcdOrientation_t;

// This struct is used to indicate the capabilities of different LCDs
typedef struct
{
  uint16_t width;         // LCD width in pixels (default orientation)
  uint16_t height;        // LCD height in pixels (default orientation)
  bool     touchscreen;   // Whether the LCD has a touch screen
  bool     orientation;   // Whether the LCD orientation can be modified
  bool     hwscrolling;   // Whether the LCD support HW scrolling
} lcdProperties_t;

extern void     lcdInit(void);
extern void     lcdTest(void);
extern uint16_t lcdGetPixel(uint16_t x, uint16_t y);
extern void     lcdFillRGB(uint16_t data);
extern void     lcdDrawPixel(uint16_t x, uint16_t y, uint16_t color);
extern void     lcdDrawPixels(uint16_t x, uint16_t y, uint16_t *data, uint32_t len);
extern void     lcdDrawHLine(uint16_t x0, uint16_t x1, uint16_t y, uint16_t color);
extern void     lcdDrawVLine(uint16_t x, uint16_t y0, uint16_t y1, uint16_t color);
extern void     lcdBacklight(bool state);
extern void     lcdScroll(int16_t pixels, uint16_t fillColor);
extern uint16_t lcdGetWidth(void);
extern uint16_t lcdGetHeight(void);
extern void     lcdSetOrientation(lcdOrientation_t orientation);
extern uint16_t lcdGetControllerID(void);
extern lcdOrientation_t lcdGetOrientation(void);
extern lcdProperties_t lcdGetProperties(void);

#endif

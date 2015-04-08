#ifndef __DRAWING_H__
#define __DRAWING_H__

#include "projectconfig.h"
#include "lcd.h"
#include "colors.h"
#include "drivers/lcd/tft/fonts/bitmapfonts.h"

#if defined CFG_TFTLCD_INCLUDESMALLFONTS & CFG_TFTLCD_INCLUDESMALLFONTS == 1
  #include "drivers/lcd/smallfonts.h"
#endif

#ifdef CFG_SDCARD
  #include "bmp.h"
#endif

typedef struct
{
  uint8_t red;
  uint8_t green;
  uint8_t blue;
} drawColorRGB24_t;

typedef enum
{
  DRAW_ROUNDEDCORNERS_NONE    = 0,
  DRAW_ROUNDEDCORNERS_ALL     = 1,
  DRAW_ROUNDEDCORNERS_TOP     = 2,
  DRAW_ROUNDEDCORNERS_BOTTOM  = 3,
  DRAW_ROUNDEDCORNERS_LEFT    = 4,
  DRAW_ROUNDEDCORNERS_RIGHT   = 5
} drawRoundedCorners_t;

typedef enum
{
  DRAW_CORNERPOSITION_TOPLEFT     = 0,
  DRAW_CORNERPOSITION_TOPRIGHT    = 1,
  DRAW_CORNERPOSITION_BOTTOMLEFT  = 2,
  DRAW_CORNERPOSITION_BOTTOMRIGHT = 3
} drawCornerPosition_t;

typedef enum
{
  DRAW_DIRECTION_LEFT,
  DRAW_DIRECTION_RIGHT,
  DRAW_DIRECTION_UP,
  DRAW_DIRECTION_DOWN
} drawDirection_t;

void      drawTestPattern      ( void );
void      drawPixel            ( uint16_t x, uint16_t y, uint16_t color );
void      drawFill             ( uint16_t color );
void      drawLine             ( uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint16_t color );
void      drawLineDotted       ( uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint16_t space, uint16_t solid, uint16_t color );
void      drawCircle           ( uint16_t xCenter, uint16_t yCenter, uint16_t radius, uint16_t color );
void      drawCircleFilled     ( uint16_t xCenter, uint16_t yCenter, uint16_t radius, uint16_t color );
void      drawCornerFilled     (uint16_t xCenter, uint16_t yCenter, uint16_t radius, drawCornerPosition_t position, uint16_t color);
void      drawArrow            ( uint16_t x, uint16_t y, uint16_t size, drawDirection_t, uint16_t color );
void      drawRectangle        ( uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint16_t color );
void      drawRectangleFilled  ( uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint16_t color );
void      drawRectangleRounded ( uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint16_t color, uint16_t radius, drawRoundedCorners_t corners );
void      drawTriangle         ( uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t color);
void      drawTriangleFilled   ( uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t color);
void      drawString           ( uint16_t x, uint16_t y, uint16_t color, const FONT_INFO *fontInfo, char *str );
uint16_t  drawGetStringWidth   ( const FONT_INFO *fontInfo, char *str );
void      drawProgressBar      ( uint16_t x, uint16_t y, uint16_t width, uint16_t height, drawRoundedCorners_t borderCorners, drawRoundedCorners_t progressCorners, uint16_t borderColor, uint16_t borderFillColor, uint16_t progressBorderColor, uint16_t progressFillColor, uint8_t progress );
void      drawButton           ( uint16_t x, uint16_t y, uint16_t width, uint16_t height, const FONT_INFO *fontInfo, uint16_t fontHeight, uint16_t borderclr, uint16_t fillclr, uint16_t fontclr, char* text );
void      drawIcon16           ( uint16_t x, uint16_t y, uint16_t color, uint16_t icon[] );
uint16_t  drawRGB24toRGB565    ( uint8_t r, uint8_t g, uint8_t b );
uint32_t  drawRGB565toBGRA32   ( uint16_t color );
uint16_t  drawBGR2RGB          ( uint16_t color );

void drawCirclePoints(int cx, int cy, int x, int y, uint16_t color);

#if defined CFG_TFTLCD_INCLUDESMALLFONTS & CFG_TFTLCD_INCLUDESMALLFONTS == 1
void      drawStringSmall      ( uint16_t x, uint16_t y, uint16_t color, char* text, struct FONT_DEF font );
#endif

#if defined CFG_SDCARD
bmp_error_t   drawBitmapImage  ( uint16_t x, uint16_t y, char *filename );
#endif

#endif

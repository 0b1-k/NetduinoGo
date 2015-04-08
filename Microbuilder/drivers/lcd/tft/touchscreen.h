#ifndef __TOUCHSCREEN_H__
  #define __TOUCHSCREEN_H__

#include "projectconfig.h"
#include "adc.h"

#define CFG_TFTLCD_TS_DEFAULTTHRESHOLD (300)
#define CFG_TFTLCD_TS_KEYPADDELAY      (100)

typedef struct Point
{
  int32_t x;
  int32_t y;
} tsPoint_t;

typedef struct Matrix
{
  int32_t An,
          Bn,
          Cn,
          Dn,
          En,
          Fn,
          Divider;
} tsMatrix_t;

typedef struct
{
  uint32_t xraw;  // Touch screen x
  uint32_t yraw;  // Touch screen Y
  uint16_t xlcd;  // LCD co-ordinate X
  uint16_t ylcd;  // LCD co-ordinate Y
  uint32_t z1;
  uint32_t z2;
  bool valid;     // Whether this is a valid reading or not
} tsTouchData_t;

typedef enum
{
  TS_ERROR_NONE         = 0,
  TS_ERROR_TIMEOUT      = -1,   // Timeout occured before a valid reading
  TS_ERROR_XYMISMATCH   = -2    // Unable to get a stable X/Y value
} tsTouchError_t;

typedef struct
{
  int x;
  int y;
  int pressure;
} TouchData;

// Method Prototypes
void            tsInit ( void );
tsTouchError_t  tsRead(tsTouchData_t* data);
void            tsCalibrate ( void );
tsTouchError_t  tsWaitForEvent(tsTouchData_t* data, uint32_t timeoutMS);
int             tsSetThreshhold(unsigned short value);
unsigned short  tsGetThreshhold(void);
tsMatrix_t*     tsGetCalibrationMatrix();
#endif

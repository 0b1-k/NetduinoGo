#include "touchscreen.h"
#include "adc.h"
#include "gpio.h"
#include "systick.h"
#include "eeprom.h"
#include "lcd.h"
#include "drawing.h"
#include "dejavusans9.h"
#include "math.h"
#include "string.h"

#define TS_LINE1 "Touch the center of"
#define TS_LINE2 "the red circle using"
#define TS_LINE3 "a stylus"

#define CFG_EEPROM_TOUCHSCREEN_CALIBRATED   (uint16_t)(0x0030)    // 1
#define CFG_EEPROM_TOUCHSCREEN_CAL_AN       (uint16_t)(0x0031)    // 4
#define CFG_EEPROM_TOUCHSCREEN_CAL_BN       (uint16_t)(0x0035)    // 4
#define CFG_EEPROM_TOUCHSCREEN_CAL_CN       (uint16_t)(0x0039)    // 4
#define CFG_EEPROM_TOUCHSCREEN_CAL_DN       (uint16_t)(0x003D)    // 4
#define CFG_EEPROM_TOUCHSCREEN_CAL_EN       (uint16_t)(0x0041)    // 4
#define CFG_EEPROM_TOUCHSCREEN_CAL_FN       (uint16_t)(0x0045)    // 4
#define CFG_EEPROM_TOUCHSCREEN_CAL_DIVIDER  (uint16_t)(0x0049)    // 4
#define CFG_EEPROM_TOUCHSCREEN_THRESHHOLD   (uint16_t)(0x004D)    // 1

#define CFG_TFTLCD_TS_DELAYBETWEENSAMPLES      (1)

static unsigned short _tsThreshhold = CFG_TFTLCD_TS_DEFAULTTHRESHOLD;
tsPoint_t _tsLCDPoints[3];
tsPoint_t _tsTSPoints[3];
tsMatrix_t _tsMatrix;

/**************************************************************************/
/*                                                                        */
/* ----------------------- Private Methods ------------------------------ */
/*                                                                        */
/**************************************************************************/
/**************************************************************************/
/*!
    @brief  Reads the current Z/pressure level using the ADC
*/
/**************************************************************************/
void tsReadZ(uint32_t* z1, uint32_t* z2)
{
  // XP = ADC
  // XM = GPIO Output Low
  // YP = GPIO Output High
  // YM = GPIO Input

  GPIO_InitTypeDef  GPIO_InitStructure;

  GPIO_InitStructure.GPIO_Pin = ADC_XM_Pin | ADC_YP_Pin;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;

  GPIO_Init(ADC_PORT, &GPIO_InitStructure);

  GPIO_InitStructure.GPIO_Pin = ADC_YM_Pin;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;

  GPIO_Init(ADC_PORT, &GPIO_InitStructure);

  GPIO_WriteBit(ADC_PORT, ADC_XM_Pin, Bit_RESET);  // GND
  GPIO_WriteBit(ADC_PORT, ADC_YP_Pin, Bit_SET);  // 3.3V

  *z1 = adcRead(ADC_XP_CHANNEL);

  nopDelay(CFG_TFTLCD_TS_DELAYBETWEENSAMPLES);

  // XP = GPIO Input
  // XM = GPIO Output Low
  // YP = GPIO Output High
  // YM = ADC

  GPIO_InitStructure.GPIO_Pin = ADC_XP_Pin;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;

  GPIO_Init(ADC_PORT, &GPIO_InitStructure);

  *z2 = adcRead(ADC_YM_CHANNEL);
}

/**************************************************************************/
/*!
    @brief  Reads the current X position using the ADC
*/
/**************************************************************************/
uint32_t tsReadX(void)
{
  // XP = GPIO Output High
  // XM = GPIO Output Low
  // YP = ADC
  // YM = GPIO Input

  GPIO_InitTypeDef  GPIO_InitStructure;

  GPIO_InitStructure.GPIO_Pin = ADC_XP_Pin | ADC_XM_Pin;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;

  GPIO_Init(ADC_PORT, &GPIO_InitStructure);

  GPIO_InitStructure.GPIO_Pin = ADC_YM_Pin;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;

  GPIO_Init(ADC_PORT, &GPIO_InitStructure);

  GPIO_WriteBit(ADC_PORT, ADC_XP_Pin, Bit_SET);  // 3.3V
  GPIO_WriteBit(ADC_PORT, ADC_XM_Pin, Bit_RESET);  // GND

  return adcRead(ADC_YP_CHANNEL);
}

/**************************************************************************/
/*!
    @brief  Reads the current Y position using the ADC
*/
/**************************************************************************/
uint32_t tsReadY(void)
{
  // YP = GPIO Output High
  // YM = GPIO Output Low
  // XP = GPIO Input
  // XM = ADC

  GPIO_InitTypeDef  GPIO_InitStructure;

  GPIO_InitStructure.GPIO_Pin = ADC_YP_Pin | ADC_YM_Pin;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;

  GPIO_Init(ADC_PORT, &GPIO_InitStructure);

  GPIO_InitStructure.GPIO_Pin = ADC_XP_Pin;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;

  GPIO_Init(ADC_PORT, &GPIO_InitStructure);

  GPIO_WriteBit(ADC_PORT, ADC_YP_Pin, Bit_SET);  // 3.3V
  GPIO_WriteBit(ADC_PORT, ADC_YM_Pin, Bit_RESET);  // GND

  // Return the ADC results
  return adcRead(ADC_XM_CHANNEL);
}

/**************************************************************************/
/*!
    @brief  Centers a line of text horizontally
*/
/**************************************************************************/
void tsCalibCenterText(char* text, uint16_t y, uint16_t color) {
  drawString((lcdGetWidth() - drawGetStringWidth(&dejaVuSans9ptFontInfo, text)) / 2, y, color, &dejaVuSans9ptFontInfo, text);
}

/**************************************************************************/
/*!
    @brief  Renders the calibration screen with an appropriately
            placed test point and waits for a touch event
*/
/**************************************************************************/
tsTouchData_t tsRenderCalibrationScreen(uint16_t x, uint16_t y, uint16_t radius) {
  drawFill(COLOR_WHITE);
  tsCalibCenterText(TS_LINE1, 50, COLOR_GRAY_50);
  tsCalibCenterText(TS_LINE2, 65, COLOR_GRAY_50);
  tsCalibCenterText(TS_LINE3, 80, COLOR_GRAY_50);
  drawCircle(x, y, radius, COLOR_RED);
  drawCircle(x, y, radius + 2, COLOR_GRAY_128);

  // Wait for a valid touch events
  tsTouchData_t data;
  tsTouchError_t error;
  bool valid = false;
  while (!valid) {
    error = tsRead(&data);
    if (!error && data.valid) {
      valid = true;
    }
  }

  return data;
}

/**************************************************************************/
/*!
    @brief Calculates the difference between the touch screen and the
           actual screen co-ordinates, taking into account misalignment
           and any physical offset of the touch screen.

    @note  This is based on the public domain touch screen calibration code
           written by Carlos E. Vidales (copyright (c) 2001).

           For more information, see the following app notes:

           - AN2173 - Touch Screen Control and Calibration
             Svyatoslav Paliy, Cypress Microsystems
           - Calibration in touch-screen systems
             Wendy Fang and Tony Chang,
             Analog Applications Journal, 3Q 2007 (Texas Instruments)
*/
/**************************************************************************/
int setCalibrationMatrix( tsPoint_t * displayPtr, tsPoint_t * screenPtr, tsMatrix_t * matrixPtr) {
  int  retValue = 0;

  matrixPtr->Divider = ((screenPtr[0].x - screenPtr[2].x) * (screenPtr[1].y - screenPtr[2].y)) -
                       ((screenPtr[1].x - screenPtr[2].x) * (screenPtr[0].y - screenPtr[2].y)) ;

  if( matrixPtr->Divider == 0 ) {
    retValue = -1 ;
  } else {
    matrixPtr->An = ((displayPtr[0].x - displayPtr[2].x) * (screenPtr[1].y - screenPtr[2].y)) -
                    ((displayPtr[1].x - displayPtr[2].x) * (screenPtr[0].y - screenPtr[2].y)) ;

    matrixPtr->Bn = ((screenPtr[0].x - screenPtr[2].x) * (displayPtr[1].x - displayPtr[2].x)) -
                    ((displayPtr[0].x - displayPtr[2].x) * (screenPtr[1].x - screenPtr[2].x)) ;

    matrixPtr->Cn = (screenPtr[2].x * displayPtr[1].x - screenPtr[1].x * displayPtr[2].x) * screenPtr[0].y +
                    (screenPtr[0].x * displayPtr[2].x - screenPtr[2].x * displayPtr[0].x) * screenPtr[1].y +
                    (screenPtr[1].x * displayPtr[0].x - screenPtr[0].x * displayPtr[1].x) * screenPtr[2].y ;

    matrixPtr->Dn = ((displayPtr[0].y - displayPtr[2].y) * (screenPtr[1].y - screenPtr[2].y)) -
                    ((displayPtr[1].y - displayPtr[2].y) * (screenPtr[0].y - screenPtr[2].y)) ;

    matrixPtr->En = ((screenPtr[0].x - screenPtr[2].x) * (displayPtr[1].y - displayPtr[2].y)) -
                    ((displayPtr[0].y - displayPtr[2].y) * (screenPtr[1].x - screenPtr[2].x)) ;

    matrixPtr->Fn = (screenPtr[2].x * displayPtr[1].y - screenPtr[1].x * displayPtr[2].y) * screenPtr[0].y +
                    (screenPtr[0].x * displayPtr[2].y - screenPtr[2].x * displayPtr[0].y) * screenPtr[1].y +
                    (screenPtr[1].x * displayPtr[0].y - screenPtr[0].x * displayPtr[1].y) * screenPtr[2].y ;

    // Persist data to EEPROM
    eepromWriteS32(CFG_EEPROM_TOUCHSCREEN_CAL_AN, matrixPtr->An);
    eepromWriteS32(CFG_EEPROM_TOUCHSCREEN_CAL_BN, matrixPtr->Bn);
    eepromWriteS32(CFG_EEPROM_TOUCHSCREEN_CAL_CN, matrixPtr->Cn);
    eepromWriteS32(CFG_EEPROM_TOUCHSCREEN_CAL_DN, matrixPtr->Dn);
    eepromWriteS32(CFG_EEPROM_TOUCHSCREEN_CAL_EN, matrixPtr->En);
    eepromWriteS32(CFG_EEPROM_TOUCHSCREEN_CAL_FN, matrixPtr->Fn);
    eepromWriteS32(CFG_EEPROM_TOUCHSCREEN_CAL_DIVIDER, matrixPtr->Divider);
    eepromWriteU8(CFG_EEPROM_TOUCHSCREEN_CALIBRATED, 1);
  }

  return(retValue) ;
}

/**************************************************************************/
/*!
    @brief  Converts the supplied touch screen location (screenPtr) to
            a pixel location on the display (displayPtr) using the
            supplied matrix.  The screen orientation is also taken into
            account when converting the touch screen co-ordinate to
            a pixel location on the LCD.

    @note  This is based on the public domain touch screen calibration code
           written by Carlos E. Vidales (copyright (c) 2001).
*/
/**************************************************************************/
int getDisplayPoint( tsPoint_t * displayPtr, tsPoint_t * screenPtr, tsMatrix_t * matrixPtr ) {
  int  retValue = 0 ;

  if( matrixPtr->Divider != 0 ) {
    displayPtr->x = ( (matrixPtr->An * screenPtr->x) +
                      (matrixPtr->Bn * screenPtr->y) +
                       matrixPtr->Cn
                    ) / matrixPtr->Divider ;

    displayPtr->y = ( (matrixPtr->Dn * screenPtr->x) +
                      (matrixPtr->En * screenPtr->y) +
                       matrixPtr->Fn
                    ) / matrixPtr->Divider ;
  } else {
    retValue = -1 ;
  }

  // Adjust value if the screen is in landscape mode
  lcdOrientation_t orientation;
  orientation = lcdGetOrientation();
  if (orientation == LCD_ORIENTATION_LANDSCAPE) {
      uint32_t oldx, oldy;
      oldx = displayPtr->x;
      oldy = displayPtr->y;
      displayPtr->x = oldy;
      displayPtr->y = lcdGetHeight() - oldx;
  }

  return(retValue);
}

/**************************************************************************/
/*                                                                        */
/* ----------------------- Public Methods ------------------------------- */
/*                                                                        */
/**************************************************************************/

/**************************************************************************/
/*!
    @brief  Initialises the appropriate GPIO pins and ADC for the
            touchscreen
*/
/**************************************************************************/
void tsInit(void) {
  _tsThreshhold = tsGetThreshhold();
  // Load values from EEPROM if touch screen has already been calibrated
  if (eepromReadU8(CFG_EEPROM_TOUCHSCREEN_CALIBRATED) == 1) {
    // Load calibration data
    _tsMatrix.An = eepromReadS32(CFG_EEPROM_TOUCHSCREEN_CAL_AN);
    _tsMatrix.Bn = eepromReadS32(CFG_EEPROM_TOUCHSCREEN_CAL_BN);
    _tsMatrix.Cn = eepromReadS32(CFG_EEPROM_TOUCHSCREEN_CAL_CN);
    _tsMatrix.Dn = eepromReadS32(CFG_EEPROM_TOUCHSCREEN_CAL_DN);
    _tsMatrix.En = eepromReadS32(CFG_EEPROM_TOUCHSCREEN_CAL_EN);
    _tsMatrix.Fn = eepromReadS32(CFG_EEPROM_TOUCHSCREEN_CAL_FN);
    _tsMatrix.Divider = eepromReadS32(CFG_EEPROM_TOUCHSCREEN_CAL_DIVIDER);
  }
}

/**************************************************************************/
/*!
    @brief  Reads the current X, Y and Z co-ordinates of the touch screen
*/
/**************************************************************************/
tsTouchError_t tsRead(tsTouchData_t* data) {
  uint32_t x1, y1;

  // Assign pressure levels regardless of touch state
  tsReadZ(&data->z1, &data->z2);

  data->xraw = 0;
  data->yraw = 0;
  data->xlcd = 0;
  data->ylcd = 0;

  // Abort if the screen is not being touched (0 levels reported)
  if (data->z1 < _tsThreshhold) {
    data->valid = false;
    return TS_ERROR_NONE;
  }

  nopDelay(CFG_TFTLCD_TS_DELAYBETWEENSAMPLES);

  // Get two X/Y readings and compare results
  x1 = tsReadX();
  y1 = tsReadY();

  // X/Y seems to be valid and reading has been confirmed twice
  data->xraw = x1;
  data->yraw = y1;

  // Convert x/y values to pixel location with matrix multiply
  tsPoint_t location, touch;
  touch.x = x1;
  touch.y = y1;

  getDisplayPoint(&location, &touch, &_tsMatrix);
  data->xlcd = location.x;
  data->ylcd = location.y;
  data->valid = true;

  return TS_ERROR_NONE;
}

/**************************************************************************/
/*!
    @brief  Starts the screen calibration process.  Each corner will be
            tested, meaning that each boundary (top, left, right and
            bottom) will be tested twice and the readings averaged.
*/
/**************************************************************************/
void tsCalibrate(void)
{
  tsTouchData_t data;

  /* --------------- Welcome Screen --------------- */
  data = tsRenderCalibrationScreen(lcdGetWidth() / 2, lcdGetHeight() / 2, 5);

  /* ----------------- First Dot ------------------ */
  // 10% over and 10% down
  data = tsRenderCalibrationScreen(lcdGetWidth() / 10, lcdGetHeight() / 10, 5);
  _tsLCDPoints[0].x = lcdGetWidth() / 10;
  _tsLCDPoints[0].y = lcdGetHeight() / 10;
  _tsTSPoints[0].x = data.xraw;
  _tsTSPoints[0].y = data.yraw;

  /* ---------------- Second Dot ------------------ */
  // 50% over and 90% down
  data = tsRenderCalibrationScreen(lcdGetWidth() / 2, lcdGetHeight() - lcdGetHeight() / 10, 5);
  _tsLCDPoints[1].x = lcdGetWidth() / 2;
  _tsLCDPoints[1].y = lcdGetHeight() - lcdGetHeight() / 10;
  _tsTSPoints[1].x = data.xraw;
  _tsTSPoints[1].y = data.yraw;

  /* ---------------- Third Dot ------------------- */
  // 90% over and 50% down
  data = tsRenderCalibrationScreen(lcdGetWidth() - lcdGetWidth() / 10, lcdGetHeight() / 2, 5);
  _tsLCDPoints[2].x = lcdGetWidth() - lcdGetWidth() / 10;
  _tsLCDPoints[2].y = lcdGetHeight() / 2;
  _tsTSPoints[2].x = data.xraw;
  _tsTSPoints[2].y = data.yraw;

  // Do matrix calculations for calibration and store to EEPROM
  setCalibrationMatrix(&_tsLCDPoints[0], &_tsTSPoints[0], &_tsMatrix);
}

/**************************************************************************/
/*!
    @brief  Causes a blocking delay until a valid touch event occurs

    @note   Thanks to 'rossum' and limor for this nifty little tidbit on
            debouncing the signals via pressure sensitivity (using Z)

    @section Example

    @code
    #include "drivers/lcd/tft/touchscreen.h"
    ...
    tsTouchData_t data;
    tsTouchError_t error;

    while (1)
    {
      // Cause a blocking delay until a touch event occurs or 5s passes
      error = tsWaitForEvent(&data, 5000);

      if (error)
      {
        switch(error)
        {
          case TS_ERROR_TIMEOUT:
            printf("Timeout occurred %s", CFG_PRINTF_NEWLINE);
            break;
          default:
            break;
        }
      }
      else
      {
        // A valid touch event occurred ... display data
        printf("Touch Event: X = %04u, Y = %04u %s",
            data.xlcd,
            data.ylcd,
            CFG_PRINTF_NEWLINE);
      }
    }

    @endcode
*/
/**************************************************************************/
tsTouchError_t tsWaitForEvent(tsTouchData_t* data, uint32_t timeoutMS) {
  tsRead(data);

  // Return the results right away if reading is valid
  if (data->valid) {
    return TS_ERROR_NONE;
  }

  if(timeoutMS){
    int count = 3;
    while(count--){
      tsRead(data);
      if(data->valid){
        break;
      }
    }
  }else{
    while (!data->valid) {
      tsRead(data);
    }
  }

  // Indicate correct reading
  return TS_ERROR_NONE;
}

/**************************************************************************/
/*!
    @brief  Updates the touch screen threshhold level and saves it
            to EEPROM
*/
/**************************************************************************/
int tsSetThreshhold(unsigned short value) {
  // Update threshhold value
  _tsThreshhold = value;
  // Persist to EEPROM
  eepromWriteU16(CFG_EEPROM_TOUCHSCREEN_THRESHHOLD, value);
  return 0;
}

/**************************************************************************/
/*!
    @brief  Gets the current touch screen threshhold level from EEPROM
            (if present) or returns the default value from projectconfig.h
*/
/**************************************************************************/
unsigned short tsGetThreshhold(void) {
  // Check if custom threshold has been set in eeprom
  unsigned short thold = eepromReadU16(CFG_EEPROM_TOUCHSCREEN_THRESHHOLD);
  if (thold != 0xFFFF) {
    // Use value from EEPROM
    _tsThreshhold = thold;
  }
  else {
    // Use the default value from projectconfig.h
    _tsThreshhold = CFG_TFTLCD_TS_DEFAULTTHRESHOLD;
  }

  return _tsThreshhold;
}

// Returns a pointer to the touchscreen calibration data
tsMatrix_t* tsGetCalibrationMatrix() {
  return &_tsMatrix;
}

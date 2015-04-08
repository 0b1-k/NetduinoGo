#ifndef _INC_FIRMATA_BOARD_
#define _INC_FIRMATA_BOARD_

#include "stdint.h"

// Hardware Abstraction Layer for Firmata library
// For details, see the original Boards.h on http://firmata.org
// C adaptation to the STM32F051 by Fabien Royer | Nwazet, LLC. | nwazet.com

// Nwazet DAQ module board
typedef enum {
  NwazetDaqD0,
  NwazetDaqD1,
  NwazetDaqD2,
  NwazetDaqD3,
  NwazetDaqD4,
  NwazetDaqD5,
  NwazetDaqD6,
  NwazetDaqD7,
  NwazetDaqA0,
  NwazetDaqA1,
  NwazetDaqA2,
  NwazetDaqA3,
  NwazetDaqA4,
  NwazetDaqA5,
  NwazetDaqA6,
  NwazetDaqA7,
  NwazetDaqI2cScl,
  NwazetDaqI2cSda,
  NwazetDaqBlinkPin,
  NwazetDaqTotalPins
} NwazetDaqPins_t;

#define MAX_SERVOS              8
#define digitalPinHasPWM(p)     IS_PIN_DIGITAL(p)
#define TOTAL_ANALOG_PINS       8
#define TOTAL_PINS              NwazetDaqTotalPins
#define VERSION_BLINK_PIN       NwazetDaqBlinkPin
#define IS_PIN_DIGITAL(p)       ((p) >= NwazetDaqD0 && (p) <= NwazetDaqD7)
#define IS_PIN_ANALOG(p)        ((p) >= NwazetDaqA0 && (p) <= NwazetDaqA7)
#define IS_PIN_PWM(p)           digitalPinHasPWM(p)
#define IS_PIN_SERVO(p)         ((p) >= NwazetDaqD0 && (p) <= NwazetDaqD7)
#define IS_PIN_I2C(p)           ((p) == NwazetDaqI2cScl || (p) == NwazetDaqI2cSda)
#define PIN_TO_DIGITAL(p)       (p - NwazetDaqA0)
#define PIN_TO_ANALOG(p)        ((p) - NwazetDaqI2cScl)
#define PIN_TO_PWM(p)           PIN_TO_DIGITAL(p)
#define PIN_TO_SERVO(p)         (p)

uint8_t readPort(uint8_t port, uint8_t bitmask);
void writePort(uint8_t port, uint8_t value, uint8_t bitmask);

#endif
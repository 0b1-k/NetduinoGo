#include "FirmataBoards.h"
#include "digitalgpio.h"
#include "stdint.h"

uint8_t readPort(uint8_t port, uint8_t bitmask){
  uint8_t out = 0;
  uint8_t pin = port * 8;
  if (IS_PIN_DIGITAL(pin+0) && (bitmask & 0x01) && DigitalGpioRead((GpioId)PIN_TO_DIGITAL(pin+0))) out |= 0x01;
  if (IS_PIN_DIGITAL(pin+1) && (bitmask & 0x02) && DigitalGpioRead((GpioId)PIN_TO_DIGITAL(pin+1))) out |= 0x02;
  if (IS_PIN_DIGITAL(pin+2) && (bitmask & 0x04) && DigitalGpioRead((GpioId)PIN_TO_DIGITAL(pin+2))) out |= 0x04;
  if (IS_PIN_DIGITAL(pin+3) && (bitmask & 0x08) && DigitalGpioRead((GpioId)PIN_TO_DIGITAL(pin+3))) out |= 0x08;
  if (IS_PIN_DIGITAL(pin+4) && (bitmask & 0x10) && DigitalGpioRead((GpioId)PIN_TO_DIGITAL(pin+4))) out |= 0x10;
  if (IS_PIN_DIGITAL(pin+5) && (bitmask & 0x20) && DigitalGpioRead((GpioId)PIN_TO_DIGITAL(pin+5))) out |= 0x20;
  if (IS_PIN_DIGITAL(pin+6) && (bitmask & 0x40) && DigitalGpioRead((GpioId)PIN_TO_DIGITAL(pin+6))) out |= 0x40;
  if (IS_PIN_DIGITAL(pin+7) && (bitmask & 0x80) && DigitalGpioRead((GpioId)PIN_TO_DIGITAL(pin+7))) out |= 0x80;
  return out;
}
void writePort(uint8_t port, uint8_t value, uint8_t bitmask){
  uint8_t pin = port * 8;
  if ((bitmask & 0x01)) DigitalGpioWrite((GpioId)PIN_TO_DIGITAL(pin+0), (BitAction)(value & 0x01));
  if ((bitmask & 0x02)) DigitalGpioWrite((GpioId)PIN_TO_DIGITAL(pin+1), (BitAction)(value & 0x02));
  if ((bitmask & 0x04)) DigitalGpioWrite((GpioId)PIN_TO_DIGITAL(pin+2), (BitAction)(value & 0x04));
  if ((bitmask & 0x08)) DigitalGpioWrite((GpioId)PIN_TO_DIGITAL(pin+3), (BitAction)(value & 0x08));
  if ((bitmask & 0x10)) DigitalGpioWrite((GpioId)PIN_TO_DIGITAL(pin+4), (BitAction)(value & 0x10));
  if ((bitmask & 0x20)) DigitalGpioWrite((GpioId)PIN_TO_DIGITAL(pin+5), (BitAction)(value & 0x20));
  if ((bitmask & 0x40)) DigitalGpioWrite((GpioId)PIN_TO_DIGITAL(pin+6), (BitAction)(value & 0x40));
  if ((bitmask & 0x80)) DigitalGpioWrite((GpioId)PIN_TO_DIGITAL(pin+7), (BitAction)(value & 0x80));
}

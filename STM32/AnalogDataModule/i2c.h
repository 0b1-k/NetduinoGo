#ifndef _INC_I2C_
  #define _INC_I2C_

typedef enum {
  OK,
  Nack,
  BusError,
  ArbitrationLost,
  OverOrUnderRun,
  Timeout,
  InvalidBusSpeed,
  InvalidByteCount
} i2cResult_t;

typedef enum {
  TenkHz,
  OneHundredkHz,
  FourHundredkHz,
  InvalidSpeedkHz
} I2cSpeed_t;

void initI2C();
i2cResult_t i2cCheckResult(const char* func, i2cResult_t result);
i2cResult_t i2cWrite(I2cSpeed_t speed, uint16_t address, uint8_t* buffer, uint8_t count);
i2cResult_t i2cRead(I2cSpeed_t speed, uint16_t address, uint8_t* buffer, uint8_t count);
void i2cBusReset();
uint8_t i2cIsBusBusy();

#endif
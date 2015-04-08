#ifndef _INC_DIGITAL_GPIO_
  #define _INC_DIGITAL_GPIO_

#include "stm32f0xx_gpio.h"
#include "stm32f0xx_tim.h"
#include "stm32f0xx_exti.h"
#include "Nwazet.h"

typedef enum {
  GpioD0,
  GpioD1,
  GpioD2,
  GpioD3,
  GpioD4,
  GpioD5,
  GpioD6,
  GpioD7,
  GpioCount
} GpioId;

typedef enum {
  Input = GPIO_Mode_IN,
  Output = GPIO_Mode_OUT
} GpioMode;

typedef enum {
  PushPull,
  OpenDrain
} GpioType;

typedef enum {
  GpioResistorTypeNone = GPIO_PuPd_NOPULL,
  PullUp = GPIO_PuPd_UP,
  PullDown = GPIO_PuPd_DOWN
} GpioResistorType;

typedef enum {
  GpioAlternateFunctionNone,
  InputCaptureMode,
  InputEncoderMode,
  InputPwmMode,
  InputInterrupt,
  OutputPwmMode
} GpioAlternateFunction;

typedef struct {
  uint32_t      Count;
  uint32_t      RollOverLimit;
  uint32_t      RollOverCount;
  uint16_t      Updated;
} GpioInterruptCounter;

typedef struct {
  GpioId                Id;
  GpioMode              Mode;
  GpioType              Type;
  GpioResistorType      ResistorType;
  GpioAlternateFunction AlternateFunction;
  GPIO_TypeDef*         Port;
  TIM_TypeDef*          Timer;
  uint16_t              TimerChannel;
  uint16_t              Pin;
  uint16_t              TimerAlternateFunctionPinSource;
  uint8_t               TimerAlternateFunctionNumber;
  uint8_t               ExtiPortSource;
  uint8_t               ExtiPinSource;
  uint32_t              ExtiLine;
  GpioInterruptCounter  InterruptCounter;
} GpioDefinition;

void initDigitalGpio();

GpioDefinition* DigitalGpioFind(GpioId id);

void DigitalGpioDefine(GpioDefinition* def);
void DigitalGpioUndefine(GpioId id);

void DigitalGpioWrite(GpioId id, BitAction action);
void DigitalGpioPulse(GpioId id, BitAction action);
uint8_t DigitalGpioRead(GpioId id);
void DigitalGpioReadAll(SerializerContext* context);

void DigitalGpioTimeBaseDefine(GpioId id, uint16_t period, uint16_t prescaler);
void DigitalGpioPwmDutyCycleSet(GpioId id, uint32_t pulseLength);

void DigitalGpioInterruptConfig(GpioId id, FunctionalState state, EXTITrigger_TypeDef trigger, uint32_t rollOverLimit);
void DigitalGpioResetInterruptCounter(GpioId id);

void DigitalGpioGetInterruptCounters(SerializerContext* context, uint8_t reset);

#endif

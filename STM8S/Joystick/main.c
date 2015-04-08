#include "stm8s.h"
#include "main.h"
#include "Nwazet.h"
#include "GoBusModuleID.h"

//#define DEBUG_LED

/*
void delay(unsigned long duration){
  duration *= 10;
  while(duration--){
    __asm("nop");
  }
}
*/

typedef struct {
  uint16_t X;
  uint16_t Y;
} JoystickDataType;

#define MaxSpiTxBufferSize 17
#define MaxSpiRxBufferSize 18
static unsigned char spiRxBuffer[MaxSpiRxBufferSize];
static unsigned char spiTxBuffer[MaxSpiTxBufferSize];

#define JoystickDataOffset 5
static JoystickDataType* joystickDataBuffer = (JoystickDataType*) &spiTxBuffer[JoystickDataOffset];

static SerializerContext sendContext;

uint16_t GetAdcValue(ADC1_Channel_TypeDef channel){
  ADC1_ConversionConfig(ADC1_CONVERSIONMODE_SINGLE, channel, ADC1_ALIGN_RIGHT);
  ADC1_StartConversion();
  while(ADC1_GetFlagStatus(ADC1_FLAG_EOC) == RESET);
  ADC1_ClearFlag(ADC1_FLAG_EOC);
  return ADC1_GetConversionValue();
}
void InitializeSpiTxBuffer(){
  spiTxBuffer[0] = 'O';
  spiTxBuffer[1] = 'K';
  InitializeSendBuffer(&sendContext, spiTxBuffer, MaxSpiTxBufferSize);
  joystickDataBuffer->X = 0xAABB;
  joystickDataBuffer->Y = 0x5566;
  Put(&sendContext, (void*)&joystickDataBuffer->X, sizeof(joystickDataBuffer->X), 1);
  Put(&sendContext, (void*)&joystickDataBuffer->Y, sizeof(joystickDataBuffer->Y), 1);
  FinalizeSendBuffer(&sendContext);
}
void InitializeMasterClock(){
  CLK_DeInit();
  // Configure the Fcpu to DIV1
  CLK_SYSCLKConfig(CLK_PRESCALER_CPUDIV1);
  // Configure the HSI prescaler to the optimal value
  CLK_SYSCLKConfig(CLK_PRESCALER_HSIDIV1);
  // Configure the system clock to use HSI clock source and to run at 16Mhz
  ErrorStatus status = CLK_ClockSwitchConfig(CLK_SWITCHMODE_AUTO, CLK_SOURCE_HSI, ENABLE, CLK_CURRENTCLOCKSTATE_ENABLE);
}
void DisableUnusedPeripherals(){
  // Disable unused peripherals (CAN is in sleep mode by default)
  //ADC1_Cmd(DISABLE);
  AWU_Cmd(DISABLE);
  I2C_Cmd(DISABLE);
  CLK_LSICmd(DISABLE);
  //SPI_Cmd(DISABLE);
  TIM1_Cmd(DISABLE);
  TIM2_Cmd(DISABLE);
  TIM4_Cmd(DISABLE);
  UART1_Cmd(DISABLE);

  // Disable unused peripheral clocks
  //CLK_PeripheralClockConfig(CLK_PERIPHERAL_ADC, DISABLE);
  CLK_PeripheralClockConfig(CLK_PERIPHERAL_AWU, DISABLE);
  CLK_PeripheralClockConfig(CLK_PERIPHERAL_I2C, DISABLE);
  // CLK_PeripheralClockConfig(CLK_PERIPHERAL_SPI, DISABLE);
  CLK_PeripheralClockConfig(CLK_PERIPHERAL_TIMER1, DISABLE);
  CLK_PeripheralClockConfig(CLK_PERIPHERAL_TIMER2, DISABLE);
  CLK_PeripheralClockConfig(CLK_PERIPHERAL_TIMER4, DISABLE);
  CLK_PeripheralClockConfig(CLK_PERIPHERAL_UART1, DISABLE);
}
void InitializeGPIO(){
  // GPIO configuration
  GPIO_DeInit(GPIOA);
  GPIO_DeInit(GPIOB);
  GPIO_DeInit(GPIOC);
  GPIO_DeInit(GPIOD);

  // GO_BUS IRQ GPIO
  GPIO_Init(GPIOC, GPIO_PIN_3, GPIO_MODE_OUT_PP_LOW_SLOW);
  // ADC1 - Channel 2: x axis
  GPIO_Init(GPIOC, GPIO_PIN_4, GPIO_MODE_IN_FL_NO_IT);
  // ADC1 - Channel 3: y axis
  GPIO_Init(GPIOD, GPIO_PIN_2, GPIO_MODE_IN_FL_NO_IT);
  // SPI NSS
  GPIO_Init(GPIOA, GPIO_PIN_3, GPIO_MODE_IN_PU_IT);

  // Leave unused pins disconnected confgured as input, in pullup mode
#ifdef DEBUG_LED
  // Test LED GPIO
  GPIO_Init(GPIOA, GPIO_PIN_1, GPIO_MODE_OUT_PP_LOW_SLOW);
#else
  GPIO_Init(GPIOA, GPIO_PIN_1, GPIO_MODE_IN_PU_NO_IT);
#endif
  GPIO_Init(GPIOA, GPIO_PIN_2, GPIO_MODE_IN_PU_NO_IT);
  GPIO_Init(GPIOB, GPIO_PIN_4, GPIO_MODE_IN_PU_NO_IT);
  GPIO_Init(GPIOB, GPIO_PIN_5, GPIO_MODE_IN_PU_NO_IT);
  GPIO_Init(GPIOC, GPIO_PIN_3, GPIO_MODE_IN_PU_NO_IT);
  GPIO_Init(GPIOD, GPIO_PIN_3, GPIO_MODE_IN_PU_NO_IT);
  GPIO_Init(GPIOD, GPIO_PIN_4, GPIO_MODE_IN_PU_NO_IT);
  GPIO_Init(GPIOD, GPIO_PIN_5, GPIO_MODE_IN_PU_NO_IT);
  GPIO_Init(GPIOD, GPIO_PIN_6, GPIO_MODE_IN_PU_NO_IT);
}
void InitializeSPI(){
  SPI_DeInit();
  SPI_Init(
      SPI_FIRSTBIT_MSB,
      SPI_BAUDRATEPRESCALER_2,
      SPI_MODE_SLAVE,
      SPI_CLOCKPOLARITY_LOW,
      SPI_CLOCKPHASE_2EDGE,
      SPI_DATADIRECTION_2LINES_FULLDUPLEX,
      SPI_NSS_HARD,
      7);
  SPI_CalculateCRCCmd(ENABLE);
  SPI_ITConfig(SPI_IT_WKUP, ENABLE);
  SPI_Cmd(ENABLE);
}
void InitializeExternalInterrupts(){
  EXTI_DeInit();
  EXTI_SetExtIntSensitivity(EXTI_PORT_GPIOA, EXTI_SENSITIVITY_FALL_ONLY);
  ITC_DeInit();
  ITC_SetSoftwarePriority(ITC_IRQ_PORTA, ITC_PRIORITYLEVEL_2);
  enableInterrupts();
}
void InitializeADC(){
  ADC1_DeInit();
  ADC1_Init(
    ADC1_CONVERSIONMODE_SINGLE,
    ADC1_CHANNEL_2,
    ADC1_PRESSEL_FCPU_D2,
    ADC1_EXTTRIG_TIM, DISABLE,
    ADC1_ALIGN_RIGHT,
    ADC1_SCHMITTTRIG_CHANNEL2, DISABLE);
  ADC1_Init(
    ADC1_CONVERSIONMODE_SINGLE,
    ADC1_CHANNEL_3,
    ADC1_PRESSEL_FCPU_D2,
    ADC1_EXTTRIG_TIM, DISABLE,
    ADC1_ALIGN_RIGHT,
    ADC1_SCHMITTTRIG_CHANNEL3, DISABLE);
  ADC1_Cmd(ENABLE);
}
void DoSpiDataExchange(){
  unsigned char TxByteCount = MaxSpiTxBufferSize;
  unsigned char TxByte = 0;
  unsigned char RxByte = 0;
  SPI_ResetCRC();
  SPI_SendData(spiTxBuffer[TxByte++]);
  wfi();
  TxByteCount--;
  while(TxByteCount){
    while(SPI_GetFlagStatus(SPI_FLAG_TXE) == RESET);
    SPI_SendData(spiTxBuffer[TxByte++]);
    TxByteCount--;
    while(SPI_GetFlagStatus(SPI_FLAG_RXNE) == RESET);
    spiRxBuffer[RxByte++] = SPI_ReceiveData();
  }
  SPI_TransmitCRC();
  while(SPI_GetFlagStatus(SPI_FLAG_TXE) == RESET);
}

void main(void)
{
  InitializeMasterClock();
  DisableUnusedPeripherals();
  InitializeGPIO();
  InitializeSPI();
  InitializeExternalInterrupts();
  InitializeADC();

  InitializeModuleUUID(spiTxBuffer, MaxSpiTxBufferSize);

#ifdef DEBUG_LED
    GPIO_WriteHigh(GPIOA, GPIO_PIN_1);
#endif

  while(spiRxBuffer[0] != 0xFE && spiRxBuffer[1] != 0xFF){
    DoSpiDataExchange();
  }

  InitializeSpiTxBuffer();

#ifdef DEBUG_LED
  GPIO_WriteLow(GPIOA, GPIO_PIN_1);
#endif

  while(1){
    joystickDataBuffer->X = GetAdcValue(ADC1_CHANNEL_2);
    joystickDataBuffer->Y = GetAdcValue(ADC1_CHANNEL_3);
    DoSpiDataExchange();
  }
}

#ifdef USE_FULL_ASSERT
void assert_failed(u8* file, u32 line){
  while (1){}
}
#endif

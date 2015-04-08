#ifndef _INC_Peripherals_
  #define _INC_Peripherals_

// SPI2 port/pin aliases
#define SPI2_PORT       GPIOB
#define SPI2_NSS_PIN    GPIO_Pin_12
#define SPI2_SCK_PIN    GPIO_Pin_13
#define SPI2_MISO_PIN   GPIO_Pin_14
#define SPI2_MOSI_PIN   GPIO_Pin_15

// SPI2 alternate function 0 pin sources
#define SPI2_NSS_PIN_SOURCE    GPIO_PinSource12
#define SPI2_SCK_PIN_SOURCE    GPIO_PinSource13
#define SPI2_MISO_PIN_SOURCE   GPIO_PinSource14
#define SPI2_MOSI_PIN_SOURCE   GPIO_PinSource15

// Go! Bus /IRQ port/pin aliases
#define GO_BUS_IRQ_PORT         GPIOD
#define GO_BUS_IRQ_PIN          GPIO_Pin_2
#define GOBUS_GPIO_SPEED        GPIO_Speed_50MHz

// Led Controller port/pin aliases
#define SR_CLOCK_PORT           GPIOF
#define SR_CLEAR_PORT           GPIOF
#define SR_LATCH_PORT           GPIOC
#define SR_OUTPUT_ENABLE_PORT   GPIOC
#define SR_DATA_IN_PORT         GPIOC

#define SR_CLEAR_PIN            GPIO_Pin_4
#define SR_CLOCK_PIN            GPIO_Pin_5
#define SR_LATCH_PIN            GPIO_Pin_10
#define SR_OUTPUT_ENABLE_PIN    GPIO_Pin_11
#define SR_DATA_IN_PIN          GPIO_Pin_12
#define SR_GPIO_SPEED           GPIO_Speed_10MHz

// I2C LED pin aliases
#define I2C2_LED_PORT           GPIOB
#define I2C2_LED                GPIO_Pin_2
#define I2C2_LED_GPIO_SPEED     GPIO_Speed_10MHz

// SD LED pin aliases
#define SD_LED_PORT             GPIOC
#define SD_LED                  GPIO_Pin_6
#define SD_LED_GPIO_SPEED       GPIO_Speed_10MHz

// UART1 LED pin aliases
#define USART_LED_PORT          GPIOC
#define USART_LED               GPIO_Pin_7
#define USART_LED_GPIO_SPEED    GPIO_Speed_10MHz

// Digital GPIOs aliases
#define DIGITAL_GPIO_SPEED      GPIO_Speed_50MHz

#define D0_PORT                 GPIOB
#define D0_PIN                  GPIO_Pin_9
#define D0_TIMER                TIM17
#define D0_TIMER_CHANNEL        TIM_Channel_1
#define D0_TIMER_ALT_FUNCTION   GPIO_AF_2
#define D0_TIMER_PIN_SOURCE     GPIO_PinSource9
#define D0_EXTI_PORT_SOURCE     EXTI_PortSourceGPIOB
#define D0_EXTI_PIN_SOURCE      EXTI_PinSource9
#define D0_EXTI_LINE            EXTI_Line9

#define D1_PORT                 GPIOB
#define D1_PIN                  GPIO_Pin_8
#define D1_TIMER                TIM16
#define D1_TIMER_CHANNEL        TIM_Channel_1
#define D1_TIMER_ALT_FUNCTION   GPIO_AF_2
#define D1_TIMER_PIN_SOURCE     GPIO_PinSource8
#define D1_EXTI_PORT_SOURCE     EXTI_PortSourceGPIOB
#define D1_EXTI_PIN_SOURCE      EXTI_PinSource8
#define D1_EXTI_LINE            EXTI_Line8

#define D2_PORT                 GPIOB
#define D2_PIN                  GPIO_Pin_10
#define D2_TIMER                TIM2
#define D2_TIMER_CHANNEL        TIM_Channel_3
#define D2_TIMER_ALT_FUNCTION   GPIO_AF_2
#define D2_TIMER_PIN_SOURCE     GPIO_PinSource10
#define D2_EXTI_PORT_SOURCE     EXTI_PortSourceGPIOB
#define D2_EXTI_PIN_SOURCE      EXTI_PinSource10
#define D2_EXTI_LINE            EXTI_Line10

#define D3_PORT                 GPIOB
#define D3_PIN                  GPIO_Pin_11
#define D3_TIMER                TIM2
#define D3_TIMER_CHANNEL        TIM_Channel_4
#define D3_TIMER_ALT_FUNCTION   GPIO_AF_2
#define D3_TIMER_PIN_SOURCE     GPIO_PinSource11
#define D3_EXTI_PORT_SOURCE     EXTI_PortSourceGPIOB
#define D3_EXTI_PIN_SOURCE      EXTI_PinSource11
#define D3_EXTI_LINE            EXTI_Line11

#define D4_PORT                 GPIOA
#define D4_PIN                  GPIO_Pin_4
#define D4_TIMER                TIM14
#define D4_TIMER_CHANNEL        TIM_Channel_1
#define D4_TIMER_ALT_FUNCTION   GPIO_AF_4
#define D4_TIMER_PIN_SOURCE     GPIO_PinSource4
#define D4_EXTI_PORT_SOURCE     EXTI_PortSourceGPIOA
#define D4_EXTI_PIN_SOURCE      EXTI_PinSource4
#define D4_EXTI_LINE            EXTI_Line0

#define D5_PORT                 GPIOB
#define D5_PIN                  GPIO_Pin_3
#define D5_TIMER                TIM2
#define D5_TIMER_CHANNEL        TIM_Channel_2
#define D5_TIMER_ALT_FUNCTION   GPIO_AF_2
#define D5_TIMER_PIN_SOURCE     GPIO_PinSource3
#define D5_EXTI_PORT_SOURCE     EXTI_PortSourceGPIOB
#define D5_EXTI_PIN_SOURCE      EXTI_PinSource3
#define D5_EXTI_LINE            EXTI_Line3

#define D6_PORT                 GPIOB
#define D6_PIN                  GPIO_Pin_4
#define D6_TIMER                TIM3
#define D6_TIMER_CHANNEL        TIM_Channel_1
#define D6_TIMER_ALT_FUNCTION   GPIO_AF_1
#define D6_TIMER_PIN_SOURCE     GPIO_PinSource4
#define D6_EXTI_PORT_SOURCE     EXTI_PortSourceGPIOB
#define D6_EXTI_PIN_SOURCE      EXTI_PinSource4
#define D6_EXTI_LINE            EXTI_Line4

#define D7_PORT                 GPIOB
#define D7_PIN                  GPIO_Pin_5
#define D7_TIMER                TIM3
#define D7_TIMER_CHANNEL        TIM_Channel_2
#define D7_TIMER_ALT_FUNCTION   GPIO_AF_1
#define D7_TIMER_PIN_SOURCE     GPIO_PinSource5
#define D7_EXTI_PORT_SOURCE     EXTI_PortSourceGPIOB
#define D7_EXTI_PIN_SOURCE      EXTI_PinSource5
#define D7_EXTI_LINE            EXTI_Line5

#define DEFAULT_PERIOD_HZ       1000
#define DEFAULT_TIMER_PERIOD    (SystemCoreClock / DEFAULT_PERIOD_HZ ) - 1

// SD card GPIO aliases
#define SD_SPI                          SPI1

#define SD_GPIO_SPEED                   GPIO_Speed_10MHz

#define SD_SPI_SCK_PIN                  GPIO_Pin_5
#define SD_SPI_SCK_GPIO_PORT            GPIOA
#define SD_SPI_SCK_SOURCE               GPIO_PinSource5
#define SD_SPI_SCK_AF                   GPIO_AF_0

#define SD_SPI_MOSI_PIN                 GPIO_Pin_7
#define SD_SPI_MOSI_GPIO_PORT           GPIOA
#define SD_SPI_MOSI_SOURCE              GPIO_PinSource7
#define SD_SPI_MOSI_AF                  GPIO_AF_0

#define SD_SPI_MISO_PIN                 GPIO_Pin_6
#define SD_SPI_MISO_GPIO_PORT           GPIOA
#define SD_SPI_MISO_SOURCE              GPIO_PinSource6
#define SD_SPI_MISO_AF                  GPIO_AF_0

#define SD_CS_PIN                       GPIO_Pin_15
#define SD_CS_GPIO_PORT                 GPIOA

#define SD_DETECT_PIN                   GPIO_Pin_8
#define SD_DETECT_GPIO_PORT             GPIOC

#define SD_WRITE_PROTECT_PIN            GPIO_Pin_9
#define SD_WRITE_PROTECT_GPIO_PORT      GPIOC

#define SD_DETECT_EXTI_LINE             EXTI_Line8
#define SD_DETECT_EXTI_PIN_SOURCE       EXTI_PinSource8
#define SD_DETECT_EXTI_PORT_SOURCE      EXTI_PortSourceGPIOC
#define SD_DETECT_EXTI_IRQn             EXTI4_15_IRQHandler

// USART GPIO aliases
#define USART_PORT                      USART2
#define USART_GPIO_PORT                 GPIOA
#define USART_GPIO_SPEED                GPIO_Speed_50MHz
#define USART_CTS_PIN                   GPIO_Pin_0
#define USART_RTS_PIN                   GPIO_Pin_1
#define USART_TX_PIN                    GPIO_Pin_2
#define USART_RX_PIN                    GPIO_Pin_3
#define USART_CTS_PIN_SOURCE            GPIO_PinSource0
#define USART_RTS_PIN_SOURCE            GPIO_PinSource1
#define USART_TX_PIN_SOURCE             GPIO_PinSource2
#define USART_RX_PIN_SOURCE             GPIO_PinSource3
#define USART_AF                        GPIO_AF_1
#define USART_DEFAULT_BAUD_RATE         115200

// I2C GPIO aliases
#define I2C_GPIO_SPEED                  GPIO_Speed_50MHz
#define I2C_SCL_PIN                     GPIO_Pin_6
#define I2C_SDA_PIN                     GPIO_Pin_7
#define I2C_SCL_PIN_SOURCE              GPIO_PinSource6
#define I2C_SDA_PIN_SOURCE              GPIO_PinSource7
#define I2C_GPIO_PORT                   GPIOF
#define I2C_AF                          GPIO_AF_0
#define I2C_PERIPHERAL                  I2C2

void initPeripherals();

#endif

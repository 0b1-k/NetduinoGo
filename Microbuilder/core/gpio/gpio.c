#include "gpio.h"

void gpioInit(){
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);
  
  GPIO_DeInit(GPIOA);
  GPIO_DeInit(GPIOB);
  GPIO_DeInit(GPIOC);
  
  GPIO_InitTypeDef  GPIO_InitStructure;
  
  GPIO_StructInit(&GPIO_InitStructure);
 
  // Initialize the GPIOs controlling the 16-bit LCD data bus (split across 2 ports)
  // Data bus bits are mapped to identical GPIO lines: Port C = bit 0-12, Port B = bit 13-15
  //
  // Low 13 bits
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3 | GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7 | GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10 | GPIO_Pin_11 | GPIO_Pin_12;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_Init(TFT_LCD_DATA_PORT_LOW, &GPIO_InitStructure);

  // High 3 bits
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_Init(TFT_LCD_DATA_PORT_HIGH, &GPIO_InitStructure);
  
  // Initialize the FMARK pin as an input (needs to be associated with an interrupt handler, unused for now)
  GPIO_InitStructure.GPIO_Pin = TFT_LCD_FMARK_PIN;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_Init(TFT_LCD_FMARK_PORT, &GPIO_InitStructure);

  // Initialize the LCD control lines
  GPIO_InitStructure.GPIO_Pin = TFT_LCD_CS_PIN | TFT_LCD_DC_PIN | TFT_LCD_WR_PIN | TFT_LCD_RD_PIN | TFT_LCD_RESET_PIN;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_Init(TFT_LCD_CONTROL_PORT, &GPIO_InitStructure);

  // Initialize the LCD Interface Mode lines
  GPIO_InitStructure.GPIO_Pin = TFT_LCD_IM0_PIN;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_Init(TFT_LCD_INTERFACE_PORT, &GPIO_InitStructure);
  
  GPIO_InitStructure.GPIO_Pin = TFT_LCD_IM1_PIN | TFT_LCD_IM2_PIN | TFT_LCD_IM3_PIN;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;  
  
  // Setup the LCD interface Mode line (one-time only) for 8080-I 16 bits mode
  GPIO_WriteBit(TFT_LCD_INTERFACE_PORT, TFT_LCD_IM0_PIN, Bit_SET);    //3.3v
  GPIO_WriteBit(TFT_LCD_INTERFACE_PORT, TFT_LCD_IM1_PIN, Bit_RESET);  // GND
  GPIO_WriteBit(TFT_LCD_INTERFACE_PORT, TFT_LCD_IM2_PIN, Bit_RESET);  // GND
  GPIO_WriteBit(TFT_LCD_INTERFACE_PORT, TFT_LCD_IM3_PIN, Bit_RESET);  // GND
}

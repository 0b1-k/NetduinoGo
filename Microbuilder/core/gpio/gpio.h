#ifndef _GPIO_H_
  #define _GPIO_H_

#include "projectconfig.h"

#define TFT_LCD_DATA_PORT_LOW   GPIOC
#define TFT_LCD_DATA_PORT_HIGH  GPIOB

#define TFT_LCD_FMARK_PIN     GPIO_Pin_12
#define TFT_LCD_FMARK_PORT    GPIOB

#define TFT_LCD_CS_PIN        GPIO_Pin_8
#define TFT_LCD_DC_PIN        GPIO_Pin_9
#define TFT_LCD_WR_PIN        GPIO_Pin_10
#define TFT_LCD_RD_PIN        GPIO_Pin_11
#define TFT_LCD_RESET_PIN     GPIO_Pin_12
#define TFT_LCD_CONTROL_PORT  GPIOA
  
#define TFT_LCD_IM0_PIN         GPIO_Pin_5
#define TFT_LCD_IM1_PIN         GPIO_Pin_6
#define TFT_LCD_IM2_PIN         GPIO_Pin_7
#define TFT_LCD_IM3_PIN         GPIO_Pin_8
#define TFT_LCD_INTERFACE_PORT  GPIOB

#define lcdTouchscreenYDPin   GPIO_Pin_0
#define lcdTouchScreenXRPin   GPIO_Pin_1
#define lcdTouchscreenYUPin   GPIO_Pin_2
#define lcdTouchscreenXLPin   GPIO_Pin_3
#define TS_PORT GPIOA

void gpioInit();

#endif
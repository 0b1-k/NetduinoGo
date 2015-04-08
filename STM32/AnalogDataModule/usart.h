#ifndef _INC_USART_
#define _INC_USART_

#include "Nwazet.h"
#include "stdint.h"

void initUSART(uint32_t baudRate);
uint8_t usartConfig(SerializerContext* context);
int usartTest();
int usartGet();
int usartPut(uint16_t d);
int usartPuts(unsigned char* s);
void usartPrintf(uint8_t enabled);
uint16_t usartGetRxTimeOutMs();

#define USART_TIMEOUT           -1
#define MAX_USART_TIMEOUT       0xFFFFFFFF
#endif

#ifndef _USART_H
#define _USART_H

#include<avr/io.h>
#include "../defs.h"

#define BAUDRATE 9600
#define BAUD_PRESCALLER (((MASTER_F_CPU / (BAUDRATE * 16UL))) - 1)

void USART_init(void);
unsigned char USART_receive(void);
void USART_send(uint8_t);
void USART_send_array(uint8_t*, uint8_t);

#endif
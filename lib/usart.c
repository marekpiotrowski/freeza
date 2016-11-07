#include "usart.h"

void USART_init(void){
	UBRR0H = (uint8_t)(BAUD_PRESCALLER>>8);
	UBRR0L = (uint8_t)(BAUD_PRESCALLER);
	UCSR0B = (1<<RXEN0)|(1<<TXEN0)|(1<<RXCIE0);
	UCSR0C = (3<<UCSZ00);
}
 
uint8_t USART_receive(void){
	while(!(UCSR0A & (1<<RXC0)));
	return UDR0;
}
 
void USART_send(uint8_t data){
	while(!(UCSR0A & (1<<UDRE0)));
	UDR0 = data;
}

void USART_send_array(uint8_t* data, uint8_t data_size) {
	uint8_t i;
	for(i = 0; i < data_size; i++) {
		USART_send(data[i]);
	}
}
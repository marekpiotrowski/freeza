#include "defs.h"
#include "lib/usart.h"
#include "lib/spi.h"
#include<util/delay.h> 
                       

int main() 
{ 
	USART_init();
    SPI_init_m328p(); 
	
    while(1)               
    { 
		uint8_t received = USART_receive();
        SPI_send(received); 
		USART_send('[');
		USART_send(received);
		USART_send(']');
		_delay_ms(50);
    } 
	
    return 0; 
}
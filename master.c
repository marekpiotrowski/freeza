#include "defs.h"
#include "lib/usart.h"
#include "lib/spi.h"
#include <util/delay.h> 
#include <avr/interrupt.h>
#include "instruction.h"

//frame format [ <<instructions>> ]                

ISR(USART_RX_vect) {
	uint8_t value, status;
	
	value = UDR0;
	
	SPI_send(value);
	
	Instruction_write(value);
}

int main() 
{ 
	uint8_t buffer[10];
	
	USART_init();
    SPI_init_m328p(); 
	sei();
    while(1)               
    { 
		switch(Instruction_get_status()) {
			case READING_DONE:
				Instruction_read(buffer);
				USART_send_array(buffer, Instruction_get_size());
				Instruction_set_idle();
				break;
		}
    } 
	
    return 0; 
}
#include "defs.h"

#ifndef F_CPU
#define F_CPU MASTER_F_CPU
#endif

#include "lib/usart.h"
#include "lib/spi.h"
#include <util/delay.h> 
#include <avr/interrupt.h>
#include "instruction.h"

//frame format [ <<instructions>> ]

#define MOTOR_X 1
#define MOTOR_Y 2
#define MOTOR_Z 3   

void send_to_motor_driver(uint8_t, int16_t, uint16_t);   

int16_t get_final_destination_from_data_frame(uint8_t motor, char* buffer); 
uint16_t calculate_time(uint8_t motor, int16_t final_destination, char* buffer);

ISR(USART_RX_vect) {
	uint8_t value, status;
	
	value = UDR0;
	
	
	Instruction_write(value);
}

int main() 
{ 
	uint8_t buffer[10];
	int16_t final_destination;
	uint16_t time;
	
	USART_init();
    SPI_init_m328p(); 
	sei();
    while(1)               
    { 
		switch(Instruction_get_status()) {
			case READING_DONE:
				Instruction_read(buffer);
				//PROCESS INSTRUCTION
				//TO PRODUCE COORDINATES AND TIME
				final_destination = get_final_destination_from_data_frame(MOTOR_X, buffer);
				
				time = calculate_time(MOTOR_X, final_destination, buffer);
				
				// additional processing in order to execute instruction properly
				send_to_motor_driver(MOTOR_X, final_destination, time);
				
				Instruction_set_idle();
				break;
		}
    } 
	
    return 0; 
}

void send_to_motor_driver(uint8_t motor, int16_t partial_destination, uint16_t time) {
	//set SS' PORT accordingly - to enable only 1 driver - 'motor' variable will be used here

	SPI_send(partial_destination & LOWER_BYTE);
	USART_send((partial_destination & LOWER_BYTE) + '0');
	SPI_send((partial_destination & HIGHER_BYTE) >> 8);
		USART_send(((partial_destination & HIGHER_BYTE) >> 8) + '0');

	SPI_send(partial_destination > 0 ? TRUE : FALSE);
			USART_send((partial_destination > 0 ? TRUE : FALSE) + '0');

	SPI_send(time & LOWER_BYTE);
				USART_send((time & LOWER_BYTE) + '0');

	SPI_send((time & HIGHER_BYTE) >> 8);
				USART_send(((time & HIGHER_BYTE) >> 8)  + '0');

}

int16_t get_final_destination_from_data_frame(uint8_t motor, char* buffer) {
	//TO IMPLEMENT
	return 5;
}

uint16_t calculate_time(uint8_t motor, int16_t final_destination, char* buffer) {
	//TO IMPLEMENT
	return 0;
}
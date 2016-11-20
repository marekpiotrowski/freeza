#include "defs.h"

#define F_CPU SLAVE_F_CPU

#include "lib/spi.h"
#include<avr/io.h> 
#include<avr/interrupt.h>
#include<math.h>
#include<util/delay.h>

#define DATA_NOT_READY 0
#define DATA_READY 1
#define DATA_CHUNK_SIZE 5



void move_forward(uint16_t delta, uint16_t time);
void move_backward(uint16_t delta, uint16_t time);

int16_t get_destination_from_data_frame(); 
uint16_t get_time_from_data_frame();

int16_t current_coordinate;

uint8_t buffer[DATA_CHUNK_SIZE];
volatile uint8_t status = DATA_NOT_READY;
volatile uint8_t buffer_index = 0;

ISR(SPI_STC_vect)
{
	uint8_t data = SPDR;
	
	if(status == DATA_NOT_READY) {
		buffer[buffer_index] = data;
		buffer_index++;
		if(buffer_index == DATA_CHUNK_SIZE) {
			status = DATA_READY;
			buffer_index = 0;
		}
	}
}


int main() 
{ 
	int16_t destination_coordinate;
	uint16_t time;
	uint8_t j;
	
    DDRD = 0xFF; 
    PORTD = 0x00;
	
	current_coordinate = 0;
	
	
    SPI_init_m16(); 
	sei();
	
    while(1) 
    { 
		if(status == DATA_READY) {
			destination_coordinate = get_destination_from_data_frame();
			time = get_time_from_data_frame();
			
			if(destination_coordinate > current_coordinate)
				move_forward(abs(destination_coordinate - current_coordinate), time);
			else
				move_backward(abs(destination_coordinate - current_coordinate), time);
				
			status = DATA_NOT_READY;
		}
    }
	
    return 0; 
}

void move_forward(uint16_t delta, uint16_t time) {
	while(1) { //TODO change condition to sth sensible - timer
		PORTD = 0b00001001;
		_delay_ms(5);

		PORTD = 0b00000101;
		_delay_ms(5);
		
		PORTD = 0b00000110;
		_delay_ms(5);
		
		PORTD = 0b00001010;
		_delay_ms(5);
	}
}

void move_backward(uint16_t delta, uint16_t time) {
	while(1) { //TODO change condition to sth sensible - timer
		PORTD = 0b00001010;
		_delay_ms(5);
		
		PORTD = 0b00000110;
		_delay_ms(5);
		
		PORTD = 0b00000101;
		_delay_ms(5);
		
		PORTD = 0b00001001;
		_delay_ms(5);
	}
}

int16_t get_destination_from_data_frame() {
	int16_t result = 0;
	uint8_t lower_byte;
	uint8_t higher_byte;
	uint8_t greater_than_zero;
	
	lower_byte = buffer[SPI_LOWER_BYTE_DESTINATION];
	higher_byte = buffer[SPI_HIGHER_BYTE_DESTINATION];
	greater_than_zero = buffer[SPI_SIGN_DESTINATION];
	
	result = (higher_byte << 8) | lower_byte;
	
	return greater_than_zero == TRUE ? result : -result;
}

uint16_t get_time_from_data_frame() {
	uint16_t result = 0;
	uint8_t lower_byte;
	uint8_t higher_byte;
	
	lower_byte = buffer[SPI_LOWER_BYTE_TIME];
	higher_byte = buffer[SPI_HIGHER_BYTE_TIME];
	result = (higher_byte << 8) | lower_byte;
	return result;
}
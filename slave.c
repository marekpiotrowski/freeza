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

#define STD_DELAY 500

void move_forward(uint16_t delta, uint16_t time);
void move_backward(uint16_t delta, uint16_t time);

int16_t get_destination_from_data_frame(); 
uint16_t get_time_from_data_frame();

void stop_motors();

int16_t current_coordinate;
int16_t error;

uint8_t buffer[DATA_CHUNK_SIZE];
volatile uint8_t status = DATA_NOT_READY;
volatile uint8_t buffer_index = 0;

volatile uint8_t motor_status;
volatile uint8_t total_overflows;

ISR(SPI_STC_vect)
{
	uint8_t data = SPDR;
	uint8_t i;
	if(status == DATA_NOT_READY) {
		buffer[buffer_index] = data;
		buffer_index++;
		if(buffer_index == DATA_CHUNK_SIZE) {
			if(buffer[0] == STOP && buffer[1] == STOP && buffer[2] == STOP && buffer[3] == STOP
				&& buffer[4] == STOP) {
				motor_status = MOTOR_STOPPED;
				status = DATA_NOT_READY;
			}
			else {
				status = DATA_READY;
			}
			buffer_index = 0;
		}
	}
}


void timer0_init()
{
    TCCR0 |= (1 << CS02)|(1 << CS00);
  
    TIMSK |= (1 << TOIE0);
}

int main() 
{ 
	int16_t destination_coordinate;
	uint16_t time;
	uint8_t j;
	
    DDRD = 0xFF; 
    PORTD = 0xFF;
	
	current_coordinate = 0;
	error = 0;
	
    SPI_init_m16(); 
	sei();
	
	TCNT0 = 0;
    total_overflows = 0;

    while(1) 
    { 
		if(status == DATA_READY) {
			status = DATA_NOT_READY;

			destination_coordinate = get_destination_from_data_frame();
			time = get_time_from_data_frame();

			if(destination_coordinate > current_coordinate) {
				motor_status = MOTOR_RUNNING_FORWARD;
				move_forward(abs(destination_coordinate - current_coordinate), time);
			}
			else {
				motor_status = MOTOR_RUNNING_BACKWARD;	
				move_backward(abs(destination_coordinate - current_coordinate), time);
			}	
		}
    }
	
    return 0; 
}

void stop_motors() {
	PORTD = 0;
	motor_status = MOTOR_STOPPED;
}

void move_forward(uint16_t delta, uint16_t time) {
	uint32_t delta_um = delta * 1000;
	delta_um += error;
	uint32_t delta_um_normalized;

	delta_um_normalized = (uint32_t)((delta_um / DISPLACEMENT_PER_STEP) + 0.5f) * DISPLACEMENT_PER_STEP;
	error += delta_um - delta_um_normalized;
	
	while(motor_status != MOTOR_STOPPED && delta_um_normalized > 0) {
		PORTD = 0b00001001;
		_delay_ms(STD_DELAY);

		PORTD = 0b00000101;
		_delay_ms(STD_DELAY);
		
		PORTD = 0b00000110;
		_delay_ms(STD_DELAY);
		
		PORTD = 0b00001010;
		_delay_ms(STD_DELAY);
		delta_um_normalized -= DISPLACEMENT_PER_STEP;
	}
	stop_motors();
}

void move_backward(uint16_t delta, uint16_t time) {
	uint32_t delta_um = delta * 1000;
	delta_um += error;
	uint32_t delta_um_normalized;
	
	delta_um_normalized = (uint32_t)((delta_um / DISPLACEMENT_PER_STEP) + 0.5f) * DISPLACEMENT_PER_STEP;
	error += delta_um - delta_um_normalized;

	while(motor_status != MOTOR_STOPPED && delta_um_normalized > 0) {
		PORTD = 0b00001010;
		_delay_ms(STD_DELAY);
		
		PORTD = 0b00000110;
		_delay_ms(STD_DELAY);
		
		PORTD = 0b00000101;
		_delay_ms(STD_DELAY);
		
		PORTD = 0b00001001;
		_delay_ms(STD_DELAY);
		
		delta_um_normalized -= DISPLACEMENT_PER_STEP;
	}
	stop_motors();
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
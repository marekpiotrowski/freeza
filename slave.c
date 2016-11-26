#include "defs.h"

#define F_CPU SLAVE_F_CPU

#include "lib/spi.h"
#include<avr/io.h> 
#include<avr/interrupt.h>
#include<math.h>
#include<util/delay.h>

#define DATA_NOT_READY 0
#define DATA_READY 1
#define DATA_CHUNK_SIZE 4

#define STD_DELAY 500

void move_forward(uint16_t number_of_steps, uint16_t delta, uint32_t time);
void move_backward(uint16_t number_of_steps, uint16_t delta, uint32_t time);

int16_t get_destination_from_data_frame(); 
uint16_t get_time_from_data_frame();

void stop_motors();

volatile int16_t current_coordinate;
int16_t error;

volatile uint8_t buffer[DATA_CHUNK_SIZE];
volatile uint8_t status = DATA_NOT_READY;
volatile uint8_t buffer_index = 0;

volatile uint8_t motor_status;
volatile uint8_t total_overflows;

volatile uint8_t should_send_ack;

ISR(SPI_STC_vect)
{
	uint8_t data = SPDR;
	uint8_t i;
	if(data == STATUS_CHECK && should_send_ack < 3) {
		if(should_send_ack == 0) { SPI_send('X'); should_send_ack++; }
		if(should_send_ack == 1) { SPI_send(current_coordinate & LOWER_BYTE); should_send_ack++; }
		if(should_send_ack == 2) { SPI_send((current_coordinate & HIGHER_BYTE) >> 8); should_send_ack++; }
	}
	else if(status == DATA_NOT_READY && data != STATUS_CHECK) {
		buffer[buffer_index] = data;
		buffer_index++;
		if(buffer_index == DATA_CHUNK_SIZE) {
			if(buffer[0] == STOP && buffer[1] == STOP && buffer[2] == STOP && buffer[3] == STOP) {
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

int main() 
{ 
	int16_t destination_coordinate;
	uint32_t delta;
	uint16_t time;
	uint32_t time_us;
	uint32_t time_per_step_us = 0;
	uint16_t number_of_steps;
	uint8_t j;
	should_send_ack = 3;
	
    DDRD = 0xFF; 
    PORTD = 0xFF;
	
	current_coordinate = 0;
	error = 0;
	
    SPI_init_m16(); 
	sei();

    while(1) 
    { 
		if(status == DATA_READY) {		
			destination_coordinate = get_destination_from_data_frame();

			time = get_time_from_data_frame();		
			time_us = (uint32_t)time * 1000;
			delta = abs(destination_coordinate - current_coordinate);
			if(delta != 0) {
				number_of_steps = round(((delta * 1000) + error) / DISPLACEMENT_PER_STEP);
				time_per_step_us = (uint32_t)round(time_us / (float)number_of_steps);

				
				
				if(time_per_step_us == 0)
					time_per_step_us = MIN_TIME_PER_STEP;
				
				if(destination_coordinate > current_coordinate) {
					motor_status = MOTOR_RUNNING_FORWARD;
					move_forward(number_of_steps, delta, time_per_step_us / 4);
				}
				else {
					motor_status = MOTOR_RUNNING_BACKWARD;	
					move_backward(number_of_steps, delta, time_per_step_us / 4);
				}
				
			}
			status = DATA_NOT_READY;
			should_send_ack	= 0;
		}
    }
	
    return 0; 
}

void stop_motors() {
	PORTD = 0;
	motor_status = MOTOR_STOPPED;
}

void move_forward(uint16_t number_of_steps, uint16_t delta, uint32_t time) {
	uint32_t delta_um_normalized;
	delta_um_normalized = number_of_steps * DISPLACEMENT_PER_STEP;
	
	//error += (delta * 1000) - delta_um_normalized;
	current_coordinate += round((number_of_steps * DISPLACEMENT_PER_STEP) / 1000.0f);	

	while(motor_status != MOTOR_STOPPED && number_of_steps > 0) {
		PORTD = 0b00001001;
		_delay_us(time);

		PORTD = 0b00000101;
		_delay_us(time);
		
		PORTD = 0b00000110;
		_delay_us(time);
		
		PORTD = 0b00001010;
		_delay_us(time);
		
		number_of_steps--;
	}
	
	
	stop_motors();
}

void move_backward(uint16_t number_of_steps, uint16_t delta, uint32_t time) {
	uint32_t delta_um_normalized;
	delta_um_normalized = number_of_steps * DISPLACEMENT_PER_STEP;
	
	//error += (delta * 1000) - delta_um_normalized;
	current_coordinate -= round((number_of_steps * DISPLACEMENT_PER_STEP) / 1000.0f);	

	while(motor_status != MOTOR_STOPPED && number_of_steps > 0) {
		PORTD = 0b00001010;
		_delay_us(time);
		
		PORTD = 0b00000110;
		_delay_us(time);
		
		PORTD = 0b00000101;
		_delay_us(time);
		
		PORTD = 0b00001001;
		_delay_us(time);
		
		number_of_steps--;
	}
		
	//	
		
	stop_motors();
}

int16_t get_destination_from_data_frame() {
	int16_t result = 0;
	uint8_t lb;
	uint8_t hb;
	
	lb = buffer[SPI_LOWER_BYTE_DESTINATION];
	hb = buffer[SPI_HIGHER_BYTE_DESTINATION];

	result = (int16_t)(hb << 8) | (int16_t)lb;
	
	
	return result;
}

uint16_t get_time_from_data_frame() {
	uint16_t result = 0;
	uint8_t lower_byte;
	uint8_t higher_byte;
	
	lower_byte = buffer[SPI_LOWER_BYTE_TIME];
	higher_byte = buffer[SPI_HIGHER_BYTE_TIME];
	result = (uint16_t)(higher_byte << 8) | (uint16_t)lower_byte;
	
	return result;
}
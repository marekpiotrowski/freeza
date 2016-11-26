#include "defs.h"

#define F_CPU MASTER_F_CPU

#include "lib/usart.h"
#include "lib/spi.h"
#include <util/delay.h> 
#include <avr/interrupt.h>
#include "instruction.h"

typedef struct {
	int16_t x;
	int16_t y;
	int16_t z;
} Position;


void send_to_motor_driver(uint8_t motor, int16_t destination, uint16_t time);
void switch_motor(uint8_t motor);

void parse_usart_instruction(char* buffer, InstructionFrame* instruction);
int16_t get_coordinate_from_data_frame(char* buffer, uint8_t motor);

void process_instruction(InstructionFrame instruction, Position* position);
void process_G0_instruction(InstructionFrame instruction, Position* position);
void process_G1_instruction(InstructionFrame instruction, Position* position);
void process_M0_instruction();


ISR(USART_RX_vect) {
	uint8_t value;
	
	value = UDR0;

	Instruction_write(value);
}

int main() 
{ 
	uint8_t buffer[USART_INSTRUCTION_BUFFER_SIZE];
	InstructionFrame instruction;
	Position position;
	uint8_t status;
	
	USART_init();
    SPI_init_m328p(); 
	sei();
	
	Instruction_set_idle();
	
    while(1)               
    { 
		switch(Instruction_get_status()) {
			case READING_DONE:
				Instruction_read(buffer);
				parse_usart_instruction(buffer, &instruction);				
				process_instruction(instruction, &position);			
				Instruction_set_idle();
				break;
		}
		
		status = SPI_send(STATUS_CHECK);
		if(status == 'X') {
			USART_send(status);
			_delay_ms(10);
			status = SPI_send(STATUS_CHECK);
			USART_send(status);
			_delay_ms(10);
			status = SPI_send(STATUS_CHECK);
			USART_send(status);
			_delay_ms(10);
		}
		
		_delay_ms(100);
    } 
	
    return 0; 
}

void send_to_motor_driver(uint8_t motor, int16_t destination, uint16_t time) {
	switch_motor(motor);
	
	SPI_send(destination > 0 ? TRUE : FALSE);		

	SPI_send(destination & LOWER_BYTE);

	SPI_send((destination & HIGHER_BYTE) >> 8);
	
	SPI_send(time & LOWER_BYTE);

	SPI_send((time & HIGHER_BYTE) >> 8);	
}

static void send_stop() {
	SPI_send(STOP);
	SPI_send(STOP);
	SPI_send(STOP);
	SPI_send(STOP);
	SPI_send(STOP);
}

void send_stop_to_motor_drivers() {
	switch_motor(MOTOR_X);
	send_stop();

	switch_motor(MOTOR_Y);
	send_stop();

	switch_motor(MOTOR_Z);
	send_stop();
}

void switch_motor(uint8_t motor) {
	PORTB = motor;
}

static short get_feed_from_data_frame(char* buffer) {
	uint8_t lower_byte;
	uint8_t higher_byte;
	short result;
	
	lower_byte = buffer[FEED_LOWER_BYTE];
	USART_send(lower_byte);
	higher_byte = buffer[FEED_HIGHER_BYTE];
	USART_send(higher_byte);
	
	result = (higher_byte << 8) | lower_byte;
	return result;
}

void parse_usart_instruction(char* buffer, InstructionFrame* instruction) {
	instruction->x = get_coordinate_from_data_frame(buffer, MOTOR_X);
	instruction->y = get_coordinate_from_data_frame(buffer, MOTOR_Y);
	instruction->z = get_coordinate_from_data_frame(buffer, MOTOR_Z);
	instruction->code = buffer[CODE_IDX];
	instruction->feed = get_feed_from_data_frame(buffer);
}

void process_instruction(InstructionFrame instruction, Position* position) {
	switch(instruction.code) {
		case G0: process_G0_instruction(instruction, position); break;
		case M0: process_M0_instruction(); break;
		case G1: process_G1_instruction(instruction, position); break; //TODO
	}
}

int16_t get_coordinate_from_data_frame(char* buffer, uint8_t motor) {
	int16_t result = 0;
	uint8_t lower_byte;
	uint8_t higher_byte;
	uint8_t greater_than_zero;
	uint8_t offset;
	
	switch(motor) {
		case MOTOR_X: offset = USART_X_DESTINATION; break;
		case MOTOR_Y: offset = USART_Y_DESTINATION; break;
		case MOTOR_Z: offset = USART_Z_DESTINATION; break;
	}

	lower_byte = buffer[offset];
	higher_byte = buffer[offset + 1];
	greater_than_zero = buffer[offset + 2];
	
	result = (higher_byte << 8) | lower_byte;
	
	return greater_than_zero == TRUE ? result : -result;
}

void process_G0_instruction(InstructionFrame instruction, Position* position) {
	if(instruction.x != UNDEFINED) {
		send_to_motor_driver(MOTOR_X, instruction.x, 0);
	}
	
	if(instruction.y != UNDEFINED)
		send_to_motor_driver(MOTOR_Y, instruction.y, 0);
	
	if(instruction.z != UNDEFINED)
		send_to_motor_driver(MOTOR_Z, instruction.z, 0);
}
void process_G1_instruction(InstructionFrame instruction, Position* position) {

}
void process_M0_instruction() {
	send_stop_to_motor_drivers();
}
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


ISR(USART_RX_vect) {
	uint8_t value;
	
	value = UDR0;

	Instruction_write(value);
}

int main() 
{ 
	uint8_t buffer[SPI_INSTRUCTION_BUFFER_SIZE];
	InstructionFrame instruction;
	Position position;
	
	USART_init();
    SPI_init_m328p(); 
	sei();
	
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
    } 
	
    return 0; 
}

void send_to_motor_driver(uint8_t motor, int16_t destination, uint16_t time) {
	USART_send(motor);

	switch_motor(motor);
	
	SPI_send(destination & LOWER_BYTE);
	
	SPI_send((destination & HIGHER_BYTE) >> 8);
	
	SPI_send(destination > 0 ? TRUE : FALSE);		
	
	SPI_send(time & LOWER_BYTE);

	SPI_send((time & HIGHER_BYTE) >> 8);	
}

void switch_motor(uint8_t motor) {
	PORTB = motor;
}

void parse_usart_instruction(char* buffer, InstructionFrame* instruction) {
	instruction->x = get_coordinate_from_data_frame(buffer, MOTOR_X);
	instruction->y = get_coordinate_from_data_frame(buffer, MOTOR_Y);
	instruction->z = get_coordinate_from_data_frame(buffer, MOTOR_Z);
	instruction->code = buffer[1];
	instruction->feed = 0;
}

void process_instruction(InstructionFrame instruction, Position* position) {
	switch(instruction.code) {
		case G0: process_G0_instruction(instruction, position); break;
		case G1: break; //TODO
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
	int16_t delta_x, delta_y, delta_z;

	if(instruction.x != UNDEFINED) {
		delta_x = instruction.x - position->x;
		send_to_motor_driver(MOTOR_X, delta_x, 0);
		position->x += delta_x;
	}
	if(instruction.y != UNDEFINED) {
		delta_y = instruction.y - position->y;
		send_to_motor_driver(MOTOR_Y, delta_y, 0);
		position->y += delta_y;
	}
		
	if(instruction.z != UNDEFINED) {
		delta_z = instruction.z - position->z;
		send_to_motor_driver(MOTOR_Z, delta_z, 0);
		position->z += delta_z;
	}
}
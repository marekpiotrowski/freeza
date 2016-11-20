#include "instruction.h"


typedef struct {
	uint8_t buffer[USART_INSTRUCTION_BUFFER_SIZE];
	uint8_t iterator;
	uint8_t status;
} Instruction;

static Instruction instruction = { .iterator = 0, .status = IDLE };


uint8_t Instruction_write(uint8_t data) {
	if(data == '[')
		instruction.status = READING;
	else if(data == ']')
		instruction.status = READING_DONE;
	else {
		if(instruction.status != READING)
			return;
		instruction.buffer[instruction.iterator] = data;
		instruction.iterator++;
	}
	return instruction.status;
}

void Instruction_read(uint8_t* buffer) {
	uint8_t i;
	for(i = 0; i < Instruction_get_size(); i++)
		buffer[i] = instruction.buffer[i];
}

uint8_t Instruction_get_size() {
	return instruction.iterator;
}

uint8_t Instruction_get_status() {
	return instruction.status;
}

void Instruction_set_idle() {
	instruction.status = IDLE;
	instruction.iterator = 0;
}

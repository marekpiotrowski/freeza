#include "instruction_parser.h"




static int validate_instruction(char* raw_instruction) {
	return TRUE; //TO IMPLEMENT
}

static void parse_instruction(char* raw_instruction, InstructionFrame* instruction) {
	//TO IMPLEMENT
	instruction->code = G0;
	instruction->x = 5;
	instruction->y = 0;
	instruction->z = 0;
	instruction->feed = 0;
}

static void get_sendable_instruction(InstructionFrame instruction, unsigned char * buf) {
	buf[0] = '[';
	
	buf[1] = instruction.code;
	
	buf[2] = instruction.x & LOWER_BYTE;
	buf[3] = (instruction.x & HIGHER_BYTE) >> 8;
	buf[4] = instruction.x > 0 ? TRUE : FALSE;
	
	buf[5] = instruction.y & LOWER_BYTE;
	buf[6] = (instruction.y & HIGHER_BYTE) >> 8;
	buf[7] = instruction.y > 0 ? TRUE : FALSE;
	
	buf[8] = instruction.z & LOWER_BYTE;
	buf[9] = (instruction.z & HIGHER_BYTE) >> 8;
	buf[10] = instruction.z > 0 ? TRUE : FALSE;
	
	buf[11] = ']';
}

int send_instruction(char* raw_instruction, HANDLE hSerial) {
	int is_instruction_valid;
	InstructionFrame instruction;
	unsigned char bytes_to_send[INSTRUCTION_SIZE];
	DWORD bytes_written;

	is_instruction_valid = validate_instruction(raw_instruction);
	
	if(is_instruction_valid == FALSE)
		return 1;
		
	parse_instruction(raw_instruction, &instruction);
	get_sendable_instruction(instruction, bytes_to_send);
	
	if(!WriteFile(hSerial, bytes_to_send, INSTRUCTION_SIZE, &bytes_written, NULL))
		return 2;
	
	return 0;
}


int send_instructions(char* raw_instruction, HANDLE hSerial) {
	//TO IMPLEMENT
	return 0;
}





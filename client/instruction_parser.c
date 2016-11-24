#include "instruction_parser.h"

static int get_coordinate_from_instruction(char coordinate, char* raw_instruction) {
	int result;
	
	char *coord = strchr(raw_instruction, coordinate);
	
	if(coord == NULL)
		return UNDEFINED;
		
	int coordinate_index = (int)(coord - raw_instruction);
	
	char current_character = '\0';
	char value[10];
	int i = 0;

	while(!isspace(raw_instruction[coordinate_index + 1 + i])) {
		current_character = raw_instruction[coordinate_index + 1 + i];
		value[i] = current_character;
		i++;
	}

	value[i] = '\0';
	
	result = atoi(value);
	return result;
}


static void parse_g0_instruction(char* raw_instruction, InstructionFrame* instruction) {
	int x, y, z;

	x = get_coordinate_from_instruction('X', raw_instruction);
	y = get_coordinate_from_instruction('Y', raw_instruction);
	z = get_coordinate_from_instruction('Z', raw_instruction);

	instruction->x = x;
	instruction->y = y;
	instruction->z = z;
	instruction->feed = 0;
	instruction->code = G0;
}

static void parse_m0_instruction(InstructionFrame* instruction) {
	instruction->x = 0;
	instruction->y = 0;
	instruction->z = 0;
	instruction->feed = 0;
	instruction->code = M0;
}

static void parse_g_type_instruction(char* raw_instruction, InstructionFrame* instruction) {
	switch(raw_instruction[1]) {
		case '0': parse_g0_instruction(raw_instruction, instruction); break;
	}
}
static void parse_m_type_instruction(char* raw_instruction, InstructionFrame* instruction) {
	switch(raw_instruction[1]) {
		case '0': parse_m0_instruction(instruction); break;
	}
}


static void parse_instruction(char* raw_instruction, InstructionFrame* instruction) {
	switch(raw_instruction[0]) {
		case 'G': parse_g_type_instruction(raw_instruction, instruction); break;
		case 'M': parse_m_type_instruction(raw_instruction, instruction); break;
	}
}

static int validate_instruction(char* raw_instruction) {
	return TRUE; //TO IMPLEMENT
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
	int i;
	is_instruction_valid = validate_instruction(raw_instruction);
	
	if(is_instruction_valid == FALSE)
		return 1;
		
	parse_instruction(raw_instruction, &instruction);
	get_sendable_instruction(instruction, bytes_to_send);
	
	for(i = 0; i < 12; i++)
		printf("%x ", bytes_to_send[i]);
	printf("\n");
	
	if(!WriteFile(hSerial, bytes_to_send, INSTRUCTION_SIZE, &bytes_written, NULL))
		return 2;

	return 0;
}


int send_instructions(char* raw_instruction, HANDLE hSerial) {
	//TO IMPLEMENT
	return 0;
}





#ifndef _INSTRUCTION_PARSER
#define _INSTRUCTION_PARSER

#include <windows.h>
#include "../defs.h"

#define INSTRUCTION_SIZE 12

typedef struct {
	short code;
	short x;
	short y;
	short z;
	short feed;
} InstructionFrame;

int send_instruction(char*, HANDLE);
int send_instructions(char*, HANDLE);

#endif
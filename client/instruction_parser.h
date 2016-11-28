#ifndef _INSTRUCTION_PARSER
#define _INSTRUCTION_PARSER

#include <windows.h>
#include <stdint.h>
#include <ctype.h>
#include <stdio.h>
#include <string.h>

#include "../defs.h"

#define INSTRUCTION_SIZE 14


int send_instruction(char*, HANDLE);
int send_instructions(char*, HANDLE);

#endif
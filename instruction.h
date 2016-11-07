#ifndef _INSTRUCTION_H
#define _INSTRUCTION_H

#include "defs.h"
#include <util/delay.h> 

#define READING 100
#define READING_DONE 101
#define IDLE 102


uint8_t Instruction_write(uint8_t);
uint8_t Instruction_get_size();
uint8_t Instruction_get_status();
void Instruction_set_idle();
void Instruction_read(uint8_t*);

#endif
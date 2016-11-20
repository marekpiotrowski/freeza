#ifndef _DEFS_H
#define _DEFS_H

#define MASTER_F_CPU 8000000UL
#define SLAVE_F_CPU 1000000UL

#define G0 1
#define G1 2
#define G2 3
#define M1 4


#define TRUE 1
#define FALSE 0

#define LOWER_BYTE 0x00FF
#define HIGHER_BYTE 0xFF00

#define DEBUG_MODE

#define MOTOR_X 1
#define MOTOR_Y 2
#define MOTOR_Z 4


#define SPI_LOWER_BYTE_DESTINATION 0
#define SPI_HIGHER_BYTE_DESTINATION 1
#define SPI_SIGN_DESTINATION 2

#define SPI_LOWER_BYTE_TIME 3
#define SPI_HIGHER_BYTE_TIME 4

#define USART_X_DESTINATION 2
#define USART_Y_DESTINATION 5
#define USART_Z_DESTINATION 8

#define SPI_INSTRUCTION_BUFFER_SIZE 10
#define USART_INSTRUCTION_BUFFER_SIZE 50

#define UNDEFINED 20000


typedef struct {
	short code;
	short x;
	short y;
	short z;
	short feed;
} InstructionFrame;


#endif
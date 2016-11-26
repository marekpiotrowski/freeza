#ifndef _SPI_H
#define _SPI_H

#include<avr/io.h>
#include "../defs.h"

void SPI_init_m328p(void);
void SPI_init_m16(void);
uint8_t SPI_send(uint8_t);

#endif
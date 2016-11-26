#include "spi.h"

void SPI_init_m328p() 
{ 
    DDRB = ( 1 << PB5 ) | ( 1 << PB3 ) | ( 1 << PB2 );              //MOSI, SCK, SS' as output
	SPCR = ( 1 << SPE ) | ( 1 << MSTR ) | ( 1 << SPR1 ) | ( 1 << SPR0 );  //Master, fosc / 128 
}   

void SPI_init_m16() 
{ 
    DDRB = ( 1 << PB6 );        //MISO as output  
    SPCR = ( 1 << SPE ) | (1 << SPIE) | ( 1 << SPR1 ) | ( 1 << SPR0 );
}  

uint8_t SPI_send(uint8_t byte) 
{ 
    SPDR = byte;                      
    while( ! bit_is_set( SPSR, SPIF ) );
	return SPDR;
}   
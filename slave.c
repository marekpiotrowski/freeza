#include "defs.h"
#include "lib/spi.h"
#include<avr/io.h> 
#include<avr/interrupt.h>


ISR(SPI_STC_vect)
{ 
    uint8_t bajt = SPDR;
    PORTD = bajt;
} 

int main() 
{ 
    DDRD = 0xFF; 
    PORTD = 0xFF; 
	
    SPI_init_m16(); 
	sei();
	
    while(1) 
    { 

    }
	
    return 0; 
}
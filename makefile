MASTER_MCU			= atmega328p
SLAVE_MCU			= atmega16
MASTER_MCU_AVRDUDE	= ATMEGA328P
SLAVE_MCU_AVRDUDE	= m16

LIBDIR = lib

m-master: master.hex
m-slave: slave.hex

# master build
master.hex: master.elf
	avr-objcopy -R .eeprom -O ihex master.elf master.hex
	
master.elf: master.o usart.o spi-m328p.o
	avr-gcc -mmcu=$(MASTER_MCU) master.o usart.o spi-m328p.o -o master.elf

master.o: master.c
	avr-gcc -mmcu=$(MASTER_MCU) -Os -c master.c
# end master build



# slave build
slave.hex: slave.elf
	avr-objcopy -R .eeprom -O ihex slave.elf slave.hex
	
slave.elf: slave.o spi-m16.o
	avr-gcc -mmcu=$(SLAVE_MCU) slave.o spi-m16.o -o slave.elf

slave.o: master.c
	avr-gcc -mmcu=$(SLAVE_MCU) -Os -c slave.c
# end slave build



# utils build
spi-m328p.o: $(LIBDIR)/spi.c
	avr-gcc -mmcu=$(MASTER_MCU) -Os -c $(LIBDIR)/spi.c -o spi-m328p.o
	
spi-m16.o: $(LIBDIR)/spi.c
	avr-gcc -mmcu=$(SLAVE_MCU) -Os -c $(LIBDIR)/spi.c -o spi-m16.o
	
usart.o: $(LIBDIR)/usart.c
	avr-gcc -mmcu=$(MASTER_MCU) -Os -c $(LIBDIR)/usart.c
# end utils build

install-master:
	avrdude -c usbasp -p $(MASTER_MCU_AVRDUDE) -u -U flash:w:master.hex
install-slave:
	avrdude -c usbasp -p $(SLAVE_MCU_AVRDUDE) -u -U flash:w:slave.hex
clean:
	rm -f master.hex master.elf master.o spi-m16.o spi-m328p.o usart.o slave.o slave.hex slave.elf
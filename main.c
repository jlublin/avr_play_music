#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/cpufunc.h>
#include <avr/pgmspace.h>

#define F_CPU 8000000
#include <util/delay.h>

#include "usart.h"
#include "spi.h"
#include "sd.h"

#define FOSC 8000000

int main(void)
{	
	volatile uint8_t read_block;
	uint16_t next_block = 0;
	char *bytes;

	init_USART();
	init_SPI();
	init_SD();
	init_PWM(&read_block);
	sei();

	print_pgm_USART(PSTR("\nReset\nInitialized everything\n"));

	print_pgm_USART(PSTR("(main) Lock address: "));
	print_hex_word(&read_block);
	write_USART('\n');

	read_block = 1;

	while(1)
	{
		while(!read_block) {} // Spin-lock like request

//		print_pgm_USART(PSTR("Reading block: "));
//		print_hex_word(next_block);
//		write_USART('\n');

//		stop_PWM();

		bytes = read_block_SD(next_block++);
		read_block = 0;

//		if(bytes == 0)
//		{
//			print_pgm_USART(PSTR("Error reading block from SD card\n"));
//			while(1) {}
//		}

//		print_pgm_USART(PSTR("Start playing\n"));
		start_PWM(bytes);
	}

	return 0;
}


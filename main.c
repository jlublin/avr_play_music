/*
	Copyright 2014,2015 Joachim Lublin

	This program is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	Foobar is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with Foobar.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/cpufunc.h>
#include <avr/pgmspace.h>
#include <avr/wdt.h>

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

	init_Watchdog();
	wdt_reset();
	init_USART();
	wdt_reset();
	init_SPI();
	wdt_reset();
	init_SD();
	wdt_reset();
	init_PWM(&read_block);
	wdt_reset();
	sei();

//	print_pgm_USART(PSTR("\nReset\nInitialized everything\n"));

//	print_pgm_USART(PSTR("(main) Lock address: "));
//	print_hex_word(&read_block);
//	write_USART('\n');

	wdt_reset();
	bytes = read_block_SD(next_block++, 0);
	read_block = 0;
	start_PWM(bytes);

	wdt_reset();
	while(1)
	{
		while(!read_block) {} // Spin-lock like request

		if(next_block >= 3600)
			next_block = 0;

		bytes = read_block_SD(next_block++, (read_block - 1)*256);
		read_block = 0;
	}

	return 0;
}

void init_Watchdog()
{
	WDTCR = (1 << WDE) | (1 << WDP2) | (1 << WDP1) | (1 << WDP0);
}

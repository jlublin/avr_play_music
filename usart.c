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
#include <avr/pgmspace.h>

#define F_CPU 8000000

void init_USART()
{
	// Set baud rate
#define BAUD 9600
#include <util/setbaud.h>
	UBRRH = UBRRH_VALUE;
	UBRRL = UBRRL_VALUE;
#if USE_2X
	UCSRA |= (1 << U2X);
#else
	UCSRA &= ~(1 << U2X);
#endif
	// Set Format
	UCSRC = (1 << URSEL) | (3 << UCSZ0);
	// Enable Tx
	UCSRB = (1 << TXEN);
}

void write_USART(unsigned char data)
{
	while(!(UCSRA & (1 << UDRE)))
		{}
	UDR = data;
}

void print_USART(char *str)
{
	for(;*str; str++)
		write_USART(*str);
}

void print_pgm_USART(char *str)
{
	char data;

	do
	{
		data = pgm_read_byte(str++);
		write_USART(data);
	} while(data);
}

void print_hex(uint8_t val)
{
	uint8_t v;
	if((v = (val & 0xF)) < 10)
		write_USART(v + '0');
	else
		write_USART(v - 10 + 'a');
}

void print_hex_byte(uint8_t val)
{
	char v;
	write_USART('0');
	write_USART('x');

	print_hex(val >> 4);
	print_hex(val);

}

void print_hex_word(uint16_t val)
{
	char v;
	write_USART('0');
	write_USART('x');

	print_hex(val >> 12);
	print_hex(val >> 8);
	print_hex(val >> 4);
	print_hex(val);

}



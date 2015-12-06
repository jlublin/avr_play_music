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

#include "spi.h"

#define CS		PB2
#define MOSI	PB3
#define MISO	PB4
#define SCLK	PB5

void init_SPI()
{
	// Set outputs and inputs
	DDRB |=  (1 << MOSI) | (1 << SCLK) | (1 << CS);
	DDRB &= ~(1 << MISO);

	// Config SPI enabled, master, f_clk/16, not doubled speed
	SPCR = (1 << SPE) | (1 << MSTR) | (1 << SPR0);
	SPSR &= ~(1 << SPI2X);

	set_cs(1);
}

uint8_t send_byte_SPI(uint8_t data)
{
	SPDR = data;
	while(!(SPSR & (1 << SPIF)))
		{}
	return SPDR;
}

void set_cs(uint8_t cs)
{
	if(cs)
		PORTB |= (1 << CS);
	else
		PORTB &= ~(1 << CS);
}


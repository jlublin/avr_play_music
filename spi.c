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


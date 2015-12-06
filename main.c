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

	print_pgm_USART(PSTR("\nReset\nInitialized everything\n"));

	print_pgm_USART(PSTR("(main) Lock address: "));
	print_hex_word(&read_block);
	write_USART('\n');

	wdt_reset();
	bytes = read_block_SD(next_block++, 0);
	read_block = 0;
	start_PWM(bytes);

	wdt_reset();
	while(1)
	{
		while(!read_block) {} // Spin-lock like request

		bytes = read_block_SD(next_block++, (read_block - 1)*256);
		read_block = 0;
	}

	return 0;
}

void init_Watchdog()
{
	WDTCR = (1 << WDE) | (1 << WDP2) | (1 << WDP1) | (1 << WDP0);
}

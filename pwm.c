#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>

uint8_t counter;
uint16_t sample_count;
char *sound_data;
volatile uint8_t *read_block_request;

ISR(TIMER0_OVF_vect)
{
	if(counter == 0)
	{
		OCR1A = sound_data[sample_count];
		sample_count++;

		if(sample_count == 256)
			*read_block_request = 1;	// Read block to first block buffer
										// and continue playing from second

		if(sample_count >= 512)
		{
			*read_block_request = 2;	// Read block to second block buffer
			sample_count = 0;			// and start playing from first
		}
	}

	counter++;
	if(counter >= 4)
		counter = 0;

}

void init_PWM(uint8_t *rb_request)
{
	counter = 0;
	sample_count = 0;
	read_block_request = rb_request;


	/* Timer 1A setup
	Clear output on compare, set on overflow
	Prescaler: clk/1 (8 MHz) (/256 -> 31 kHz PWM)
	Init counter: 0
	*/
	DDRB |= (1 << PB1); // OC1A output
	TCCR1A = (1 << COM1A1) | (1 << WGM10);
	TCCR1B = (1 << WGM12) | (1 << CS10);
	OCR1A = 0x80;

	/* Timer0 setup
	Prescaler: clk/1 (8 MHz)
	*/
	TCCR0 = (1 << CS00);
}

void start_PWM(char *snd_data)
{
	sound_data = snd_data;

	/*
	Init counter: 0
	Enable timer0 overflow interrupt
	*/
//	TCNT0 = 0;
	TIMSK |= (1 << TOIE0);
}

void stop_PWM()
{
	/* Disable timer0 overflow interrupt */
	TIMSK &= ~(1 << TOIE0);
}

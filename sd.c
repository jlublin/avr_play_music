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

#include "sd.h"
#include "spi.h"

#define GO_IDLE_STATE            0 | 0x40
#define SEND_OP_COND             1 | 0x40
#define SEND_CSD                 9 | 0x40
#define STOP_TRANSMISSION        12 | 0x40
#define SEND_STATUS              13 | 0x40
#define SET_BLOCK_LEN            16 | 0x40
#define READ_SINGLE_BLOCK        17 | 0x40
#define READ_MULTIPLE_BLOCKS     18 | 0x40
#define WRITE_SINGLE_BLOCK       24 | 0x40
#define WRITE_MULTIPLE_BLOCKS    25 | 0x40
#define ERASE_BLOCK_START_ADDR   32 | 0x40
#define ERASE_BLOCK_END_ADDR     33 | 0x40
#define ERASE_SELECTED_BLOCKS    38 | 0x40
#define CRC_ON_OFF               59 | 0x40

uint8_t send_command_SD(uint8_t cmd, unsigned long arg)
{
	set_cs(0);

	send_byte_SPI(cmd);
	send_byte_SPI(arg >> 24);
	send_byte_SPI(arg >> 16);
	send_byte_SPI(arg >> 8);
	send_byte_SPI(arg);
	send_byte_SPI(0x95);

	uint8_t rec;
	while((rec = send_byte_SPI(0xFF)) == 0xFF)
		{}

	send_byte_SPI(0xFF);
	set_cs(1);

	return rec;
}

void init_SD()
{
	uint8_t i;

	set_cs(0);
	
	for(i = 0; i < 10; i++)
		send_byte_SPI(0xFF);

	while(send_command_SD(GO_IDLE_STATE, 0) != 1)
		{}

	set_cs(1);

	send_byte_SPI(0xFF);
	send_byte_SPI(0xFF);

	while(send_command_SD(SEND_OP_COND, 0))
		{}

	send_command_SD(CRC_ON_OFF, 0); // Turn it off (already off?)
	send_command_SD(SET_BLOCK_LEN, 256);	// 256 allows two blocks to fit in
											// SRAM along the other stuff

	
	SPCR &= ~(1 << SPR0);	// f_osc/4
}

char sd_block[512];	// 2*256

char *read_block_SD(unsigned long block_nr, uint16_t offset)
{
	uint16_t i;

	if(send_command_SD(READ_SINGLE_BLOCK, block_nr<<8))
		return 0;

	set_cs(0);

	while(send_byte_SPI(0xFF) != 0xFE)
		{}

	for(i = 0; i < 256; i++)
		sd_block[offset + i] = send_byte_SPI(0xFF);

	// 16-bit CRC
	send_byte_SPI(0xFF);
	send_byte_SPI(0xFF);

	send_byte_SPI(0xFF);
	set_cs(1);

	return sd_block;
}


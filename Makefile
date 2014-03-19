
NAME = play_music_sd
MCU_TARGET = atmega8

CC = avr-gcc
CFLAGS = -g -mmcu=$(MCU_TARGET) -Os
LDFLAGS = -Wl,-Map,$(NAME).map

OBJ = main.o usart.o spi.o sd.o pwm.o

OBJCOPY = avr-objcopy

.PHONY: install all clean

all: $(NAME).elf

clean:
	rm *.o *.map *.elf *.hex

$(NAME).elf: $(OBJ)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^

%.o: %.c
	$(CC) $(CFLAGS) -c $<

hex: $(NAME).elf
#	$(OBJCOPY) -j .text -j .data -O ihex $< $(NAME).hex
	$(OBJCOPY) -j .text -O ihex $< $(NAME).hex

install: hex
	sudo avrdude -c avrispmkii -p m8 -P usb -B 1 -U lfuse:w:0xe4:m -U hfuse:w:0xd7:m -U flash:w:$(NAME).hex
#	sudo avrdude -c avrispmkii -p m8 -P usb -U flash:w:$(NAME).hex

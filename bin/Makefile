#Nicholas Shanahan (2016)

F_CPU := 1000000
CC := avr-gcc
MMCU := atmega1284p
CFLAGS := -g -O2 -Wall -Wextra -std=gnu99

EXE := test
HEX := $(EXE).hex

MAIN := ../temperature_app.c

default: $(EXE)

all: program

INCS = -I ../drivers/ds18b20 \
	   -I ../drivers/owi \
	   -I ../drivers/lcd

SRCS = ../drivers/ds18b20 \
	   ../drivers/owi \
	   ../drivers/lcd

#VPATH will extract dependencies from the
#listed source directories automatically	   
VPATH = $(SRCS)

OBJS = owi.o \
	   owi_crc.o \
	   DS18B20.o \
	   lcd_driver.o
	   
%.o:%.c
	$(CC) -c $(CFLAGS) -mmcu=$(MMCU) -D_FCPU=$(F_CPU) $(INCS) $<

#Builds the target specified by the EXE variable	
$(EXE): $(OBJS) $(MAIN)
	$(CC) $(CFLAGS) -mmcu=$(MMCU) -D_FCPU=$(F_CPU) $(OBJS) $(INCS) $(MAIN) -o $(EXE)

#Builds hex file specified by the HEX variable	
$(HEX): $(EXE)
	avr-strip $(EXE)
	avr-objcopy -R .eeprom -O ihex $(EXE) $(HEX)
	
#Writes the hex file to the microncontroller flash memory
program: $(HEX)
	sudo avrdude -p m1284p -c buspirate -P /dev/ttyUSB0 -U flash:w:$(HEX)
	
#Removes the executable, hex file, and object files from PWD	
clean:
	rm $(EXE) $(HEX) $(OBJS)

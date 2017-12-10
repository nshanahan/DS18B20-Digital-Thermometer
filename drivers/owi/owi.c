/***************************************************************
 * Nicholas Shanahan (2016)
 *
 * DESCRIPTION:
 *  - Driver for Dallas 1-Wire Interface Bus. This is a purely
 *    software implementation and unlike other OWI (One-Wire Interface)
 *    drivers does not rely on UART hardware. All OWI pins bus must
 *    belong to the same port. The OWI port location is configurable at
 *    compile time. Intended for use with an AVR microcontroller.
 *    Assumes EXTERNAL pull-up resistors.
 *
 *    Developed in accordance with AVR318: Dallas 1-Wire Master text.
 *    http://www.atmel.com/images/doc2579.pdf
 *
 **************************************************************/

 /**************************************************************
					 Includes
 ***************************************************************/
#include "owi.h"
#include "owi_delay.h"
#include "owi_cmds.h"
#include <avr/io.h>
#include <stdint.h>
#include <stdbool.h>
#include <util/delay.h>
#include <avr/interrupt.h>

 /**************************************************************
					 Defines
 ***************************************************************/
#define F_CPU 1000000UL

#define CONCAT(A, B) (A##B)
#define DDR(letter) CONCAT(DDR, letter)	
#define PIN(letter) CONCAT(PIN, letter)
#define PORT(letter) CONCAT(PORT, letter)

 /** OWI Bit-Level Macros **/
 //Macro that releases control of the OWI bus
#define RELEASE_BUS(OWI_PIN) \
  (DDR(OWI_BUS) &= ~(1 << OWI_PIN))

//Macro to drive the OWI bus low
#define DRIVE_BUS_LOW(OWI_PIN) \
  DDR(OWI_BUS) |= (1 << OWI_PIN); \
  PORT(OWI_BUS) &= ~(1 << OWI_PIN);

//Macro to read the OWI bus value
#define READ_BUS_VALUE(OWI_PIN) \
  (PIN(OWI_BUS) & (1 << OWI_PIN))

#define ROM_SIZE 8
#define ROM_SEARCH_FAILED 0xFF

/**************************************************************
				 Function Prototypes
***************************************************************/
/*
 * Inline these functions to remove function call overhead.
 * The Dallas One-Wire protocol is time sensitive and is being
 * implemented in software rather than hardware.
 */
static inline void write_bit1(uint8_t pin) __attribute__((always_inline));
static inline void write_bit0(uint8_t pin) __attribute__((always_inline));
//Not static but intended to be pseudo-private
inline bool owi_read_bit(uint8_t pin) __attribute__((always_inline));

/** Bit-level Functions **/

/*Write a 1 bit on the specified OWI bus locations*/
static void write_bit1(uint8_t pin)
{
	cli();
	DRIVE_BUS_LOW(pin);
	_delay_us(OWI_DELAY_US_A);
	RELEASE_BUS(pin);
	_delay_us(OWI_DELAY_US_B);
	sei();
}

/*Write a 0 bit on the specified OWI bus locations*/
static void write_bit0(uint8_t pin)
{
	cli();
	DRIVE_BUS_LOW(pin);
	_delay_us(OWI_DELAY_US_C);
	RELEASE_BUS(pin);
	_delay_us(OWI_DELAY_US_D);
	sei();
}

/*Read a bit from the OWI bus*/
bool owi_read_bit(uint8_t pin)
{
	cli();
	DRIVE_BUS_LOW(pin);
	_delay_us(OWI_DELAY_US_A);
	RELEASE_BUS(pin);
	_delay_us(OWI_DELAY_US_E);

	//Read the bus value
	bool val = READ_BUS_VALUE(pin);

	_delay_us(OWI_DELAY_US_F);
	sei();

	return val;
}

/** OWI API **/

/*See owi.h for details*/
void owi_init(uint8_t pin)
{
	RELEASE_BUS(pin);
	_delay_us(OWI_DELAY_US_H);
}

/*See owi.h for details*/
bool owi_is_device_present(uint8_t pin)
{
	bool present = false;

	cli();
	DRIVE_BUS_LOW(pin);
	_delay_us(OWI_DELAY_US_H);
	RELEASE_BUS(pin);
	_delay_us(OWI_DELAY_US_I);

	/*
	 *Pin is pulled high by pull-up resistor(s)
	 *if no device is present. Otherwise, held low.
	 */
	if (!(PIN(OWI_BUS) & _BV(pin)))
	{
		present = true;
	}

	_delay_us(OWI_DELAY_US_J);
	sei();

	return present;
}

/*See owi.h for details*/
void owi_send(uint8_t data, uint8_t pin)
{
	bool lsb;

	for (uint8_t i = 0; i < 8; i++)
	{
		lsb = data & 0x01;
		//Write a 0 or a 1 to OWI bus
		(lsb) ? write_bit1(pin) : write_bit0(pin);
		//Shift data to isolate next bit
		data >>= 1;
	}
}

/*Reads a byte of data from the OWI bus*/
uint8_t owi_receive(uint8_t pin)
{
	uint8_t data = 0;

	for (uint8_t i = 0; i < 8; i++)
	{
		//Set bit position if a 1 is read from bus
		if (owi_read_bit(pin))
		{
			data |= _BV(i);
		}
	}

	return data;
}

/** ROM Command Functions **/

/*See owi.h for details*/
void owi_skip_rom(uint8_t pin)
{
	owi_send(SKIP_ROM_CMD, pin);
}

/*See owi.h for details*/
void owi_read_rom(uint8_t *rom, uint8_t pin)
{
	//Issue command to read ROM
	owi_send(READ_ROM_CMD, pin);

	/*
	 *read from from slave
	 *least significant byte sent first
	 */
	for (int8_t i = ROM_SIZE - 1; i >= 0; i--)
	{
		rom[i] = owi_receive(pin);
	}
}

/*See owi.h for details*/
void owi_match_rom(uint8_t *rom, uint8_t pin)
{
	//Issue command to match ROM
	owi_send(MATCH_ROM_CMD, pin);

	//Write ROM value on OWI bus
	for (int8_t i = ROM_SIZE - 1; i >= 0; i--)
	{
		owi_send(rom[i], pin);
	}
}

/*See owi.h for details*/
uint8_t owi_search_rom(uint8_t *bit_pattern, uint8_t last_deviation, uint8_t pin)
{
	bool bit1 = 0;
	bool bit2 = 0;

	uint8_t new_deviation = 0;
	uint8_t bit_index = 0;
	uint8_t byte_index = 0;
	uint8_t current_index = 1;

	/*issue Search ROM command*/
	owi_send(SEARCH_ROM_CMD, pin);

	while (byte_index < 8)
	{
		while (bit_index < 8)
		{
			bit1 = owi_read_bit(pin);
			bit2 = owi_read_bit(pin);

			/*No ROM discovered, seach failed*/
			if (bit1 && bit2)
			{
				return ROM_SEARCH_FAILED;
			}

			/*Devices discovered w/ same first bit of ROM*/
			else if (bit1 != bit2)
			{
				/*Set bit to a one*/
				if (bit1)
				{
					bit_pattern[byte_index] |= _BV(bit_index);
				}

				/*Set bit to a zero*/
				else
				{
					bit_pattern[byte_index] &= ~_BV(bit_index);
				}
			}

			/*Devices discovered w/ different first bit of ROM*/
			else
			{
				if (current_index == last_deviation)
				{
					bit_pattern[byte_index] |= _BV(bit_index);
				}

				else if (current_index > last_deviation)
				{
					bit_pattern[byte_index] &= ~_BV(bit_index);
					new_deviation = current_index;
				}

				else if (!(bit_pattern[byte_index] & _BV(bit_index)))
				{
					new_deviation = current_index;
				}

				else continue;
			}

			/*Write the bit to the OWI bus*/
			(bit_pattern[byte_index] & _BV(bit_index)) ? write_bit1(pin) : write_bit0(pin);

			/*Move to next bit in the current byte*/
			bit_index++;

			/*Increment overall bit position tracker*/
			current_index++;
		}

		bit_index = 0;
		byte_index++;
	}

	return new_deviation;
}
/* End of owi.c */

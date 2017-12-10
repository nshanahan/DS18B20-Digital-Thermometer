/***************************************************************
 * Nicholas Shanahan (2016)
 *
 * DESCRIPTION:
 *  - Driver for the Dallas Semiconductors DS18B20 Digital Thermometer.
 *    A software implemented 1-Wire Bus drive to used to interface
 *    the DS18B20. This driver is intended for an AVR microcontroller.
 *    The DS18B20 may be connected any port on the defined OWI bus.
 *    The factory default 12-bits of precision is utilized.
 *
 **************************************************************/

 /**************************************************************
				 Includes
 ***************************************************************/
#include "DS18B20.h"
#include "owi.h"
#include "owi_crc.h"
#include <avr/io.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdfix.h>
#include <stdlib.h>

 /**************************************************************
					Defines
 ***************************************************************/
 /*Memory Commands*/
#define WRITE_SCRATCHPAD_CMD 0x4E
#define READ_SCRATCHPAD_CMD  0xBE
#define CONVERT_TEMP_CMD     0x44

/*Scratchpad Macros*/
//Size of Scratchpad Memory in Bytes
#define SCRATCHPAD_SIZE 9
//Scratchpad Address that Contains CRC Value
#define SCRATCHPAD_CRC 8
//Scratchpad CRC Success Code
#define SCRATCHPAD_CRC_PASS 1
//Scratchpad CRC Failure Code
#define SCRATCHPAD_CRC_FAIL 0
//Scratchpad Read Success Code
#define SCRATCHPAD_READ_PASS 1
//Scratchpad Read Error Code
#define SCRATCHPAD_READ_ERR 0

/*Positions of Temperature Bytes in Scratchpad*/
#define TEMP_HI 1
#define TEMP_LO 0

/*Precision of Temperature Reading*/
#define PRECISION 0.0625F

/**************************************************************
			Function Prototypes
***************************************************************/
static bool scratchpad_crc(uint8_t *scratchpad);
static bool read_scratchpad(ds18b20_dev_t *dev, uint8_t *scratchpad);
/*Extern'd from owi.c*/
extern bool owi_read_bit(uint8_t pin);

/*Computes the CRC8 of the Scratchpad data and compares to CRC value read*/
static bool scratchpad_crc(uint8_t *scratchpad)
{
	uint8_t crc = 0;

	//Compute the CRC of scratchpad data
	for (uint8_t i = 0; i < SCRATCHPAD_CRC; i++)
	{
		crc = crc8(scratchpad[i], crc);
	}

	//Return success if computed CRC matches read CRC
	return (crc == scratchpad[SCRATCHPAD_CRC]) ? 
		SCRATCHPAD_CRC_PASS : 
		SCRATCHPAD_CRC_FAIL;
}

/*Reads Scratchpad and calculates the CRC8 of received data*/
static bool read_scratchpad(ds18b20_dev_t *dev, uint8_t *scratchpad)
{
	bool rs;

	//Verify OWI device is on bus
	if (owi_is_device_present(dev->pin))
	{
		//Address the DS18B20 sensor
		owi_match_rom(dev->rom, dev->pin);
		//Send Read Scratchpad command
		owi_send(READ_SCRATCHPAD_CMD, dev->pin);

		//Read the entire scratchpad
		for (uint8_t i = 0; i < SCRATCHPAD_SIZE; i++)
		{
			scratchpad[i] = owi_receive(dev->pin);
		}

		//Compute the CRC8 of the scratchpad data
		bool crc_result = scratchpad_crc(scratchpad);

		//Return read error if CRCs did not match
		rs = (crc_result != SCRATCHPAD_CRC_PASS) ?
			SCRATCHPAD_READ_ERR :
			SCRATCHPAD_READ_PASS;
	}

	else
	{
		rs = SCRATCHPAD_READ_ERR;
	}

	return rs;
}

/** DS18B20 API **/

/*See DS18B20.h for details*/
bool ds18b20_init(ds18b20_dev_t *dev, uint8_t pin)
{
	bool rs;

	if (dev != NULL)
	{
		dev->temp = 0;
		dev->pin = pin;
		owi_init(pin);
		rs = DS18B20_INIT_PASS;
	}

	else
	{
		rs = DS18B20_INIT_FAIL;
	}

	return rs;
}

/*See DS18B20.h for details*/
void ds18b20_get_rom(ds18b20_dev_t *dev)
{
	if (owi_is_device_present(dev->pin))
	{
		//Read ROM from DS18B20
		owi_read_rom(dev->rom, dev->pin);
	}
}

/*See DS18B20.h for details*/
bool ds18b20_read_temp(ds18b20_dev_t *dev)
{
	uint8_t scratchpad[SCRATCHPAD_SIZE] = { 0 };
	uint16_t raw_temp = 0;
	bool rs, err;

	if (owi_is_device_present(dev->pin))
	{
		//Address the DS18B20 sensor
		owi_match_rom(dev->rom, dev->pin);
		//Send Convert Temperature memory command
		owi_send(CONVERT_TEMP_CMD, dev->pin);

		do {
			//Wait for conversion to complete
		} while (!owi_read_bit(dev->pin));

		rs = read_scratchpad(dev, scratchpad);

		//Return error code if temp could not be read
		if (rs == SCRATCHPAD_READ_ERR)
		{
			err = DS18B20_TEMP_READ_ERR;
		}

		else
		{
			err = DS18B20_TEMP_READ_PASS;
			//Complete transaction
			owi_is_device_present(dev->pin);
			//Set temperature value
			raw_temp = ((scratchpad[TEMP_HI] << 8) | scratchpad[TEMP_LO]);
			//Convert to readable format (in Celsius)
			dev->temp = ((float)raw_temp * PRECISION);
		}
	}

	else
	{
		err = DS18B20_TEMP_READ_ERR;
	}

	return err;
}
/* End of DS18B20.h */

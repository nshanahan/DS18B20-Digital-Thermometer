#define F_CPU 1000000UL

/**************************************************************
							Includes
***************************************************************/
#include "DS18B20.h"
#include "lcd_driver.h"
#include <stdint.h>
#include <stdbool.h>
#include <stdfix.h>
#include <avr/io.h>
#include <util/delay.h>
#include <stdlib.h>

/**************************************************************
							Defines
***************************************************************/
#define DS18B20_PIN 2

/**************************************************************
						Test Program
***************************************************************/
int main()
{
	bool rs = 0;
	char temp[9];

	//init temp sensor
	ds18b20_dev_t dev = {
		.pin = DS18B20_PIN,
		.rom = {0},
		.temp = 0
	};

	//Start the LCD
	initialize_LCD_driver();
	//Setup DS18B20 Thermometer
	rs = ds18b20_init(&dev, DS18B20_PIN);

	if (rs == DS18B20_INIT_PASS)
	{
		//Assumes only one OWI device
		ds18b20_get_rom(&dev);

		//Read temperature ~ every 2 seconds
		while (1)
		{
			rs = ds18b20_read_temp(&dev);
			//Check that read was successful
			if (rs == DS18B20_TEMP_READ_PASS)
			{
				lcd_erase();
				dtostrf(dev.temp, 7, 4, temp);
				temp[8] = '\0';
				lcd_puts(temp);
				_delay_ms(2000);
			}
		}
	}

	return 0;
}

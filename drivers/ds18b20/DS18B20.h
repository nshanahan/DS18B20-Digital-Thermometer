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

#ifndef DS18B20_H_
#define DS18B20_H_

#ifdef __cplusplus
extern "C" {
#endif

/**************************************************************
			   Includes
***************************************************************/
#include <stdint.h>
#include <stdbool.h>
#include <stdfix.h>

/**************************************************************
			   Defines
***************************************************************/
/*DS18B20 Status Codes*/
#define DS18B20_INIT_PASS      0
#define DS18B20_INIT_FAIL      1
#define DS18B20_TEMP_READ_PASS 0
#define DS18B20_TEMP_READ_ERR  1

/**************************************************************
			   Typedefs
***************************************************************/
typedef struct {
  uint8_t  pin;
  uint8_t  rom[8];
  float	   temp;
} ds18b20_dev_t;

/***************************************************************
 *
 * DESCRIPTION:
 *  - Initializes the OWI Bus and subsequently the DS18B20 bus
 *    position defined by the user.
 *
 **************************************************************/
bool ds18b20_init(ds18b20_dev_t *dev, uint8_t pin);

/***************************************************************
 *
 * DESCRIPTION:
 *  - Reads the ROM ID of the DS18B20 device. Assumes a single
 *    DS18B20 device is connected to the OWI Bus. The user must
 *    pass a pointer to the 8-byte buffer where the ROM will be
 *    stored.
 *
 **************************************************************/
void ds18b20_get_rom(ds18b20_dev_t *dev);

/***************************************************************
*
* DESCRIPTION:
*  - Reads the temperature value in degrees Celsius from the
*    DS18B20 thermometer. The user must pass a pointer to accum
*    variable (fixed-point) where the temperature should be placed
*    as well as the ROM ID of the sensor.
*
*    If the device is read successfully, the functin will return
*    the code "DS18B20_TEMP_READ_PASS" else if will return
*    "DS18B20_TEMP_READ_ERR".
*
**************************************************************/
bool ds18b20_read_temp(ds18b20_dev_t *dev);

#ifdef __cplusplus
}
#endif

#endif
/* End of DS18B20.h */

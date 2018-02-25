/***************************************************************
 * @file ds18b20.h
 *
 * @par Nicholas Shanahan (2016)
 *
 * @brief Driver for the Dallas Semiconductors DS18B20 Digital Thermometer.
 * The DS18B20 may be connected any port on the defined OWI bus.
 * The factory default 12-bits of precision is utilized.
 *
 **************************************************************/

#ifndef _DS18B20_H
#define _DS18B20_H

#ifdef __cplusplus
extern "C" {
#endif

/**************************************************************
                          Includes
***************************************************************/
#include <stdint.h>
#include <stdbool.h>

/**************************************************************
                           Macros
***************************************************************/

/**************************************************************
                          Typedefs
***************************************************************/
typedef struct {
  uint8_t  pin;
  uint8_t  rom[8];
  float    temp;
} ds18b20_dev_t;

/**************************************************************
                       Public Functions
***************************************************************/
/*!
 * @brief Initializes DS18B20 device and OWI on defined pin.
 * @param[in] dev Pointer to DS18B20 device structure.
 * @param[in] pin OWI bus pin where device is connected.
 * @return bool
 */
bool ds18b20_init(ds18b20_dev_t *dev, uint8_t pin);

/*!
 * @brief Reads the ROM ID of the DS18B20 device. Assumes a single
 * DS18B20 device is connected to the OWI Bus. The ROM is read into
 * buffer in the device structure.
 * @param[in] dev Pointer to DS18B20 device structure.
 * @return bool
 */
bool ds18b20_get_rom(ds18b20_dev_t *dev);

/*!
* @brief Reads the temperature value in degrees Celsius from the
* DS18B20 thermometer. Temperature is read into the temperature
* floating point variable in the device structure.
* @param[in] dev Pointer to device structure.
* @return bool
*/
bool ds18b20_read_temp(ds18b20_dev_t *dev);

#ifdef __cplusplus
}
#endif

#endif /* _DS18B20_H */

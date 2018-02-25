/***************************************************************
 * @file ds18b20.c
 *
 * @par Nicholas Shanahan (2016)
 *
 * @brief Driver for the Dallas Semiconductors DS18B20 Digital Thermometer.
 * A software implemented 1-Wire Bus drive to used to interface
 * the DS18B20. This driver is intended for an AVR microcontroller.
 * The DS18B20 may be connected any port on the defined OWI bus.
 * The factory default 12-bits of precision is utilized.
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
#include <stdlib.h>

/**************************************************************
                          Macros
***************************************************************/
//memory Commands
#define WRITE_SCRATCHPAD_CMD 0x4E
#define READ_SCRATCHPAD_CMD  0xBE
#define CONVERT_TEMP_CMD     0x44

#define SCRATCHPAD_LEN_BYTES 9
#define EXPECTED_CRC_IDX 8
#define TEMP_HI_IDX 1
#define TEMP_LO_IDX 0

//precision of temperature reading
#define PRECISION 0.0625F

/**************************************************************
                    Private Function Prototypes
***************************************************************/
static bool scratchpad_crc(uint8_t *scratchpad);
static bool read_scratchpad(ds18b20_dev_t *dev, uint8_t *scratchpad);

/*!
 * @brief Computes the CRC8 of the Scratchpad
 * data and compares result to the expected value.
 * @param[in] scratchpad Pointer to scratchpad data.
 * @return bool
 */
static bool scratchpad_crc(uint8_t *scratchpad)
{
  bool err = false;
  uint8_t idx = 0;
  uint8_t crc = 0;
  
  //compute the CRC of scratchpad data
  for (idx = 0; idx < EXPECTED_CRC_IDX; idx++)
  {
    crc = crc8(scratchpad[idx], crc);
  }

  if (crc != scratchpad[EXPECTED_CRC_IDX])
  {
    err = true;
  }

  return err;
}

/*!
 * @brief Reads Scratchpad and calculates the CRC8 of received data.
 * @param[in] dev Pointer to DS18B20 device structure.
 * @param[out] scratchpad Buffer to hold scratchpad memory data.
 * @return bool
 */
static bool read_scratchpad(ds18b20_dev_t *dev, uint8_t *scratchpad)
{
  bool err = false;
  uint8_t idx = 0;
  
  //verify OWI device is on bus
  owi_detect_presence(dev->pin);
  //address the DS18B20 sensor
  owi_match_rom(dev->rom, dev->pin);
  //send Read Scratchpad command
  owi_send_byte(READ_SCRATCHPAD_CMD, dev->pin);
  
  //read the entire scratchpad memory
  for (idx = 0; idx < SCRATCHPAD_LEN_BYTES; idx++)
  {
    scratchpad[idx] = owi_recv_byte(dev->pin);
  }
  
  //compute the CRC8 of the scratchpad data
  err = scratchpad_crc(scratchpad);

  return err;
}

/**************************************************************
                    Public Functions
***************************************************************/
//See DS18B20.h
bool ds18b20_init(ds18b20_dev_t *dev, uint8_t pin)
{
  bool err = false;
  
  if (dev == NULL)
  {
    err = true;
  }
  
  if (!err)
  {
    dev->temp = 0;
    dev->pin = pin;
    owi_init(pin);
  }
  
  return err;
}

//See DS18B20.h
bool ds18b20_get_rom(ds18b20_dev_t *dev)
{
  bool err = false;
  bool present;
  
  //check if DS18B20 device is present
  present = owi_detect_presence(dev->pin);

  if (!present)
  {
    err = true;
  }
  
  if (!err)
  {
    owi_read_rom(dev->rom, dev->pin);
  }

  return err;
}

//See DS18B20.h
bool ds18b20_read_temp(ds18b20_dev_t *dev)
{
  bool err = false;
  uint16_t raw_temp = 0;
  uint8_t scratchpad[SCRATCHPAD_LEN_BYTES];
  
  owi_detect_presence(dev->pin);
  //address the DS18B20 sensor
  owi_match_rom(dev->rom, dev->pin);
  //send Convert Temperature memory command
  owi_send_byte(CONVERT_TEMP_CMD, dev->pin);
  //wait for conversion to complete
  while (owi_is_busy(dev->pin));

  err = read_scratchpad(dev, scratchpad);
  
  if (!err)
  {
    //complete transaction
    owi_detect_presence(dev->pin);
    //format temperature data
    raw_temp = (scratchpad[TEMP_HI_IDX] << 8) | scratchpad[TEMP_LO_IDX];
    //convert to readable format (in Celsius)
    dev->temp = ((float)raw_temp * PRECISION);
  }
  
  return err;
}

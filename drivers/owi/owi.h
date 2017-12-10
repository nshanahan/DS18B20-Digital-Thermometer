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
 
#ifndef OWI_H_
#define OWI_H_

#ifdef __cplusplus
extern "C" {
#endif

/**************************************************************
			Includes
***************************************************************/
#include <stdint.h>
#include <stdbool.h>

/**************************************************************
			Defines
***************************************************************/
/* 
 * Pre-define port for the time being.
 * Consider making this configurable
 * somehow in the future 
 */
#define OWI_BUS D

/**************************************************************
		    Function Prototypes
***************************************************************/

/***************************************************************
 *
 * DESCRIPTION:
 *  - Initializes the specified OWI port locations by configuring
 *    them as input pin. The user should declare a mask that contains
 *    a one in the bit position corresponding to the port location
 *    of the OWI device. Does NOT enable internal pull-up resistors.
 *
 **************************************************************/
void owi_init(uint8_t pin);

/***************************************************************
 *
 * DESCRIPTION:
 *  - Issues a reset to the specified OWI bus pins and determines
 *    if an OWI slave device is present. Returns a one if the slave 
 *    is present and a zero if the slave is not present.
 *
 **************************************************************/
bool owi_is_device_present(uint8_t pin);

/***************************************************************
 *
 * DESCRIPTION:
 *  - Writes a byte of data to the specified OWI bus pins.
 *
 **************************************************************/
void owi_send(uint8_t data, uint8_t pin);

/***************************************************************
 *
 * DESCRIPTION:
 *  - Reads a byte of data from the specified OWI bus pin.
 *
 **************************************************************/
uint8_t owi_receive(uint8_t pin);

/***************************************************************
 *
 * DESCRIPTION:
 *  - Issues SKIP ROM command to the specified OWI bus pins. Can 
 *    only be used when sending data to a slave device. Does not 
 *    address a specific slave.
 *
 **************************************************************/
void owi_skip_rom(uint8_t pin);

/***************************************************************
 *
 * DESCRIPTION:
 *  - Reads the unique 64-bit identifier of a OWI slave device.
 *    The user passes a pointer to an 8-byte buffer where the
 *    identifier will be stored.
 *
 **************************************************************/
void owi_read_rom(uint8_t *rom, uint8_t pin);

/***************************************************************
 *
 * DESCRIPTION:
 *  - Transmits a 64-bit OWI identifer specified by the user over
 *    the specified OWI bus pin. This command effectively addresses
 *    a specific slave device.
 *
 **************************************************************/
void owi_match_rom(uint8_t *rom, uint8_t pin);

/***************************************************************
 *
 * DESCRIPTION:
 *  - Searches specified pin for the 64-bit ROM identifier of some
 *    OWI device. This function is used if there multiple devices
 *    connected to the bus whose ROM values are unknown. The user
 *    must pass a pointer to an 8-byte buffer where the ROM will
 *    be stored. The user must also pass the value of the previous
 *    deviation. The function will return if value a new deviation
 *    value each call.
 *
 **************************************************************/
uint8_t owi_search_rom(uint8_t *bit_pattern, uint8_t last_deviation, uint8_t pin);

#ifdef __cplusplus
}
#endif

#endif
/* End of owi.h */

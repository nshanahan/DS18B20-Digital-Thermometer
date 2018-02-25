/***************************************************************
 * @file owi.h
 *
 * @par Nicholas Shanahan (2018)
 *
 * @brief Driver for Dallas 1-Wire Interface (OWI) bus. Intended for
 * the AVR family of microcontrollers. This implementation 
 * does not rely on UART hardware. Assumes use of external
 * pull-up resistors.
 *
 * Developed in accordance with AVR318: Dallas 1-Wire Master text.
 * http://www.atmel.com/images/doc2579.pdf
 *
 **************************************************************/

#ifndef _OWI_H
#define _OWI_H

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
/* 
 * Dallas OWI port letter.
 * TODO: Can this be made configurable some other way?
 */
#define OWI_PORT D

/**************************************************************
                      Pulbic Functions
***************************************************************/
/*!
 * @brief Initializes the specified OWI port locations by configuring
 * them as input pin. The user should declare a mask that contains
 * a one in the bit position corresponding to the port location
 * of the OWI device. Does NOT enable internal pull-up resistors.
 * @param[in] pin Pin number OWI device is connected to.
 * @return None.
 */
void owi_init(uint8_t pin);

/*!
 * @brief Indicates whether or not OWI bus is busy. Returns
 * Boolean true if bus is busy.
 * @param[in] pin Pin number OWI device is connected to.
 * @return bool
 */
bool owi_is_busy(uint8_t pin);

/*!
 * @brief Issues a reset to the specified OWI bus pin and determines
 * if an OWI slave device is present. Returns Boolean true if the slave 
 * is present and Boolean false if the slave is not present.
 * @param[in] pin Pin number OWI device is connected to.
 * @return bool
 */
bool owi_detect_presence(uint8_t pin);

/*!
 * @brief Writes a byte of data to OWI bus specified by the
 * pin input value.
 * @param[in] data Data value to write to bus.
 * @param[in] pin Pin number OWI device is connected to.
 * @return None.
 */
void owi_send_byte(uint8_t data, uint8_t pin);

/*!
 * @brief Reads a byte of data from OWI bus specified by the
 * pin input value.
 * @param[in] pin Pin number OWI device is connected to.
 * @return uint8_t
 */
uint8_t owi_recv_byte(uint8_t pin);

/*!
 * @brief Issues SKIP ROM command to the specified OWI bus pins. 
 * Can only be used when sending data to a slave device. Does not 
 * address a specific slave.
 * @param[in] pin Pin number OWI device is connected to.
 * @return None.
 */
void owi_skip_rom(uint8_t pin);

/*!
 * @brief Reads the unique 64-bit identifier of a OWI slave device.
 * The user passes a pointer to an 8-byte buffer where the
 * identifier will be stored.
 * @param[out] rom 8-byte buffer to store device ID.
 * @param[in] pin Pin number OWI device is connected to.
 * @return None.
 */
void owi_read_rom(uint8_t *rom, uint8_t pin);

/*!
 * @brief Transmits a 64-bit OWI identifer specified by the user over
 * the specified OWI bus pin. This command effectively addresses
 * a specific slave device.
 * @param[in] rom Pointer to 8-byte ID.
 * @param[in] pin Pin number OWI device is connected to.
 * @return None.
 */
void owi_match_rom(uint8_t *rom, uint8_t pin);

/*!
 * @brief Searches specified pin for the 64-bit ROM identifier of some
 * OWI device. This function is used if there multiple devices
 * connected to the bus whose ROM values are unknown. The user
 * must pass a pointer to an 8-byte buffer where the ROM will
 * be stored. The user must also pass the value of the previous
 * deviation. The function will return if value a new deviation
 * value each call.
 * @param[in] last_deviation
 * @param[in] pin Pin number OWI device is connected to.
 * @param[out] rom 8-byte buffer to store device ID.
 * @return uint8_t
 */
uint8_t owi_search_rom(uint8_t *rom, uint8_t last_deviation, uint8_t pin);

#ifdef __cplusplus
}
#endif

#endif /* _OWI_H */

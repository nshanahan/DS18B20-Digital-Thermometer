/***************************************************************
 * @file owi_crc.h
 * 
 * @par Nicholas Shanahan (2016)
 *
 * @brief Utility to compute the 8-bit cyclic redundancy check (CRC8)
 * of a byte of data. Intended to be used to compute the CRC8
 * of a large byte array.
 *
 **************************************************************/

#ifndef _OWI_CRC_H
#define _OWI_CRC_H

#ifdef __cplusplus
extern "C" {
#endif

/**************************************************************
                            Includes
***************************************************************/
#include <stdint.h>

/**************************************************************
                         Public Functions
***************************************************************/

/***************************************************************
 *
 * @brief Computes a Cyclic Redundancy Check of a byte of data. 
 * The input parameter seed may be zero or the previously
 * computed CRC value if attempting to calculated the CRC of
 * an array of bytes.
 * @param[in] data Data byte to perform CRC on.
 * @param[in] see CRC seed value.
 * @return uint8_t
 *
 **************************************************************/
uint8_t crc8(uint8_t data, uint8_t seed);

#ifdef __cplusplus
}
#endif

#endif /* _OWI_CRC_H */

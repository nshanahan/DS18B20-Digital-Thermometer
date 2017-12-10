/***************************************************************
 * Nicholas Shanahan (2016)
 *
 * DESCRIPTION:
 *  - Utility to compute the 8bit cyclic redundancy check (CRC8)
 *    of a byte of data. Intended to be used to compute the CRC8
 *    of a large byte array.
 *
 **************************************************************/
#ifndef OWI_CRC_H_
#define OWI_CRC_H_

#ifdef __cplusplus
extern "C" {
#endif

/**************************************************************
			  Includes
***************************************************************/
#include <stdint.h>

/**************************************************************
		    Function Prototypes
***************************************************************/

/***************************************************************
 *
 * DESCRIPTION:
 *  - Computes a Cyclic Redundancy Check of a byte of data. 
 *    The input parameter seed may be zero or the previously
 *    computed CRC value if attempting to calculat the CRC of
 *    an array of bytes.
 *
 **************************************************************/
uint8_t crc8(uint8_t data, uint8_t seed);

#ifdef __cplusplus
}
#endif

#endif
/* End of owi_crc.h */

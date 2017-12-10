/***************************************************************
 * Nicholas Shanahan (2016)
 *
 * DESCRIPTION:
 *  - Utility to compute the 8bit cyclic redundancy check (CRC8)
 *    of a byte of data. Intended to be used to compute the CRC8
 *    of a large byte array.
 *
 **************************************************************/

 /**************************************************************
					 Includes
 ***************************************************************/
#include "owi_crc.h"
#include <stdint.h>

 /**************************************************************
					 Defines
 ***************************************************************/
#define CRC8_POLY 0x18 //CRC8 polynomial

 /**************************************************************
					Function Prototypes
 ***************************************************************/
 /*See owi_crc.h for details*/
uint8_t crc8(uint8_t data, uint8_t seed)
{
	for (uint8_t i = 0; i < 8; i++)
	{
		if ((data ^ seed) & 0x01)
		{
			seed ^= CRC8_POLY;
			seed >>= 1;
			seed |= (1 << 7);
		}

		else
		{
			seed >>= 1;
		}

		data >>= 1;
	}

	//CRC result
	return seed;
}
/* End of owi_crc.h */

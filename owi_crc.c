/***************************************************************
 * @file owi_crc.c
 * 
 * @par Nicholas Shanahan (2016)
 *
 * @brief Library to calculate Cyclic Redundancy Check (CRC).
 *
 **************************************************************/

/**************************************************************
                            Includes
***************************************************************/
#include "owi_crc.h"
#include <stdint.h>
#include <avr/io.h>

/**************************************************************
                            Macros
***************************************************************/
//CRC8 polynomial
#define CRC8_POLY 0x18
#define BYTE_TO_BITS 8

/**************************************************************
                      Public Functions
***************************************************************/
//See owi_crc.h
uint8_t crc8(uint8_t data, uint8_t seed)
{
    uint8_t idx;
    
    for(idx = 0; idx < BYTE_TO_BITS; idx++)
    {
        if((data ^ seed) & 0x01)
        {
            seed ^= CRC8_POLY;
            seed >>= 1;
            seed |= _BV(7);
        }
        
        else
        {
            seed >>= 1;
        }
    
        data >>= 1;
    }

    return seed;
}

/***************************************************************
 * @file owi.c
 *
 * @par Nicholas Shanahan (2018)
 *
 * @brief Driver for Dallas 1-Wire Interface (OWI) bus. Intended for
 * the AVR family of microcontrollers. This implementation 
 * does not rely on UART hardware. Assumes presence external
 * pull-up resistors.
 *
 * Developed in accordance with AVR318: Dallas 1-Wire Master text.
 * http://www.atmel.com/images/doc2579.pdf
 *
 **************************************************************/
 
/**************************************************************
                            Includes
***************************************************************/
#include "owi.h"
#include "owi_delay.h"
#include <avr/io.h>
#include <stdint.h>
#include <stdbool.h>
#include <avr/interrupt.h>
//CPU frequency required for util library
#define F_CPU 16000000UL
#include <util/delay.h>

/**************************************************************
                            Macros
***************************************************************/
#define CONCAT(A,B) (A##B)
#define DDR(letter) CONCAT(DDR,letter)
#define PIN(letter) CONCAT(PIN,letter)
#define PORT(letter) CONCAT(PORT,letter)

#define BYTE_TO_BITS 8
#define ROM_LEN_BYTES 8
#define ROM_SEARCH_FAILED 0xFF

//ROM Commands
#define SKIP_ROM_CMD   0xCC
#define READ_ROM_CMD   0x33
#define MATCH_ROM_CMD  0x55
#define SEARCH_ROM_CMD 0xF0

/**************************************************************
                     Private Function Prototypes
***************************************************************/
/*
 * Inline these functions to remove overhead. Added "always_inline"
 * attribute to ensure the function is inlined even if compiler 
 * optimizations are turned off.
 */
static inline void release_bus(uint8_t pin) __attribute__ ((always_inline));
static inline void drive_bus_low(uint8_t pin) __attribute__ ((always_inline));
static inline bool read_bus_value(uint8_t pin) __attribute__ ((always_inline));
static inline void write_bit1(uint8_t pin) __attribute__ ((always_inline));
static inline void write_bit0(uint8_t pin) __attribute__ ((always_inline));
static inline bool read_bit(uint8_t pin) __attribute__ ((always_inline));

/*!
 * @brief Set OWI pin as an input.
 * @param[in] pin Pin number on OWI port.
 * @return None.
 */
static inline void release_bus(uint8_t pin)
{
    DDR(OWI_PORT) &= ~_BV(pin);
}

/*!
 * @brief Drives OWI pin low.
 * @param[in] pin Pin number on OWI port.
 * @return None.
 */
static inline void drive_bus_low(uint8_t pin)
{
    DDR(OWI_PORT) |= _BV(pin);
    PORT(OWI_PORT) &= ~_BV(pin);
}

/*!
 * @brief Reads OWI pin value.
 * @param[in] pin Pin number on OWI port.
 * @return bool
 */
static inline bool read_bus_value(uint8_t pin)
{
    return (PIN(OWI_PORT) & _BV(pin));
}

 
/*!
 * @brief Write a 1 to OWI bus.
 * @param[in] pin Pin number on OWI port.
 * @return None.
 */
static inline void write_bit1(uint8_t pin)
{
    cli();
    drive_bus_low(pin);
    _delay_us(OWI_DELAY_US_A);
    release_bus(pin);
    _delay_us(OWI_DELAY_US_B);
    sei();
}

/*!
 * @brief Write a 0 to OWI bus.
 * @param[in] pin Pin number on OWI port.
 * @return None.
 */
static inline void write_bit0(uint8_t pin)
{
    cli(); 
    drive_bus_low(pin);
    _delay_us(OWI_DELAY_US_C);
    release_bus(pin);
    _delay_us(OWI_DELAY_US_D);
    sei();
}

/*!
 * @brief Read a bit from the OWI bus.
 * @param[in] pin Pin number on OWI port.
 * @return bool
 */
static inline bool read_bit(uint8_t pin)
{
    bool bit;
    
    cli();
    drive_bus_low(pin);
    _delay_us(OWI_DELAY_US_A);
    release_bus(pin);
    _delay_us(OWI_DELAY_US_E);
    bit = read_bus_value(pin);
    _delay_us(OWI_DELAY_US_F);
    sei();

    return bit;
}

/**************************************************************
                     Public Functions
***************************************************************/

//See owi.h
void owi_init(uint8_t pin)
{
    release_bus(pin);
    _delay_us(OWI_DELAY_US_H);
}

//See owi.h
bool owi_is_busy(uint8_t pin)
{
    return !(read_bit(pin));
}

//See owi.h
bool owi_detect_presence(uint8_t pin)
{
    bool present;

    cli();
    drive_bus_low(pin);
    _delay_us(OWI_DELAY_US_H);
    release_bus(pin);
    _delay_us(OWI_DELAY_US_I);
    
    /* Pin is pulled low if device is present.
     * Otherwise pulled high by pull-up resistor. 
     */
    present = !(read_bus_value(pin));
    _delay_us(OWI_DELAY_US_J);
    sei();
    
    return present;
}

//See owi.h
void owi_send_byte(uint8_t data, uint8_t pin)
{
    uint8_t idx;
    
    for (idx = 0; idx < BYTE_TO_BITS; idx++)
    {
        //write lsb to OWI bus
        (data & 0x01) ? write_bit1(pin) : write_bit0(pin);
        //get next bit
        data >>= 1;
    }
}

//See owi.h
uint8_t owi_recv_byte(uint8_t pin)
{
    uint8_t idx;
    uint8_t data = 0;
    
    for (idx = 0; idx < BYTE_TO_BITS; idx++)
    {
        if(read_bit(pin)) 
        {
            data |= _BV(idx);
        }
    }
    
    return data;
}

//See owi.h
void owi_skip_rom(uint8_t pin)
{
    owi_send_byte(SKIP_ROM_CMD, pin);
}

//See owi.h
void owi_read_rom(uint8_t *rom, uint8_t pin)
{
    int8_t idx;
    
    owi_send_byte(READ_ROM_CMD, pin);

    for (idx = (ROM_LEN_BYTES - 1); idx >= 0; idx--)
    {
        rom[idx] = owi_recv_byte(pin);
    }
}

//See owi.h
void owi_match_rom(uint8_t *rom, uint8_t pin)
{
    int8_t idx;
    
    owi_send_byte(MATCH_ROM_CMD,pin);

    for (idx = (ROM_LEN_BYTES - 1); idx >= 0; idx--)
    {
        owi_send_byte(rom[idx], pin);
    }
}

//See owi.h
uint8_t owi_search_rom(uint8_t *rom, uint8_t last_deviation, uint8_t pin)
{
    bool bit1 = 0;
    bool bit2 = 0;
    bool err = false;
    uint8_t bit_idx = 0;
    uint8_t byte_idx = 0;
    uint8_t curr_idx = 1;
    uint8_t new_deviation = 0;
    
    owi_send_byte(SEARCH_ROM_CMD, pin);
    
    while (byte_idx < ROM_LEN_BYTES)
    {
        while (bit_idx < BYTE_TO_BITS)
        {
            bit1 = read_bit(pin);
            bit2 = read_bit(pin);
            
            //no ROM discovered, search failed
            if (bit1 && bit2)
            {
                err = true;
                new_deviation = ROM_SEARCH_FAILED;
                break;
            }
                     
            //devices discovered with same first bit of ROM
            else if (bit1 != bit2)
            {
                if (bit1) 
                {
                    rom[byte_idx] |= _BV(bit_idx);
                }
        
                else 
                {
                    rom[byte_idx] &= ~_BV(bit_idx);
                }
            }
            
            //devices discovered with different first bit of ROM
            else
            {
                if (curr_idx == last_deviation)
                {
                    rom[byte_idx] |= _BV(bit_idx);
                }
                
                else if (curr_idx > last_deviation)
                {
                    rom[byte_idx] &= ~_BV(bit_idx);
                    new_deviation = curr_idx;
                }
                
                else if (!(rom[byte_idx] & _BV(bit_idx)))
                {
                    new_deviation = curr_idx;
                }
            }
            
            //write the bit to the OWI bus
            (rom[byte_idx] & _BV(bit_idx)) ? write_bit1(pin) : write_bit0(pin);
            //move to next bit in the current byte
            bit_idx++;
            //increment overall bit position tracker
            curr_idx++;
        }
        
        if (err)
        {
            break;
        }
        
        bit_idx = 0;
        byte_idx++;
    }

    return new_deviation;
}

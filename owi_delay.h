/***************************************************************
 * @file owi_delay.h
 *
 * @par Nicholas Shanahan (2016)
 *
 * @brief Dallas Semiconductors recommended delay values
 * (in microseconds) for the for the bit-level OWI functions.
 * See able 3 of the AVR318: Dallas 1-Wire Master text
 * http://www.atmel.com/images/doc2579.pdf
 *
 **************************************************************/
 
#ifndef _OWI_DELAY_H
#define _OWI_DELAY_H

/*************************************************************
                            Macros
***************************************************************/
//OWI delay values in microseconds
#define OWI_DELAY_US_A  6
#define OWI_DELAY_US_B  64
#define OWI_DELAY_US_C  60
#define OWI_DELAY_US_D  10
#define OWI_DELAY_US_E  9
#define OWI_DELAY_US_F  55
#define OWI_DELAY_US_G  0
#define OWI_DELAY_US_H  480
#define OWI_DELAY_US_I  70
#define OWI_DELAY_US_J  410

#endif /* _OWI_DELAY_H */

/***************************************************************
 * Nicholas Shanahan (2016)
 *
 * DESCRIPTION:
 *  - Contains Dallas Semiconductors recommended delay values
 *    (in microseoncds) for the for the bit-level OWI functions.
 *    This information is located in Table 3 of the AVR318: Dallas
 *    1-Wire Master text: http://www.atmel.com/images/doc2579.pdf
 *    See owi.h for additional information about OWI.
 *
 **************************************************************/

#ifndef OWI_DELAY_H_
#define OWI_DELAY_H_

/**************************************************************
			  Defines
***************************************************************/
/*OWI Delay Values in Microseconds*/
#define OWI_DELAY_US_A	6
#define OWI_DELAY_US_B	64
#define OWI_DELAY_US_C	60
#define OWI_DELAY_US_D	10
#define OWI_DELAY_US_E  9
#define OWI_DELAY_US_F  55
#define OWI_DELAY_US_G  0
#define OWI_DELAY_US_H  480
#define OWI_DELAY_US_I  70
#define OWI_DELAY_US_J  410

#endif
/* End of owi_delay.h */

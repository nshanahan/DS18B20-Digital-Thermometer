/***************************************************************
 * Nicholas Shanahan (2016)
 *
 * DESCRIPTION:
 *  - Defines Dallas 1-Wire protocol commands.
 *
 *    Developed in accordance with AVR318: Dallas 1-Wire Master text.
 *    http://www.atmel.com/images/doc2579.pdf
 *
 **************************************************************/

#ifndef OWI_CMDS_H_
#define OWI_CMDS_H_

/**************************************************************
			Defines
***************************************************************/
//ROM Commands
#define SKIP_ROM_CMD   0xCC
#define READ_ROM_CMD   0x33
#define MATCH_ROM_CMD  0x55
#define SEARCH_ROM_CMD 0xF0

#endif
/*End of owi_cmds.h*/

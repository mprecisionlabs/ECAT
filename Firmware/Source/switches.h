#ifndef __SWITCHES_H			/* Avoid multiple inclusions */
#define __SWITCHES_H

/*
 * Other includes
 */

#include <shortdef.h>

/*
 * General definitions
 */

#define	PIRQ_ADDR			(0x040000)
#define	LED_ADDR				(0x040000)
#define	SWITCH_2_ADDR		(0x044000)
#define	SWITCH_3_ADDR		(0x048000)

/*
 * MACRO function definitions
 * The prototypes for the following are:
 *
 *		uchar	read_pirq(void);	-- Read the PIRQ inputs on the micro board
 *		uchar	read_sw2(void);	-- Read dip switch 2 on the micro board
 *		uchar	read_sw3(void);	-- Read dip switch 3 on the micro board
 *		void	setleds(int x);	-- Sets the status LEDs on the micro board
 */

#define	read_pirq()		(*(uchar *)PIRQ_ADDR)
#define	read_sw2()		(*(uchar *)SWITCH_2_ADDR)
#define	read_sw3()		(*(uchar *)SWITCH_3_ADDR)
#define	setleds(x)		(*(uchar*)LED_ADDR) = ((x) ^ 0x00ff)

#endif							/* ifndef __SWITCHES_H */

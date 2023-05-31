#ifndef __PERIOD_H			/* Avoid multiple inclusions */
#define __PERIOD_H

/*
 *  Other includes
 */

#include <shortdef.h>		/* Short type definitions */

extern volatile ushort	PICR;

/*
 * Macro Function Definitions
 * The prototypes for the following are:
 *
 *		void	PIT_kill(void);
 */

#define	PIT_kill(x)			(PICR = 0);

/*
 * PUBLIC function prototypes
 *
 *		PIT_init	-- Initializes the PIT timer.
 */

void	PIT_init(short, void (*p)());

#endif							/* ifndef __PERIOD_H */

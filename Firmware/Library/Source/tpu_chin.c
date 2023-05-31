#include "timer.h"

extern TPU_STRUCT	*TPU;		/* This is VOLATILE */

/* This routine sets the TPU for 477 nsec counts & sets up the TPU vector */

void	TPU_channel_init(
uchar	ch,
uchar	func,
uchar	hsr,
uchar	hsq,
uchar	prio,
uchar	int_enab)
{
	ushort int	offset;
	ushort int	value;
	ushort int	mask;

/* Setup the channel function */
	offset = 3 - (ch / 4);
	value = (func & 15) << ( ch % 4) * 4;
	mask  = ~(15 << ((ch % 4) * 4));
	TPU->CFS[offset] = (TPU->CFS[offset] & mask) | value;

/* Set up the interrupt enable */
	value = (1 << ch) * (int_enab & 1);
	mask  = ~(1 << ch);
	TPU->CIER = (TPU->CIER & mask) | value;

/* Set up the HSQ */
	offset = 1 - (ch / 8);
	value  = (hsq & 3) << ((ch % 8) * 2);
	mask   = ~(3 << ((ch % 8) * 2));
	TPU->HSQR[offset] = (TPU->HSQR[offset] & mask) | value;

/* Set up the HSR */
	offset = 1 - (ch / 8);
	value  = (hsr & 3) << ((ch % 8) * 2);
	TPU->HSSRR[offset] = (TPU->HSSRR[offset] & mask) | value;

	value = (prio & 3) << ((ch % 8) * 2);
	TPU->CPR[offset] = (TPU->CPR[offset] & mask) | value;
}



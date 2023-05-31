#include "timer.h"

extern volatile ushort	TPU_rams[16][8];
extern volatile ulong	param1[];
extern volatile ulong	param2[];
extern volatile ulong	param3[];

/*
 *	This routine sets the TPU up to initiate an interrupt periodically
 *	The function takes two variables:
 *		n:	Interval between interrupts (1 to 32767) * 4.77E-7 seconds
 *		p:	pointer to the function to be called during the interrupt
 *			THE FUNCTION MUST NOT BE OF TYPE INTERRUPT! it must be a normal
 *			reentrant C function with no arguments and no returned values
 */

int	TPU_periodic_int(
uint	n,
void	(*p)())
{
	int	ch;

	/* First find an unused tpu channel */
	do
		for(ch = 15; ((mode[ch] != NONE) && (ch > MAX_TPU_USED)); ch--)
			;
	while(mode[ch] != NONE);

	mode[ch]   = REPEAT_INT_GEN;
	param1[ch] = (long)p;
	param2[ch] = n;
	param3[ch] = n;

	TPU_rams[ch][0] = 0x0093;			/* 0 1001 0011 */
	TPU_rams[ch][1] = 0x4000;			/* arbitrary offset to start rolling */
	TPU_rams[ch][2] = 0x00EC;
	TPU_rams[ch][3] = 0x0000;			/* Unused */
	TPU_channel_init(ch,0x0E,1,0,3,1);
	return(ch);
}

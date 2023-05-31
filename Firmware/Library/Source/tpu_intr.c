#include "timer.h"

extern volatile ushort	TPU_rams[16][8];
extern volatile ulong	param1[];
extern volatile uchar	happened[];

/*
 *	This routine sets up a TPU input to trigger and interrupt on
 *	Rising, falling, or both edges
 *	TPU_interrupt(ch,m,p)
 *	where:
 *		ch: Channel number to use
 *		 m: Mode 1 = positive edge; -1 = negative edge; 0 = both
 *		 p: Pointer to the function to be called during the interrupt
 *			 THE FUNCTION MUST NOT BE OF TYPE INTERRUPT! it must be a normal
 *			 reentrant C function with no arguments and no returned values
 *	SEE the ITC section of the TPU manual (p3-13).
 */

/*
 * return state should be
 *		FF - NO transitions
 *		1	- High transition
 *		0	- Low transition
 */

uchar	TPU_read_int_pin(
uchar	ch)
{
	uchar	tvar;

	tvar = happened[ch];
	happened[ch] = 0xFF;
	return(tvar);
}

void	TPU_interrupt(
uchar	ch,
short	m,
void	(*p)())
{
	param1[ch] = (long)p;
	mode[ch]   = INT_GEN;

	if(m > 0)
		TPU_rams[ch][0] = 0x0007;					/* 0 0000 0111  rising edge	*/
	else
		if(m < 0)
			TPU_rams[ch][0] = 0x000B;				/* 0 0000 1011  falling edge	*/
		else
			TPU_rams[ch][0] = 0x000F;				/* 0 0000 1111  both edges		*/

 	TPU_channel_init(ch,0x08,3,0,3,1);
}

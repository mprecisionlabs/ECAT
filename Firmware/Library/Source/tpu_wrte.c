#include "timer.h"

/*
 *  This will set the desired TPU channel HI or LOW as per
 *  the state variable.  Uses the TPU DIO function - TPU (p3-5, p3-11).
 */

void	TPU_write(
uchar	ch,
uchar	state)
{
	mode[ch] = OUT;
	if(state)
		TPU_channel_init(ch,0x08,1,0,1,0);
	else
		TPU_channel_init(ch,0x08,2,0,1,0);
}

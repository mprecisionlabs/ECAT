#include "timer.h"

extern volatile ushort	TPU_rams[16][8];
extern volatile ulong	param1[];

/*
 *  This function will set the desired TPU channel to be an input
 *  And the count the transitions until the transition count is zeroed
 *  by calling this function again.
 *  To access the transition count call TPU_read_event
 */

void	TPU_start_event(
uchar	ch)
{
	TPU_rams[ch][0] = 0x0007;
	TPU_rams[ch][1] = 0x000e;
	TPU_rams[ch][2] = 0xffff;
	param1[ch] = 0;
	mode[ch] = COUNT;
	TPU_channel_init(ch,0x0A,0,1,3,1);
	TPU_channel_init(ch,0x0A,1,1,3,1);
}

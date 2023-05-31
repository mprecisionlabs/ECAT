#include "timer.h"

extern volatile ushort	TPU_rams[16][8];
extern volatile ulong	param1[];

/*
 *	This routine causes the timer channel ch to output a  pulse of
 *	the duration specified  with n, polarity pol
 *	The n can be 1 to 32767 and will result in a duration of n * 477ns
 *	The function will return without waiting for the pulse to complete
 */

void	TPU_one_shot(
uchar		ch,
ushort	duration,
ushort	polarity,
void		(*p)())
{
	int	i;

	TPU_rams[ch][1] = duration;
	TPU_rams[ch][2] = 0x00ec;
	TPU_rams[ch][3] = 0x0000;
	param1[ch] = (long)p;
	mode[ch] = ONE_SHOT;
	if(polarity)
		i = 1;
	else
		i = 0;
	if(polarity > 0)
	{
		TPU_rams[ch][0] = 0x008D;				/* 0 1000 1101  channel control */
		TPU_channel_init(ch,0x0E,1,0,3,i);
	}
	else
	{
		TPU_rams[ch][0] = 0x008E;				/* 0 1000 1110  channel control */
		TPU_channel_init(ch,0x0E,1,0,3,i);
	}
}

#include "timer.h"

extern volatile ushort	TPU_rams[16][8];
extern volatile ulong	param1[];

/*
 *  This is the companion to the TPU_start_event function
 *  It will return the number of transitions since the channel
 *  was initialized.
 */

long	TPU_read_event(
uchar	ch)
{
	long	i;
	long	j;

	do
	{
		i = param1[ch];
		j = TPU_rams[ch][3];
	}
	while(i != param1[ch]);		/* Tests for Transition during measurement */
	return(j + (i << 16));
}

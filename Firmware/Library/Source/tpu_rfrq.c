#include <float.h>
#include "timer.h"

extern volatile ulong	param1[];
extern volatile ulong	param2[];
extern volatile uchar	happened[];

/*
 *	This is the companion to the function TPU_start_freq
 *	It returns the frequency measured on the asociated channel
 *	It returns zero if no measurment has been made
 *	It does no detect overflow.
 *	(see the minimum frequency listed in TPU_start_freq)
 */

float	TPU_read_freq(
char	ch)
{
	float	r;

	if(mode[ch] != FREQ)
		return(-1.0);

	if(happened[ch])
		r = ((param1[ch] >> 8) & 0xFF) / (param2[ch] * 4.7684E-7);
	else
		r = 0.0;
	happened[ch] = 0;
	return(r);
}

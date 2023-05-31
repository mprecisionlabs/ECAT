#include "timer.h"

extern volatile ushort	TPU_rams[16][8];
extern volatile uint	TPU_rami[16][4];

/*
 *  This functions will set the desired TPU channel to be an output
 *  With the waveform period of period * 477 nsec and
 *  the hi time = hi * 477nsec.  Uses TPU pwm (p3-34, p3-42)
 */

void	TPU_set_pwm(
uchar	ch,					/* Channel						*/
ushort	period,			/* Period in TCR1 counts	*/
ushort	hi)				/* Time Hi in TCR1 counts	*/
{
	uint	i;

	TPU_rami[ch][1] = (uint)((hi * 65536) + period);
	if(mode[ch] != PWM)
	{															/* do full initialization */
		TPU_rams[ch][0] = 0x91;
		mode[ch] = PWM;
		TPU_channel_init(ch,0x09,2,0,2,0);
	}
}

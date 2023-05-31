#include "timer.h"

extern volatile ushort	TPU_rams[16][8];
extern volatile ulong	param1[];
extern volatile ulong	param2[];
extern volatile ulong	param3[];
extern volatile uchar	happened[];

/*
 *	This routine will set up a phase based trigger
 *	Where the zero crossing  on one channel will cause a phase related
 *	event on another channel
 *	This event can be one of four events:
 *			Positive pulse of some duration
 *			Negative pulse of some duration
 *			A positive step
 *			A negative step
 *
 *	TPU_phase_trigger(
 *			unsigned char ch_sync,		The channel to sync to
 *			unsigned char ch_out,		The channel to effect
 *			unsigned char slope,			non zero for a positive step
 *			unsigned int duration,		The pulse duration (0 for permanant)
 *			unsigned int phase,			The phase in degrees
 *			unsigned int times,			The number of times to trigger
 *			void	p())						The callback function
 */

void	TPU_phase_trigger(
uchar	ch_sync,
uchar	ch_out,
uchar	slope,
uint	duration,
uint	phase,
uint	times,
void	p())
{
	int	ch;

	do
 		for(ch = 15; ((mode[ch]!=NONE) && (ch > MAX_TPU_USED)); ch--)
			;
	while(mode[ch]!=NONE);

	if(phase > 359)
		phase = phase % 360;
	TPU_write(ch_out,!(slope));

	mode[ch_sync]	= TRIGSYNC;
	mode[ch_out]	= TRIGOUTS;
	mode[ch]			= TRIGOUTS;

	param1[ch_sync]   = phase;			/* Holds phase from trigger in degrees	*/
	param2[ch_sync]   = 0x10000;		/* Holds time of last transition			*/
	param3[ch_sync]	= ch;				/* Long trigger helper channel			*/
	happened[ch_sync] = ch_out;		/* Holds address of ch_out					*/

	param2[ch]			= times;			/* Holds number of iterations to do		*/
	happened[ch]		= ch_out;

	param1[ch_out]		= duration;		/* Holds interval to be high or low		*/
	param2[ch_out]		= times;			/* Holds output state 0 for - 1 for +	*/
	param3[ch_out]		= (long)p;
	happened[ch_out]	= slope;			/* Holds number of iterations to do		*/

	/* Now to set up an ITC channel on CH_sync */

	if(phase < 45)
		param1[ch_sync] += 360;
	TPU_rams[ch_sync][0] = 0x07;		/* Rising edge	*/

	TPU_rams[ch_sync][1] = 0x0E;		/* No links and bad location for BANK_ADDRESS */
	TPU_rams[ch_sync][2] = 1;			/* Number of counts per interrupt */

	if(slope)
		TPU_rams[ch_out][0] = 0x8A;
	else
		TPU_rams[ch_out][0] = 0x85;

	TPU_rams[ch_out][1] = 3000;
	TPU_rams[ch_out][2] = 0x08C;
	TPU_rams[ch_out][3] = 0;

	TPU_channel_init(ch,0xE,0,0,3,1);
	TPU_channel_init(ch_out,0xE,0,0,3,1);
	TPU_channel_init(ch_sync,0xA,1,1,3,1);
}

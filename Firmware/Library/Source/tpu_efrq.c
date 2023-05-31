#include "timer.h"
#include <stdio.h>

extern volatile ushort	TPU_rams[16][8];
extern volatile ulong	param1[];
extern volatile ulong	param2[];
extern volatile ulong	param3[];
extern volatile uchar	happened[16];
extern volatile int		SYNC_RISING_EDGE;

static long FREQ_TIMES[4];
static long done_with_freq;

volatile TPU_MODE prev_mode[16];

/* Start sync channel to detect rising edge  */

void  TPU_sync_start(char ch)
{ 
	mode[ch] = SYNC_CHANNEL;						/* sync channel mode		*/

 /* Now to set up the input channel to be input capture on rising edge */
	TPU_rams[ch][0] = 0x07;							/* Detect rising edge	*/
	TPU_rams[ch][1] = 0x0E;
	TPU_rams[ch][2] = 0x01;							/* Count 1 Transition	*/
	TPU_channel_init(ch,0x0A,1,1,3,1);
}


/*
 * Sync channel interrupt function (called in TPU_trap)
 * Sets rising edge flag
 */
void	process_Sync_Channel(uchar ch)
{
	SYNC_RISING_EDGE = 1;							/* Rising edge flag		*/
}

float	TPU_ECAT_freq(char sync_ch)
{ 
	done_with_freq		 = 0;
	happened[sync_ch]	 = 0;
	prev_mode[sync_ch] = mode[sync_ch];
	mode[sync_ch]		 = ECAT_FREQ;

	if(prev_mode[sync_ch] != SYNC_CHANNEL)
	{
		/* Now to set up the input channel to be input capture on both edges! */
		TPU_rams[sync_ch][0] = 0x07;				/* Detect rising edge	*/
		TPU_rams[sync_ch][1] = 0x0E;
		TPU_rams[sync_ch][2] = 0x01;				/* Count 1 Transition	*/
		TPU_channel_init(sync_ch,0x0A,1,1,3,1);
	}
	TPU_delay(70);

	if(done_with_freq)
		return(1/(477E-9*(float)done_with_freq));
	else
		return(0);
}


void process_Ecat_Freq(uchar ch)
{
	if(happened[ch] < 4)
	{
		FREQ_TIMES[happened[ch]] = TPU_rams[ch][4];
		happened[ch]++;
	}
	switch(happened[ch])
	{
		case 1:
			TPU_rams[ch][0] = 0x0F;					/* Detect either edge	*/
			TPU_channel_init(ch,0x0A,1,1,3,1);
		case 3:
		case 5:
			SYNC_RISING_EDGE = 1;					/* Rising edge flag		*/
			return;
		case 2:
			if(FREQ_TIMES[0] > FREQ_TIMES[1])
				FREQ_TIMES[1] += 0x10000;
			FREQ_TIMES[0] = (FREQ_TIMES[1] + FREQ_TIMES[0]) / 2;
			/* Surge time [0] now has the zero # 1 Time */
			return;
		case 4:
			if(FREQ_TIMES[2] > FREQ_TIMES[3])
				FREQ_TIMES[3] += 0x10000;
			FREQ_TIMES[2] = (FREQ_TIMES[3] + FREQ_TIMES[2]) / 2;
			/* Surge time [2] now has the zero # 2 Time */
			while(FREQ_TIMES[2] < FREQ_TIMES[0])
				FREQ_TIMES[2] += 0x10000;
			while(FREQ_TIMES[3] < FREQ_TIMES[2])
				FREQ_TIMES[3] += 0x10000;
			done_with_freq = FREQ_TIMES[2] - FREQ_TIMES[0];	/* Period */
			if(prev_mode[ch] != SYNC_CHANNEL)
				TPU_kill(ch);
			else
			{
				mode[ch] = SYNC_CHANNEL;
				TPU_rams[ch][0] = 0x07;				/* Detect rising edge	*/
				TPU_channel_init(ch,0x0A,1,1,3,1);
			}
			return;
	} /* End of case */
}

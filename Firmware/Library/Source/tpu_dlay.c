#include "timer.h"

extern volatile ushort	TPU_rams[16][8];
extern const	tpu_mask;
extern int		reset_flags;

/*
 *  This will function use any available TPU channel to calculate a delay
 *  then read it's value.  Uses the TPU OC function TPU (p3-22, p3-33).
 */

void	TPU_delay(
int	delay)
{
	int	i,j,ch;

	if(!(reset_flags & tpu_mask))
		TPU_init();
	i = (delay * 0x830) - 0x1A2;
	j = i / 0x8000;						/* The number of full delays to wait */
	i = (i & 0x7FFF);


	/* First find an unused tpu channel */
	do
		for(ch = 15; ((mode[ch] != NONE) && (ch > MAX_TPU_USED)); ch--)
			;
	while(mode[ch] != NONE);

	do
	{
		if(j)
			TPU_rams[ch][1] = 0x7E50;			/* Experimentally determined 431 */
		else											/* counts used up in code			*/
			TPU_rams[ch][1] = (ushort)i;
		TPU_rams[ch][2] = 0x00ec;
		TPU_rams[ch][3] = 0x0000;
		mode[ch] = DELAY;
		TPU_rams[ch][0]=0x008D;					/* 0 1000 1101 - channel control */

		TPU_channel_init(ch,0x0E,1,0,3,1);
		while(mode[ch] == DELAY)
			;
		j--;
	}
	while(j >= 0);
	mode[ch] = NONE;
}


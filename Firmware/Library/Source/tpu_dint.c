#include "timer.h"

extern volatile ushort	TPU_rams[16][8];
extern volatile ulong	param1[];
extern volatile ulong	param2[];

int	TPU_delay_int(
int	n,
void	(*p)())
{
	int	ch;
	int	i,j;

	/* First find an unused tpu channel */
	do
		for(ch = 15; ((mode[ch] != NONE) && (ch > MAX_TPU_USED)); ch--)
			;
	while(mode[ch] != NONE);

	i = (n / 32768) + 1;
	param2[ch] = i;						/* Number of timer to int before call */
	i = (n / i);

	mode[ch]   = DELAY_INT;
	param1[ch] = (long)p;

	TPU_rams[ch][2] = i;
	TPU_rams[ch][0] = 0x13;				/* 0 0001 0011 */
	TPU_channel_init(ch,0x08,3,1,3,1);
	return(ch);
}


int	TPU_repeat_delayint(
char  ch,
int	n,
void	(*p)())
{
	int	i,j;

	/* First find an unused tpu channel */
	if	(ch == 0)
	  {
		do
			for(ch = 15; ((mode[ch] != NONE) && (ch > MAX_TPU_USED)); ch--)
				;
		while(mode[ch] != NONE);
		mode[ch]   = REPEAT_DELAYINT;
	  }
	i = (n / 32768) + 1;
	param2[ch] = i;	  					/* Number of timer to int before call */
	i = (n / i);

	param1[ch] = (long)p;

	TPU_rams[ch][2] = i;
	TPU_rams[ch][0] = 0x13;				/* 0 0001 0011 */
	TPU_channel_init(ch,0x08,3,1,3,1);
	return(ch);
}




        

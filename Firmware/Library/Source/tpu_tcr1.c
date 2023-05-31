#include "timer.h"

extern volatile short	TPU_rams[16][8];
extern volatile ulong	param1[];

/*
 *  This will function use any available TPU channel to capture the
 *  TCR1 time then read it's value.
 *  Uses the TPU OC function TPU (p3-22, p3-33).
 */

int	TPU_read_tcr1(void)
{
	int	ch;

	/* First find an unused tpu channel */
	do
		for(ch = 15; ((mode[ch] != NONE) && (ch > MAX_TPU_USED)); ch--)
			;
	while(mode[ch] != NONE);
	mode[ch] = CATCH;

	TPU_rams[ch][0] = 0x0000;
	TPU_rams[ch][1] = 0x0000;
	TPU_rams[ch][3] = 0x00EC;
	TPU_rams[ch][4] = 0x0000;
	TPU_channel_init(ch,0x0E,1,3,3,1);
	while(mode[ch] == CATCH)
		;
	mode[ch] = NONE;
	return((int)param1[ch]);
}

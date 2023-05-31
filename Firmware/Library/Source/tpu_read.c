#include "timer.h"

extern TPU_STRUCT		*TPU;						/* This is VOLATILE */
extern volatile ushort	TPU_rams[16][8];

/*
 *  This will function any TPU channel as an input and then read it's value
 *  Uses the TPU DIO function TPU (p3-5, p3-11).
 */

char	TPU_read(
uchar	ch)
{
	ushort	tvar;

	mode[ch] = IN;
	TPU_rams[ch][0] = 0x0f;					/* 0 0000 1111 */
	TPU_rams[ch][2] = 0x4000;

	TPU_channel_init(ch,0x08,3,2,3,0);
	while(TPU->HSSRR[1-(ch/8)] & (3 << ((ch % 8) * 2)));
	tvar = TPU_rams[ch][1];
	if(tvar & 32768)
		return(1);
	else
		return(0);
}

#include "serint.h"

extern volatile ushort	SCSR;
extern volatile uchar	int_active;
extern volatile uint	rx_char_cnt;

uchar	SCI_charavail(void)
{
	if(int_active)
		return(rx_char_cnt != 0);
	else
		return(SCSR & 0x0040);
}

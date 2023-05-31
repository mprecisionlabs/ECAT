#include "serint.h"

extern volatile ushort	SCDR;
extern volatile ushort	SCSR;

extern uchar	localecho;
extern volatile uchar	int_active;
extern volatile uchar	rx_fifo[RX_FIFO_SIZE];
extern volatile uint	rx_tail;
extern volatile uint	rx_char_cnt;

char	_INCHRW(void)
{
	char	c;

	if(int_active)
	{
		while(!rx_char_cnt)
			;
		c = rx_fifo[rx_tail];
		rx_tail = (rx_tail + 1) % RX_FIFO_SIZE;
		rx_char_cnt--;
		if(localecho)
			_OUTCHR(c);
		return(c);
	}
	else
	{
		while(!(SCSR & 0x0040))
			;
		c = (uchar)SCDR;
		while(!(SCSR &0x0100))
			;
		SCDR = c;
		return(c);
	}
}

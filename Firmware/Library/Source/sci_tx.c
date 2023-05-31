#include "serint.h"

extern volatile ushort	SCDR;
extern volatile ushort	SCSR;
extern volatile ushort	SCCR1;
extern volatile uchar	int_active;
extern volatile uchar	tx_int_active;
extern volatile uchar	tx_fifo[TX_FIFO_SIZE];
extern volatile uint	tx_head;
extern volatile uint	tx_tail;
extern volatile uint	tx_char_cnt;

void	_OUTCHR(
char	c)
{
#ifdef DO_TXINT
	if(int_active)
	{
		while(tx_char_cnt > (TX_FIFO_SIZE - 2))
			;	/* Fifo is almost full wait */

		tx_fifo[tx_head] = c;
		tx_head = (tx_head + 1) % TX_FIFO_SIZE;
		tx_char_cnt++;
		if((!tx_int_active) && (tx_char_cnt))
		{
			tx_int_active = 2;
			c = tx_fifo[tx_tail];
			tx_tail = (tx_tail + 1) % TX_FIFO_SIZE;
			tx_char_cnt--;
			tx_int_active = 1;
			SCCR1	= 0x00AC;					/* TxInt, RxInt, TxEnab, RxEnab	*/
			SCDR	= c;
		}
	}
	else
#endif
	{
		while(!(SCSR & 0x0100));
			SCDR = c;
	}
}

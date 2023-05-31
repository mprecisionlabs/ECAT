#include "serint.h"

extern volatile ushort	SCDR;
extern volatile ushort	SCSR;
extern volatile ushort	SCCR1;

extern volatile uchar	tx_int_active;
extern volatile uchar	rx_fifo[];
extern volatile uint	rx_head;
extern volatile uint	rx_overflow;
extern volatile uint	rx_char_cnt;
extern volatile uchar	tx_fifo[TX_FIFO_SIZE];
extern volatile uint	tx_tail;
extern volatile uint	tx_char_cnt;

interrupt void	SCI_trap(void)
{
	ushort	status;
	uchar		c;

	status = SCSR;

	if(status & RDR)
	{
		c = (uchar)SCDR;
		rx_fifo[rx_head] = c;
		rx_head = (rx_head + 1) % RX_FIFO_SIZE;
		rx_char_cnt++;
		if(rx_char_cnt > RX_FIFO_SIZE)
			rx_overflow = 1;
	}
	if((status & TDRE) && (tx_int_active == 1))
	{
		if(tx_char_cnt)
		{
			c = tx_fifo[tx_tail];
			tx_tail = (tx_tail + 1) % TX_FIFO_SIZE;
			tx_char_cnt--;
			SCDR = c;
		}
		else
		{
			SCCR1 = 0x002C;				/* RxInt, TxEnab, RxEnab	*/
			tx_int_active = 0;
		}
	}
}

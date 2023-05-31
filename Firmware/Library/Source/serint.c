#include "vectors.h"
#include "serint.h"

extern volatile ulong	vectors[];
extern volatile ushort	SCDR;
extern volatile ushort	SCSR;
extern volatile ushort	SCCR0;
extern volatile ushort	SCCR1;
extern volatile ushort	QMCR;
extern volatile ushort	QILR_QIVR;

uchar	localecho;
volatile uchar	int_active;
volatile uchar	tx_int_active;
interrupt void	SCI_TRAP(void);

volatile uchar	rx_fifo[RX_FIFO_SIZE];
volatile uint		rx_head;
volatile uint		rx_tail;
volatile uint		rx_overflow;
volatile uint		rx_char_cnt;

volatile uchar	tx_fifo[TX_FIFO_SIZE];
volatile uint		tx_head;
volatile uint		tx_tail;
volatile uint		tx_overflow;
volatile uint		tx_char_cnt;

void	init_int_sio(
int		baud,
uchar	echo)
{
	int	i;

	rx_head = 0;
	rx_tail = 0;
	rx_overflow = 0;
	rx_char_cnt = 0;
	tx_head = 0;
	tx_tail = 0;
	tx_char_cnt = 0;
	tx_int_active = 0;
	QMCR = QSM_ARB;								/* QSM configuration register		*/
	i = QILR_QIVR;
	i = i & 0xF800;								/* Clear SCI_IRQ level				*/
	i |= (SCI_IRQ_LEVEL * 256) + QSM_VECTOR_BASE;
														/* QSM interrupt level Register	*/
	QILR_QIVR = i;  
	vectors[QSM_VECTOR_BASE & 254] = (long)SCI_TRAP;			/* IRQ vector	*/

	SCCR0 = 524288 / baud;
	SCCR1 = 0x002C;								/* RxInt, TxEnab, RxEnab			*/
	int_active = 1;
	localecho  = echo & 0x01;
}


interrupt void	SCI_TRAP(void)
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

void	int_tx(
uchar	c)
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

void	_OUTCHR(
char	c)
{
	if(int_active)
		int_tx(c);
	else
	{
		while(!(SCSR & 0x0100));
			SCDR = c;
	}
}

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

uchar	charavail(void)
{
	if(int_active)
		return(rx_char_cnt != 0);
	else
		return(SCSR & 0x0040);
}

void	stop_serint(void)
{
	SCCR1 = 0x000C;
	int_active = 0;
	SCCR0 = 524288 / 19200;
}

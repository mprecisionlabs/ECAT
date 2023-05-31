#include "serint.h"
#include "vectors.h"

extern volatile ulong	vectors[];
extern volatile ushort	SCCR0;
extern volatile ushort	SCCR1;
extern volatile ushort	QMCR;
extern volatile ushort	QILR_QIVR;

uchar	localecho;
volatile uchar	int_active;
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
volatile uchar	tx_int_active;

void	SCI_init(
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
	vectors[QSM_VECTOR_BASE & 254] = (long)SCI_trap;			/* IRQ vector	*/
														   
	SCCR0 = 524288 / baud;
	SCCR1 = 0x002C;								/* RxInt, TxEnab, RxEnab			*/
	int_active = 1;
	localecho  = echo & 0x01;
}

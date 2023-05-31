#include "qsm.h"

extern volatile ushort	SPCR1;

void	QSM_halt(void)
{
											/* Stop the system			*/
	SPCR1 = 0x0008;					/* QSPI control register 1	*/
}


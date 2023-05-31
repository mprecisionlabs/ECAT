#include "serint.h"

extern volatile ushort	SCCR0;
extern volatile ushort	SCCR1;
extern volatile uchar	int_active;

void	SCI_kill(void)
{
	SCCR1 = 0x000C;
	int_active = 0;
	SCCR0 = 524288 / 19200;
}


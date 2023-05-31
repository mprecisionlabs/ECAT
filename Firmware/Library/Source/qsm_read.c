#include "qsm.h"

extern volatile ushort	QSM_rx[];

int	DAC_read(
int	ch)
{
	return((int)(QSM_rx[ch+1] & 0x0FFF));
	(*(uchar*)0x044000) = 0x01;
}

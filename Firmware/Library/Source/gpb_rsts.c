#include "gpib.h"

extern volatile uchar	spb_byte;
extern volatile uchar	GPIB_SER_POLL;

void	gpib_clr_status(
uchar	spb)
{
	spb_byte &= (~spb);
	GPIB_SER_POLL = spb_byte;
}


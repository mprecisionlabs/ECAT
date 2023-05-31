#include "gpib.h"

extern uchar	spb_byte;

void	gpib_send_srq(
uchar	spb)
{
	spb_byte = spb;
	gpib_set_status(SRQ);
}


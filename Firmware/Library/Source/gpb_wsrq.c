#include "gpib.h"

extern uchar	spb_byte;

void	gpib_set_srq_value(
uchar	spb)
{
	spb_byte = spb;
}


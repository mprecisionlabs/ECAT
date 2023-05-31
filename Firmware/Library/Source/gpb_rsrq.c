#include "gpib.h"

extern uchar	spb_byte;

uchar	get_srq_value(void)
{
	return(spb_byte);
}


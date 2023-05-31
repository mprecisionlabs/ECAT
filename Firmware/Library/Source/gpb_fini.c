#include "gpib.h"

extern BUF_TYPE	*out_buff;
extern BUF_TYPE	*in_buff;

void	gpib_file_init(void)
{
	out_buff = in_buff = NULL;
}


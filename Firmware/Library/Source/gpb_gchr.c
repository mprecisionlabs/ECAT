#include "gpib.h"

extern BUF_TYPE	*in_buff;

char	gpib_get_char(void)
{
	char	c;

	while(in_buff == NULL)
		in_buff = gpib_get_buffer();
	if(buffer_empty(in_buff))
	{
		c = '\n';
		destroy_buffer(in_buff);
		in_buff = gpib_get_buffer();
	}
	else
		c = get_char_from_buffer(in_buff);
	return(c);
}



#include "gpib.h"

extern BUF_TYPE	*out_buff;

void	gpib_write(
char	c)
{
	if(out_buff == NULL)
		out_buff = create_buffer(255);
	if(c == '\n')
	{
		gpib_send_buffer(out_buff);
		out_buff = NULL;
	}
	else
		add_char_to_buffer(out_buff,c);
}



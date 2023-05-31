#include "buffer.h"

/*
 *	add_char_to_buffer
 *
 *	This function will:
 *		- add a char to the specified buffer
 *		- increment the tail pointer if buf_type
 *		- the tail pointer will wrap around if incremented beyond buf_len
 *		- return:
 *			- 2 = wrap around occured
 *			- 1 = 10 bytes left
 *			- 0 = successful
 */
int	add_char_to_buffer(
BUF_TYPE	*buf_ptr,
uchar	c)
{
	int	rv;

	if(buf_ptr->tail < (buf_ptr->buf_len-10))
		rv = 1;
	else
		if(buf_ptr->tail == (buf_ptr->buf_len-1))
			rv = 2;
		else
			rv = 0;

	buf_ptr->buf[buf_ptr->tail] = c;
	buf_ptr->tail++;
	if(buf_ptr->tail == buf_ptr->buf_len)
		buf_ptr->tail = 0;
	if(buf_ptr->tail == buf_ptr->head) 
	{
		buf_ptr->head++;
		if(buf_ptr->head == buf_ptr->buf_len)
			buf_ptr->head = 0;
	}
	return(rv);
}

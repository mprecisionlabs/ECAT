#include "buffer.h"

/*
 *	get_char_from_buffer
 *
 *	This function will:
 *		- get a char from the specified buffer
 *		- wrap around if the head exceeds the buffer length
 *		- return -1 if the buffer head = buffer tail
 */

short	get_char_from_buffer(
BUF_TYPE	*buf_ptr)
{
	uchar	c;

	if(buf_ptr->head == buf_ptr->tail)
		return((short)-1);
	else
	{
		c = buf_ptr->buf[buf_ptr->head];
		buf_ptr->head++;
      if(buf_ptr->head == buf_ptr->buf_len )
		buf_ptr->head=0; /* buffer must have wrapped around */
	}
	return((short)c);
}

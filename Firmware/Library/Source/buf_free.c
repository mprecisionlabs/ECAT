#include <stdio.h>
#include "buffer.h"

/*
 *	reallocate_buffer
 *
 *	This function will:
 *		- reallocate memory for the buffer of length "buf_len"
 *		- modify the pointer if necessary, and buf_len in buf_type
 */

int	reallocate_buffer(
BUF_TYPE	*buf_ptr,
uint	buf_len)
{
	char	*temp_str;

	if((buf_ptr == (BUF_TYPE *)NULL) || ((temp_str = (char *)realloc(buf_ptr->buf,buf_len)) == (char *)NULL))
	{
		fprintf(stderr,"(FATAL) MEM SHORT - FREE BUFF");
		return((char)NULL);
	}
	buf_ptr->buf = temp_str;
	buf_ptr->buf[buf_len] = (char)NULL;
	buf_ptr->buf_len = buf_len;

	if((buf_ptr->head > buf_len) || (buf_ptr->tail > buf_len))
	{
		reset_buffer(buf_ptr);
		fprintf(stderr,"(FATAL) RESET - FREE BUFF");
		return(0);
	}
	return(1);
}

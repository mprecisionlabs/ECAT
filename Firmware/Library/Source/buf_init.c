#include <stdio.h>
#include "buffer.h"

/*
 *	create_buffer
 *
 *	This function will:
 *		- allocate memory for the buffer of length "buf_len"
 *		- return a pointer of buf_type to the newly created buffer
 */

BUF_TYPE	*create_buffer(
uint	buf_len)
{
	BUF_TYPE	*temp_buf;
	char		*temp_str;

	if(((temp_buf = (BUF_TYPE *)malloc(sizeof(BUF_TYPE))) == NULL) ||
		((temp_str = (char *)(BUF_TYPE *)malloc(buf_len)) == NULL))
	{
		fprintf(stderr,"(FATAL) MEM SHORT - BUFF");
		return(NULL);
	}
	temp_buf->buf		= temp_str;
	temp_buf->buf[0]	= (char)NULL;
	temp_buf->buf_len	= buf_len;
	temp_buf->head		= 0;
	temp_buf->tail		= 0;
	return(temp_buf);
}

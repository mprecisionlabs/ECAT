#include <stdio.h>
#include "buffer.h"

void	reset_buffer(
BUF_TYPE	*buf_ptr)
{
	buf_ptr->buf[0]= (char)NULL;
	buf_ptr->head	= 0;
	buf_ptr->tail	= 0;
}


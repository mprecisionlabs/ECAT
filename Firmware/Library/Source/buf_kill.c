#include "buffer.h"

/*
 *	destroy_buffer
 *
 *	This function will:
 *		- free memory allocated for the the specified buffer of "buf_type"
 */

void	destroy_buffer(
BUF_TYPE	*buf_ptr)
{
	free(buf_ptr->buf);
	free(buf_ptr);
}


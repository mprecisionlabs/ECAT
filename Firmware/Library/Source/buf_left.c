#include "buffer.h"
unsigned int      get_chars_in_buffer(BUF_TYPE *buf_ptr)
{return ((buf_ptr->tail)-(buf_ptr->head));
}



#ifndef __BUFFER_H			/* Avoid multiple inclusions */
#define __BUFFER_H

/*
 * Other includes
 */

#include <shortdef.h>		/* Short type definitions */

/*
 * General Definitions
 */

typedef struct buf_type
{
	char	*buf;
	uint	buf_len;
	int	head;
	int	tail;
} BUF_TYPE;

/*
 *  Macro Function Definitions
 *  The prototypes for the following are:
 *
 *			char	*get_char_ptr(BUF_TYPE *buf_ptr);
 *			uint	get_buffer_size(BUF_TYPE *buf_ptr);
 *			int	get_buffer_head(BUF_TYPE *buf_ptr);
 *			int	get_buffer_tail(BUF_TYPE *buf_ptr);
 *			uchar	buffer_empty(BUF_TYPE *buf_ptr);
 */

#define	get_char_ptr(x)		(&(x->buf[x->head]))
#define	get_buffer_size(x)	((x->buf_len))
#define	get_buffer_head(x)	((x->head))
#define	get_buffer_tail(x)	((x->tail))
#define	buffer_empty(x)		(((x)->head==(x)->tail)?(1):(0))

/*
 *  PUBLIC function prototypes
 *
 *			create_buffer	-- Allocate a buffer and return a pointer to it
 *			destroy_buffer	-- Destroy and free the buffer's memory
 *			reset_buffer	-- Resets the buffer structure to all NULLs
 *			get_char_from_buffer	-- Gets a char from a buffer and checks for wrap
 *			add_char_to_buffer	-- Adds a char to a buffer and checks for wrap
 */

BUF_TYPE	*create_buffer(uint buf_len);
void	destroy_buffer(BUF_TYPE *buf_ptr);
void	reset_buffer(BUF_TYPE *buf_ptr);
short	get_char_from_buffer(BUF_TYPE *buf_ptr);
int	reallocate_buffer(BUF_TYPE *buf_ptr, uint buf_len);
int	add_char_to_buffer(BUF_TYPE *buf_ptr, uchar c);
unsigned int	get_chars_in_buffer(BUF_TYPE *buf_ptr);

#endif							/* ifndef __BUFFER_H */

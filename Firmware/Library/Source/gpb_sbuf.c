#include "gpib.h"

extern volatile uchar	GPIB_DATA;
extern GPIB_BUF	*gpib_send_head;
extern uint			Gpib_status;

void	gpib_send_buffer(
BUF_TYPE	*buf_ptr)
{
	GPIB_BUF	*temp_gpib_buf;
	GPIB_BUF	*temp_send_ptr;

	if((temp_gpib_buf = malloc(sizeof(GPIB_BUF))) != NULL)
	{
		if(gpib_send_head == NULL)
		{
			gpib_send_head = temp_gpib_buf;
			gpib_send_head->next = NULL;
			gpib_send_head->gpib_buf = buf_ptr;
			if(Gpib_status & 0x1000)
				GPIB_DATA = (get_char_from_buffer(buf_ptr));
		}
		else
		{
			temp_send_ptr = gpib_send_head;
			while(temp_send_ptr->next != NULL)
				temp_send_ptr = temp_send_ptr->next;
			temp_send_ptr->next = temp_gpib_buf;
			temp_gpib_buf->gpib_buf = buf_ptr;
			temp_gpib_buf->next = NULL;
		}
	}
	else
		fprintf(stderr,"(FATAL) MEM SHORT - SND BUFF");
	gpib_set_status((MAV|SRQ));
}



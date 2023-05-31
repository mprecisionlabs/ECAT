#include "gpib.h"

extern volatile uchar	GPIB_INT_0;
extern volatile uchar	GPIB_INT_1;
extern volatile uchar	GPIB_ADDR_STAT;
extern volatile uchar	GPIB_AUX;
extern volatile uchar	GPIB_DATA;
extern volatile uint		EOIS;

extern GPIB_BUF	*current_gpib_buf;
extern GPIB_BUF	*gpib_send_head;
extern PF	reset_func;
extern PF	local_func;
extern PF	remote_func;
extern PF	group_func;
extern PF	spoll_func;

void interrupt	gpib_interrupt(void)
{
	short	c;
	uchar	status0;
	uchar	status1;
	GPIB_BUF	*temp_gpib_buf;
	BUF_TYPE	*temp_buf;

	asm(" movem.l a0-a7/d0-d7,-(sp)");
	status0 = (GPIB_INT_0);
	status1 = (GPIB_INT_1);

	/*--- status register 0 processing ---*/
	if(status0 & 0x20)								/* BI true */
	{
		c = (GPIB_DATA);
		add_char_to_buffer(current_gpib_buf->gpib_buf,c);
	}
	if(status0 & 0x08)								/* EOI received, BI true */
	{
		EOIS++;
		/*--- allocate next buffer ---*/
		if(((temp_gpib_buf = malloc(sizeof(GPIB_BUF))) != NULL) &&
			((temp_buf = create_buffer(GPIB_BUFFER_SIZE)) != NULL))
		{
			current_gpib_buf->next = temp_gpib_buf;
			current_gpib_buf = temp_gpib_buf;
			current_gpib_buf->gpib_buf = temp_buf;
			current_gpib_buf->next = NULL;
		}
		else
			fprintf(stderr,"(FATAL) MEM SHORT - EOI BUFF");
	}
	if(status0 & 0x10)
	{
		gpib_clr_status(MAV);
		if(gpib_send_head != NULL)				/* Data Out Register available */
		{
			c = get_char_from_buffer(gpib_send_head->gpib_buf);
			/*--- send char if char != -1 ---*/
			if(c != -1)
			{
				if(buffer_empty(gpib_send_head->gpib_buf))
				{
					GPIB_AUX = 0x08;				/* EOI */
/*****************
					if(gpib_send_head == NULL)
						gpib_clr_status(MAV);
*****************/
				}
				GPIB_DATA = c;
			}
			else
			{
				temp_gpib_buf = gpib_send_head->next;
				destroy_buffer(gpib_send_head->gpib_buf);
				free(gpib_send_head);
				gpib_send_head = temp_gpib_buf;
/*****************
				if(gpib_send_head == NULL)
					gpib_clr_status(MAV);
*****************/
			}
		}
/**************************
		else
		{										* Send requested and nothing to send *
			GPIB_AUX  = 0x08;
			GPIB_DATA = 0x00;
		}
**************************/
	}
	if(status0 & 0x02)					/* local/remote toggle */
	{
		if((GPIB_ADDR_STAT) & 0x80)
			((PF)(remote_func))();
		else
			((PF)(local_func))();
	}
	if(status0 & 0x04)					/* we were serial polled */
	{
		gpib_clr_status(SRQ);
		((PF)(spoll_func))();
	}

	/*--- status register 1 processing ---*/
	if(status0 & 0x40)					/* interrupt occured in INT 1 register */
	{
		if(status1 & 0x08)
		{										/* device clear received */
			((PF)(reset_func))();
			GPIB_AUX = 0x01;				/* Release the DAC holdoff */
		}
		if(status1 & 0x80)
		{
			((PF)(group_func))();
			GPIB_AUX = 0x01;				/* Release the DAC holdoff */
		}
	}
	asm(" movem.l (sp)+,a0-a7/d0-d7");
}


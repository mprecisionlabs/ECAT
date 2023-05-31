#include "gpib.h"

extern volatile uint	EOIS;
extern GPIB_BUF	*gpib_list_head;


BUF_TYPE	*gpib_get_buffer(void)
{
	BUF_TYPE	*return_buf;
	GPIB_BUF	*temp_buf;

	if(gpib_list_head->next != NULL)
	{
		return_buf = gpib_list_head->gpib_buf;
		temp_buf = gpib_list_head;
		gpib_list_head = gpib_list_head->next;
		free(temp_buf);
		EOIS--;
	}
	else
		return_buf = NULL;
	return(return_buf);
}


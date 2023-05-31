#include "gpib.h"

extern BUF_TYPE	*in_buff;
extern GPIB_BUF	*gpib_list_head;

int   gpib_char_avail(void)
{
   return((in_buff!=NULL) || (gpib_list_head->next != NULL));
}


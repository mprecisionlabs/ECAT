#include "gpib.h"

extern volatile ulong	vectors[];
extern volatile uchar	GPIB_INT_0;
extern volatile uchar	GPIB_INT_1;
extern volatile uchar	GPIB_ADDR_STAT;
extern volatile uchar	GPIB_AUX;
extern volatile uchar	GPIB_PAR_POLL;

void  show_all(void)
{
   printf("\n\rStatus 0    = %X\n\r",(int)(GPIB_INT_0));
   printf("Status 1    = %X\n\r",(int)(GPIB_INT_1));
   printf("Addr Status = %X\n\r",(int)(GPIB_ADDR_STAT));
   printf("BUS Status  = %X\n\r",(int)(GPIB_AUX));
   printf("Command     = %X\n\r",(int)(GPIB_PAR_POLL));
}


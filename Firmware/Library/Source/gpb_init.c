#include "gpib.h"

extern volatile ulong	vectors[];
extern volatile uchar	GPIB_INT_0;
extern volatile uchar	GPIB_INT_1;
extern volatile uchar	GPIB_ADDR_STAT;
extern volatile uchar	GPIB_AUX;
extern volatile uchar	GPIB_ADDR_REG;
extern volatile uchar	GPIB_SER_POLL;

GPIB_BUF	*gpib_list_head;
GPIB_BUF	*current_gpib_buf;
GPIB_BUF	*gpib_send_head;
BUF_TYPE	*out_buff;
BUF_TYPE	*in_buff;

volatile uint	EOIS;
uchar	spb_byte;
uint	Gpib_mask_0;
uint	Gpib_mask_1;
uint	Gpib_status;
PF		reset_func;
PF		local_func;
PF		remote_func;
PF		group_func;
PF		spoll_func;

/*------------------------------------------------------------------------*/
/* init_gpib                                                              */
/*                                                                        */
/* This function will:                                                    */
/*  - immediately set the software reset bit of the TMS9914A              */
/*  - read the gpib hardware address switch                               */
/*  - set/clear the primary/secondary addressing capability               */
/*  - initialize the serial and parallel poll registers to 0              */
/*  - initialize interrupt mask register 0 to allow all interrupts        */
/*  - initialize interrupt mask register 1 to:                            */
/*     - enable group execute trigger                                     */
/*     - disable error                                                    */
/*     - disable unrecognized command                                     */
/*     - disable address pass through (enable for secondary addressing)   */
/*     - enable device clear                                              */
/*     - disable service request                                          */
/*------------------------------------------------------------------------*/

void	gpib_init (
int	addr,
void	(*resetp)(),
void	(*go_remote)(),
void	(*go_local)(),
void	(*grp_trg)(),
void	(*spoll)())
{
	uchar	address;
	uint	sr;
	GPIB_BUF	*temp_gpib_buf;
	BUF_TYPE	*temp_buf;

	reset_func	= resetp;
	local_func	= go_local;
	remote_func	= go_remote;
	group_func	= grp_trg;
	spoll_func	= spoll;
	GPIB_AUX		= 0x80;					/* software reset mode */
	EOIS = 0;
	address = addr;
	Gpib_mask_0 = 0xFF;
	Gpib_mask_1 = 0x88;

	GPIB_ADDR_REG = (uchar)address;
	GPIB_INT_0 = (Gpib_mask_0);
	GPIB_INT_1 = (Gpib_mask_1);

	GPIB_AUX = 0x00;						/* take out of software reset */
	GPIB_SER_POLL = 0x00;				/* initialize serial poll register */

	/*--- initialize gpib receive buffer linked list ---*/
	if(((temp_gpib_buf = malloc(sizeof(GPIB_BUF))) != NULL) &&
		((temp_buf = create_buffer(GPIB_BUFFER_SIZE)) != NULL))
	{
		gpib_list_head = temp_gpib_buf;
		current_gpib_buf = temp_gpib_buf;
		gpib_list_head->gpib_buf = temp_buf;
		gpib_list_head->next = NULL;
		Gpib_status = 0;
	}
	else
		fprintf(stderr,"(FATAL) MEM SHORT - RCV BUFF");

	/*--- initialize gpib send buffer linked list ---*/
	gpib_send_head = NULL;
	vectors[(int)28] = (long)gpib_interrupt;
}


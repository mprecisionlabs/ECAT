#include <stdio.h>
#include <float.h>
#include "timer.h"
#include "vectors.h"

const tpu_mask = 0x0001;			/* The mask in reset_flags to mark init */
extern int  reset_flags;
extern volatile ushort	TPU_rams[16][8];
extern volatile uint	TPU_rami[16][4];
extern volatile ulong	vectors[];
extern volatile ushort	TCR1_READ;

volatile ulong	param1[16];		/* Misc variable used for channel storage */
volatile ulong	param2[16];		/* Misc variable used for channel storage */
volatile ulong	param3[16];		/* Misc variable used for channel storage */

volatile uchar		happened[16];
volatile TPU_MODE	mode[16];
volatile uchar		tpuFlag[16];
TPU_STRUCT	*TPU;						/* This is VOLATILE */

void	TPU_init(void)
{
	int	tvar;

	if(!(reset_flags & tpu_mask))
	{
		for(tvar = 0; tvar < 16; mode[tvar++] = NONE)
			tpuFlag[tvar] = 1;
		TPU = (TPU_STRUCT *)0xFFFE00;
		TPU->TMCR = 0x2040+TPUARB;					/* 0010 0000 0100 -- TPUARB */
		TPU->TICR = (TPU_IRQ_LEVEL*256)+(TPU_VECTOR_BASE);
		for(tvar = 0; tvar < 16; tvar++)
		{
			vectors[TPU_VECTOR_BASE+tvar] = (long)TPU_trap;
			vectors[(TPU_VECTOR_BASE+tvar)+256] = (long)TPU_trap;
		}
		reset_flags |= tpu_mask;
	}
}

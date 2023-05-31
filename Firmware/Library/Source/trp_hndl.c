#include "switches.h"

ushort	*eptr;
uint		*regs[17];
ushort	stkcnt;

extern volatile ushort	QILR_QIVR;
extern volatile uchar	int_active;


void badtrap(void)
{
	asm("  move.l  d0,_regs");
	asm("  move.l  d1,_regs+4");
	asm("  move.l  d2,_regs+8");
	asm("  move.l  d3,_regs+12");
	asm("  move.l  d4,_regs+16");
	asm("  move.l  d5,_regs+20");
	asm("  move.l  d6,_regs+24");
	asm("  move.l  d7,_regs+28");
	asm("  move.l  a0,_regs+32");
	asm("  move.l  a1,_regs+36");
	asm("  move.l  a2,_regs+40");
	asm("  move.l  a3,_regs+44");
	asm("  move.l  a4,_regs+48");
	asm("  move.l  a5,_regs+52");
	asm("  move.l  a6,_regs+56");
	asm("  move.l  a7,_regs+60");
	asm("  move.l  USP,a0");
	asm("  move.l  a0,_regs+64");

	eptr = (unsigned short *)asm("  move.l a7,d0");

	QILR_QIVR  = 0;
	int_active = 0;
	setleds(0x99);

	if(read_sw3() & 0x01)
	{
		printf("Exception: ");
		switch((*(eptr+0x03))&0xF000)
		{
			case 0x0000:	/* Format $0 - Four Word Stack Frame	*/
				printf("Four Word\n\r");
				printf("Format/Vector=%04X\n",*(eptr+0x03));
				break;
			case 0x2000:	/* Format $2 - Six Word Stack Frame		*/
				printf("Six Word\n\r");
				printf("Format/Vector=%04X\n",*(eptr+0x03));
				break;
			case 0xC000:	/* Format $C - BERR Stack Frame			*/
				printf("Bus Error\n\r");
				printf("Format/Vector=%04X\n",*(eptr+0x03));
				printf("SSW=%04X  Fault Addr=%04X%04X  data=%04X%04X  ",*(eptr+0x08),
							*(eptr+0x04),*(eptr+0x05),*(eptr+0x06),*(eptr+0x07));
				printf("Curr PC=%04X%04X  Cnt Reg=%04X\n\r",*(eptr+0x01),*(eptr+0x02),*(eptr+0x07));
				break;
			default:
				break;
		}
		printf("D0 = %08X   D1 = %08X   D2 = %08X   D3 = %08X\n\r",
					regs[0],regs[1],regs[2],regs[3]);
		printf("D4 = %08X   D5 = %08X   D6 = %08X   D7 = %08X\n\r",
					regs[4],regs[5],regs[6],regs[7]);
		printf("A0 = %08X   A1 = %08X   A2 = %08X   A3 = %08X\n\r",
					regs[8],regs[9],regs[10],regs[11]);
		printf("A4 = %08X   A5 = %08X   A6 = %08X   A7 = %08X\n\r",
					regs[12],regs[13],regs[14],regs[15]);
		printf("USER SP = %08X\n\r",regs[16]);
	}
	while(1)
		;
}

void interrupt	spurious(void)
{
	return;

	if(read_sw3() & 0x01)
		printf("Spurious\n");
}

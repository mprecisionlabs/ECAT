#include "key.h"

extern volatile ushort	key_control;
extern volatile ushort	key_data;
extern int	karray[];
extern int	khead;
extern int	ktail;

interrupt void	KEY_trap(void)
{
	int	i;
	int	j;

	i = (key_control & 7);
	while(i) 
	{
		key_control = (ushort)0x50;				/* 01010000 Read matrice data */
		j = (key_data & 0x3F);
		i = (key_control & 7);
		karray[khead++] = j;
		if(khead == MAX_KEYBUFF)
			khead = 0;
		if(khead == ktail)
			ktail++;
		if(ktail == MAX_KEYBUFF)
			ktail = 0;
	}
	key_control = (ushort)0xE0;					/* 11100000 Clear IRQ */
}

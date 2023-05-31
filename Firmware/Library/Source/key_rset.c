#include "key.h"

extern volatile ulong	vectors[];
extern volatile ushort	key_control;
extern volatile ushort	key_data;

ushort	led_array[4];					/* The state of the output LED's		*/
int	karray[MAX_KEYBUFF];				/* Keyboard buffer (circular)			*/
int	khead;								/* Pointer into the keyboard buffer	*/
int	ktail;								/* Pointer into the keyboard buffer	*/

void	KEY_reset(void)
{
	int	i;

	khead = 0;
	ktail = 0;
	vectors[(int)27] = (int)KEY_trap;

	TPU_delay(10);
	key_control = (unsigned short)0x02;			/* 8 chars with Encoded keys	*/
	TPU_delay(10);
	key_control = (unsigned short)0x34;			/* Program for a 1Mhz clock	*/
	TPU_delay(10);
	key_control = (unsigned short)0x90;			/* Reset display					*/
	TPU_delay(10);
	for(i = 0; i < 8; i++)
	{
		key_data = 0x00;
		led_array[i] = 0x00;
	}
	key_control = (unsigned short)0x90;
	for(i = 0; i < 8; i++)
		key_data = 0x00;
}

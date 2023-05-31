#include "key.h"

extern volatile ushort	key_control;
extern volatile ushort	key_data;
extern ushort	led_array[];

void	KEY_led(
int		led,
uchar	state)
{
	int	i;

	if(state)
		led_array[(led % 8)] |= (1 << (led / 8));
	else
		led_array[(led % 8)] &= (~(1 << (led / 8)));

	key_control = (unsigned short)0x90;
	for(i = 0; i < 8; i++)
	{
		key_data = led_array[i];
	}
}

#include "key.h"

extern ushort	led_array[];

int	KEY_ledstate(
int	led)
{
	if(led > 3)
		return(led_array[led-4] & 0x20);
	else
		return(led_array[led] & 0x10);
}

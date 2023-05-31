#include "key.h"


void	KEY_wait(void)
{
	while(!KEY_hit())
		;
	KEY_flush();
}

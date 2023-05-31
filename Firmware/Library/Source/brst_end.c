#include <float.h>
#include "burst.h"
#include "timer.h"

extern volatile PRIV_BURST	priv_burst[];
extern volatile char			priv_switch;

void	stop_burst(void)
{
	priv_burst[0].mode	= priv_burst[1].mode  = OFF;
	priv_burst[0].state	= priv_burst[1].state = 0;
	TPU_write(BURST_CHANNEL,0);
	TPU_write(LED_CHANNEL,0);
	TPU_kill(priv_burst[priv_switch].channel);
}

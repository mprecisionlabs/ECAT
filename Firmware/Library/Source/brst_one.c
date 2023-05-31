#include <float.h>
#include "burst.h"
#include "timer.h"

extern volatile PRIV_BURST	priv_burst[];
extern volatile char			priv_switch;

int	one_burst(
float	on_time,
float	period,
float	frequency,
uchar	type,
uint	phase)
{
	uint	phasetmp;
	int	phaseoff;

	priv_burst[0].mode		= priv_burst[1].mode		 = MOM;
	priv_burst[0].state		= priv_burst[1].state	 = 0;
	priv_burst[0].on_time	= priv_burst[1].on_time	 = (int)(on_time / (TPU_TICK * 2));
	priv_burst[0].off_time	= priv_burst[1].off_time = (int)((period - on_time) / TPU_TICK);
	priv_burst[0].period		= priv_burst[1].period	 = (int)(1/(frequency*TPU_TICK));
	priv_burst[0].type		= priv_burst[1].type		 = type;
	phasetmp = phase;
	phaseoff = ((1.0 / frequency) + 0.0003) * 21600;
	phasetmp -= phaseoff;
	if(phasetmp < 0)
		phasetmp = 360 + phasetmp;
	priv_burst[0].phase = priv_burst[1].phase = phasetmp;
	priv_switch = 0;
	return(TPU_delay_int(2096,doburst));
}

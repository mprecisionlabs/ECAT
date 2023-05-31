#include <float.h>
#include "burst.h"
#include "timer.h"

extern volatile PRIV_BURST	priv_burst[];
extern volatile char			priv_switch;

int	start_burst(
float	on_time,
float	period,
float	frequency,
uchar	type,
uint		phase,
uchar	mode)
{
	int	phaseoff;
	uint	phasetmp;

	priv_burst[0].mode		= priv_burst[1].mode		 = mode;
	priv_burst[0].state		= priv_burst[1].state	 = 0;
	priv_burst[0].on_time	= priv_burst[1].on_time	 = (int)(on_time/TPU_TICK) - 314;
	priv_burst[0].off_time	= priv_burst[1].off_time = (int)((period - on_time - (float)EXTRA_TICKS) / TPU_TICK);
	priv_burst[0].period		= priv_burst[1].period	 = (int)(1/(frequency*TPU_TICK));
	priv_burst[0].type		= priv_burst[1].type		 = type;

#undef	NO_SYNC_FIX
#ifdef	NO_SYNC_FIX
	phaseoff = (int)((((float)1.0 / frequency) + (float)0.002172) * (float)21600.0);
	phasetmp = (200 + phase) - phaseoff;
	priv_burst[0].phase = priv_burst[1].phase = phasetmp;
#else
	priv_burst[0].phase = priv_burst[1].phase = phase + 144;
#endif
	priv_switch = 0;
	return(TPU_delay_int(2096,doburst));
}

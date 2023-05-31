#include <float.h>
#include "burst.h"
#include "timer.h"

extern volatile PRIV_BURST	priv_burst[];
extern volatile char			priv_switch;

void	update_burst(
float	on_time,
float	period,
float	frequency,
uint		phase,
uchar	type)
{
	PRIV_BURST	*pb_ptr;
	int	phaseoff;
	uint	phasetmp;

	pb_ptr = &(priv_burst[((priv_switch)?(0):(1))]);
	priv_burst[0].on_time	= priv_burst[1].on_time	 = (int)(on_time/TPU_TICK) - 314;
	pb_ptr->off_time	= (int)((period - on_time - (float)EXTRA_TICKS) / TPU_TICK);
	pb_ptr->period		= (int)(1/(frequency*TPU_TICK));
#undef	NO_SYNC_FIX
#ifdef	NO_SYNC_FIX
	phaseoff = (int)((((float)1.0 / frequency) + (float)0.002172) * (float)21600.0);
	phasetmp = (200 + phase) - phaseoff;
	pb_ptr->phase	= phasetmp;
#else
	pb_ptr->phase	= phase + 144;
#endif
	pb_ptr->type	= type;
	priv_switch = (priv_switch)?(0):(1);
}


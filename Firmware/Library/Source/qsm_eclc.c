#include "qsm.h"

extern int	AD_COUNTS[];
extern int	AD_SET_COUNTS[];

void	DAC_stop_calc(
int	ch)
{
	AD_SET_COUNTS[ch] = 0;
	AD_COUNTS[ch] = 0;
}


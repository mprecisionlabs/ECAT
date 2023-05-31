#include <float.h>
#include <limits.h>
#include "qsm.h"

extern int	AD_AVG[];
extern int	AD_SET_COUNTS[];

float	DAC_avg(
int	ch)
{
	if((AD_SET_COUNTS[ch]) && (AD_AVG[ch]!=INT_MIN))
		return((float)AD_AVG[ch] / (float)256.0);
	else
		return((float)-1);
}


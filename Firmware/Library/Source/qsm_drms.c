#include <float.h>
#include <math.h>
#include <limits.h>
#include "qsm.h"

extern int	AD_RMS[];
extern int	AD_AVG[];
extern int	AD_SET_COUNTS[];

float	DAC_rms(
int	ch)
{
	if((AD_SET_COUNTS[ch]) && (AD_AVG[ch]!=INT_MIN) && (AD_RMS[ch]!=INT_MIN))
		return(sqrt((float)AD_RMS[ch]));
	else
		return((float)0);
}


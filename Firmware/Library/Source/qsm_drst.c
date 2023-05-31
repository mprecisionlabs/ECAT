#include <limits.h>
#include "qsm.h"

extern int	AD_MAX[];
extern int	AD_MIN[];

void	DAC_reset(
int	ch)
{
	AD_MAX[ch] = INT_MIN;
	AD_MIN[ch] = INT_MAX;
}


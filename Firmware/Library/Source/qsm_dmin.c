#include "qsm.h"

extern int	AD_MIN[];

int	DAC_min(
int	ch)
{
	return((int)AD_MIN[ch]);
}


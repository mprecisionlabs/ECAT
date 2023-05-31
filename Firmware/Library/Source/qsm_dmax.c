#include	"qsm.h"

extern int	AD_MAX[];

int	DAC_max(
int	ch)
{
	return((int)AD_MAX[ch]);
}


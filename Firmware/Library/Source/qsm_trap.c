#include <limits.h>
#include "qsm.h"

extern volatile ushort	QSM_rx[];
extern volatile uchar   SPSR;
extern int	AD_MAX[];
extern int	AD_MIN[];
extern int	AD_RMS[];
extern int	AD_AVG[];
extern int	AD_WORKING_RMS[];
extern int	AD_WORKING_AVG[];
extern int	AD_COUNTS[];
extern int	AD_SET_COUNTS[];
extern volatile uchar	AD_HIT;

interrupt void	QSM_trap(void)
{
	int	i,j;

	asm(" movem.l a0-a7/d0-d7,-(sp)");
	AD_HIT = 1;
	i = SPSR;
	SPSR = 0;
	for(i = 0; i < 8; i++)
		if(AD_SET_COUNTS[i])
		{
			j = QSM_rx[i+1];
			if(j > AD_MAX[i])
				AD_MAX[i] = j;
			if(j < AD_MIN[i])
				AD_MIN[i] = j;
			AD_WORKING_AVG[i] += j;
			j = j - (AD_AVG[i] / 256);
			AD_WORKING_RMS[i] += j * j;
			AD_COUNTS[i]--;
			if(AD_COUNTS[i] == 0)
			{
				if(AD_AVG[i] != INT_MIN)
					AD_RMS[i] = AD_WORKING_RMS[i];
				AD_AVG[i] = (AD_WORKING_AVG[i] * 256) / AD_SET_COUNTS[i];
				AD_COUNTS[i] = AD_SET_COUNTS[i];
				AD_WORKING_RMS[i] = AD_WORKING_AVG[i] = 0;
			}
		}
	asm(" movem.l (sp)+,a0-a7/d0-d7");
}

#include <limits.h>
#include "qsm.h"

extern int	AD_RMS[];
extern int	AD_AVG[];
extern int	AD_COUNTS[];
extern int	AD_SET_COUNTS[];

/*
 *  start_ad_calc -Starts doing extended calculations for A/D channels.
 *
 *       Syntax: void start_ad_calc(int channel, int count);
 *  Description: Starts doing RMS, Max, Min and average calculations for the
 *               A/D input number 'channel'. 'count' specifies how many
 *               A/D samples should be included in each RMS calculation.
 *   Exceptions: The QSM_init() function must have been called before
 *               the A/D values will have any meaning.
 *  Source Code: QSM.C
 *      Include: QSM.H
 */

void	start_ad_calc(
int	ch,
int	counts)
{
	AD_SET_COUNTS[ch] = counts;
	AD_COUNTS[ch] = counts;
	AD_RMS[ch] = INT_MIN;
	AD_AVG[ch] = INT_MIN;
}


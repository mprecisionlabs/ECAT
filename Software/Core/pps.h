/****************************************************************************

PPS.H -- Header file with constants for PPS calculations:  originally for
		E421P module.  Has CE40 constants also;  functions called from
		E400.cpp dialog proc.

History:
	8/28/95	JFL	Add max values for CE40.

****************************************************************************/

#include "module.h"
#include "globals.h"

const int DUR_MAXMS  = 20;
const long DUR_MAXPLS = 200;
const long PPS_MAXPLS = 10000;
const long CE40_MAXMS = 15;
const long CE40_MAXPLS = 150;

struct PPS_data
{
	float max_volt;
	float max_pulses;
};


const PPS_data *Is_PPS();

float MaxDuration(BOOL is_pulses,float freq,float voltage,float period);

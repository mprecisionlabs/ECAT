/***************************************************************************

PPS.CPP -- Duration routines, originally for PPS421.  (E421P module)

History:
	8/28/95	JFL	Added changes due to differences on CE40 box (max 150 pulses)

***************************************************************************/
#include "stdafx.h"
#include "pps.h"

const PPS_data pps421[] = 
	{{2000,10000},{3000,5000},{4400,2500},{8000,600},{0,0}};

const PPS_data *Is_PPS()
{
	if (calset[EFT_MODULE].id == E421P_BOXT)
	     return pps421; 
	else return 0;
}

float MaxDuration(BOOL is_pulses,float volt,float freq,float period)
{

  const PPS_data *pps_data = Is_PPS();

	// not PPS (normal case)

	if (!pps_data)
		{
		if (is_pulses)
			if (!IS_CE_BOX) 		return (float)DUR_MAXPLS;  	// ECAT, pulses
			else 						return (float)CE40_MAXPLS;	// CE40, pulses
		else if (IS_CE_BOX) 		return (float)CE40_MAXMS;	// CE40, msec mode
		else	          			return (float)DUR_MAXMS;    // ECAT, msec mode
		}
		
	// remainder for E421PPS only

	if (is_pulses)
	   freq *= 1000;
	
	long pulses = DUR_MAXPLS;
        
	while (volt > pps_data->max_volt)
	   pps_data++;
	if (pps_data->max_volt)
	   pulses = (long)pps_data->max_pulses;      //max pps

	if (!(freq > pulses || period > 1000))  //if (freq>pps && period>1sec) 
 	{				       								
	   if (freq < pulses) pulses = (long)freq;
	   pulses = pulses * (long) period / 1000;	 //max dur. in pulses
	   if (pulses > PPS_MAXPLS) pulses = PPS_MAXPLS; //limit to 10000
	}

	if (is_pulses)
	   return (float)(pulses);
	else
	   return (pulses * 1000 / freq);      // max duratiom in ms
}

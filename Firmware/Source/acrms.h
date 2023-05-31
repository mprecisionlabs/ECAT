#ifndef __ACRMS_H				/* Avoid multiple inclusions */
#define __ACRMS_H

#include "shortdef.h"

#define EUTBLUE_TIMEOUT	8000	     		/* timeout eut blue switch goes on 	*/

/* factor to convert A/D value to amps */
#define HIRANGE_FACTOR 	40.96	    		/* relay RD14 is on		      		*/
#define LORANGE_FACTOR 	2.048	    		/* relay RD14 is off						*/
#define MAX_AC_PEAK	   1000
#define RMS_PEAK_INIT	10000

#define AC_IRANGE			(0x00004000)	/* RLY_RD14									*/

#define ACSTART_PERIOD  12

#define AmpsPeak(p)  ((p)>=0 ? (p)/HIRANGE_FACTOR+0.05 : (p)/HIRANGE_FACTOR-0.05)

typedef	struct tag_rmsdata				/* ADS_TRAP data structure 			*/
{
 	int    zero;			/* zero offset   				*/
 	int    rms_n;        /* public num over period 	*/
 	int    rms_sum;      /* public sum over period 	*/
 	int    peakp;  		/* peak plus  over period	*/
 	int    peakm;  		/* peak minus over period	*/
	int	 peakmax;		/* peak max over runtime 	*/
	int	 peakmin;		/* peak min over runtime 	*/
	ushort ad_ch;  		/* A/D channel + BIPP or 0	*/
 
} RMSDATA;

int  RMS_DefLim(void);
void RMS_Stop(void);
void RMS_Start(void);
void RMS_Enable(void);
void RMS_Disable(void);
void RMS_Init(void);
void RMS_ResetModules(void);
void RMS_SetNetwork(uchar bay);
void RMS_Main(void);
int  RMS_BAY_rpt(P_PARAMT *params);
int  RMS_DEFLIM_rpt(P_PARAMT *params);
int  RMS_PITR_pgm(P_PARAMT *params);
int  RMS_PLIM_pgm(P_PARAMT *params);
int  RMS_SMAX_pgm(P_PARAMT *params);
int  RMS_SMIN_pgm(P_PARAMT *params);
int  RMS_LIM_rpt(P_PARAMT *params);
int  RMS_StatusRep(P_PARAMT *params);
int  RMS_Test_rpt(P_PARAMT *params);
void show_state_rmsbad(void);

#endif								/* ifndef ACRMS_H */

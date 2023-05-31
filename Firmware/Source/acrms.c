#include <stdio.h>
#include <mriext.h>
#include "parser.h"
#include "intface.h"
#include "ecat.h"
#include "timer.h"
#include "front.h"
#include "period.h"
#include "acrms.h"
#include "display.h"
#include "switches.h"
#include "pqf.h"
#include "emc.h"

extern volatile CHGDATA	chgdata;
extern FILE		*destination;
extern MODDATA	moddata[];
extern CPLDATA	cpldata;
extern ACDATA		acdata;
extern EMCDATA	emcdata;
extern SRGDATA	srgdata;
extern EFTDATA	eftdata;

/******* variables used in ADS_TRAP *************/

extern volatile int SYNC_RISING_EDGE;
extern uint	ADRMS_SRG_trap;
extern uint	ADRMS_SRG_END;
extern ushort	*ADADD;

extern volatile ushort	AD_CH;
extern volatile int	RMSCNT;
extern volatile int	RMS_PEAKP;
extern volatile int	RMS_PEAKM;
extern volatile int	RMS_N;
extern volatile int	RMS_SUM;

/* RMSDATA structure field offsets */
extern volatile ushort	OFF_ADCH;
extern volatile ushort	OFF_ZERO;
extern volatile ushort	OFF_RMSN;
extern volatile ushort	OFF_RMSSUM;
extern volatile ushort	OFF_PEAKP;
extern volatile ushort	OFF_PEAKM;
extern volatile ushort	OFF_PEAKMAX;
extern volatile ushort	OFF_PEAKMIN;

/* RMSDATA structures */
extern RMSDATA	RMS_L1;
extern RMSDATA	RMS_L2;
extern RMSDATA	RMS_L3;

extern volatile RMSDATA	*ACTIVE_RMS;			/* Active line RMSDATA	*/
static RMSDATA	*rmsdata[3];						/* Addresses of RMSDATA	*/

/************************************************/
extern P_PARAMT	paraml[3];
extern DISDATA	display;
extern uchar		noneTrack;
extern uchar		ecatmode;
extern uchar		(*func_manager)(char);

extern volatile ulong	param1[];
extern volatile ulong	param2[];
extern volatile ulong	param3[];
extern volatile ushort	TPU_rams[16][8];

char	ac_linecnt;									/* Number of AC lines to monitor	*/
static uchar	savACbox;

/* peak & RMS limits in A/D units */
static int		ad_peaklim;
static float	ad_rmsmaxlim;
static float	ad_rmsminlim;

void	RMS_CheckAc(void);
void	RMS_UpdateAdLimits(void);

void	SRG_UpdateAcPeaks(void);
void	EFT_UpdateAcPeaks(void);
void	RMS_ProcessError(void);
void	blue_off(void);

/******* AC limits in A/D units,depending on Irange ******/

void	RMS_UpdateAdLimits(void)
{
	ad_peaklim		= acdata.peak_lim * HIRANGE_FACTOR;
	ad_rmsmaxlim	= acdata.rmsmax_lim * HIRANGE_FACTOR;
	ad_rmsmaxlim	*= ad_rmsmaxlim;
	ad_rmsminlim	= acdata.rmsmin_lim * HIRANGE_FACTOR;
	ad_rmsminlim	*= ad_rmsminlim;
}

int	RMS_DefLim(void)					/* Default RMS limit	*/
{
	int	deflim = 0;

	if(acdata.acbox != GHOST_BOX)
		deflim = moddata[acdata.acbox].modinfo.calinfo.data[CAL_RMSMAX_OFF];
	return(deflim);
}

void	RMS_Init(void)
{
	acdata.acbox		= GHOST_BOX;
	savACbox				= GHOST_BOX;
	acdata.flag			= 0;
	acdata.error		= 0;
	acdata.pit_cnt		= 0x02;
	acdata.peak_lim	= 20000;
	acdata.rmsmax_lim	= 20000;
	acdata.rmsmin_lim	= 0;
	acdata.line			= 0;
	acdata.measure_on	= FALSE;

	rmsdata[0] = &RMS_L1;
	rmsdata[1] = &RMS_L2;
	rmsdata[2] = &RMS_L3;
}

void	RMS_SetNetwork(
uchar	bayaddr)
{
	int	deflim;
	int	cnt;

	if(emcdata.modeFlag && emcdata.mode != EMC_SRGEFT)	/* Flicker or Harm */
		return;
	if(!acdata.flag || acdata.acbox == bayaddr)
		return;

	if(acdata.acbox != GHOST_BOX)
	{
		RMS_Stop();
		acdata.acbox = GHOST_BOX;
	}
	acdata.error = 0;

	if(bayaddr != GHOST_BOX)
		if(moddata[bayaddr].modinfo.options & OPT_ACRMS)
		{
			acdata.acbox = bayaddr;
			if(moddata[bayaddr].modinfo.typePrimary & TYPE_3PHASE)
				ac_linecnt = 3;
			else
				ac_linecnt = 2;

			cnt = (moddata[bayaddr].modinfo.viMonitors) & 0x0f;
			if(cnt < ac_linecnt)
				ac_linecnt = cnt;
			if(ac_linecnt == 0)
				acdata.acbox = GHOST_BOX;
		}
	if(acdata.acbox != GHOST_BOX)
	{
		if(savACbox != acdata.acbox)
		{
			acdata.line			= 0;
			acdata.peak_lim	= 20000;
			acdata.rmsmin_lim	= 0;
			if((deflim = RMS_DefLim()) > 0)
				acdata.rmsmax_lim = deflim;
			savACbox = acdata.acbox;
		}
		*cpldata.srcrelay |= AC_IRANGE;
		ECAT_WriteRelay0(cpldata.srcaddr,*cpldata.srcrelay);
		RMS_UpdateAdLimits();	     /* Update peak & RMS limits in A/D units */
	}
}

void	RMS_ResetModules(void)
{
	RMS_SetNetwork(cpldata.srcaddr);
}

void	RMS_Start(void)		   					/* Function to start ADS_Trap */
{
	uint	*src_adr;
	uint	*dest_adr;
	char	l;

	RMS_Stop();
	TPU_delay(100);
	acdata.error = 0;
	TPU_sync_start(PHASE_TPU);						/* Start sync channel */
	PIT_kill(0);

	/* Initialize ADS_TRAP offsets & data */
	OFF_ADCH		= (uint)&rmsdata[0]->ad_ch		- (uint)rmsdata[0];
	OFF_RMSN		= (uint)&rmsdata[0]->rms_n		- (uint)rmsdata[0];
	OFF_ZERO		= (uint)&rmsdata[0]->zero		- (uint)rmsdata[0];
	OFF_RMSSUM	= (uint)&rmsdata[0]->rms_sum	- (uint)rmsdata[0];
	OFF_PEAKP	= (uint)&rmsdata[0]->peakp		- (uint)rmsdata[0];
	OFF_PEAKM	= (uint)&rmsdata[0]->peakm		- (uint)rmsdata[0];
	OFF_PEAKMAX	= (uint)&rmsdata[0]->peakmax	- (uint)rmsdata[0];
	OFF_PEAKMIN	= (uint)&rmsdata[0]->peakmin	- (uint)rmsdata[0];

	ADADD = (ushort *)(ADC_CONTROL+(acdata.acbox*0x1000));	/* A/D adr */

	rmsdata[0]->ad_ch = 4 + MAX180_BIPP;
	rmsdata[1]->ad_ch = 5 + MAX180_BIPP;
	rmsdata[2]->ad_ch = 6 + MAX180_BIPP;

	for(l = 2; l >= 0; l--)
	{
		if((l+1) == ac_linecnt)
			break;
		else
			rmsdata[l]->ad_ch = 0;					/* Init A/D channel numbers */
	}

	for(l = 0; l < ac_linecnt; l++)
	{
		rmsdata[l]->peakmax	= -RMS_PEAK_INIT;
		rmsdata[l]->peakmin	= RMS_PEAK_INIT;
		acdata.ldata[l].peakm	= 0;
		acdata.ldata[l].peakp	= 0;
		acdata.ldata[l].rms		= 0;
	}

	/* Copy ADS_TRAP code to 2K RAM */
	src_adr  = &ADRMS_SRG_trap;								/* ROM code adr	*/
	dest_adr = (uint *)0x00C0100;								/* RAM code adr	*/
	while (src_adr <= &ADRMS_SRG_END)
	{
		*dest_adr = *src_adr;									/* Copy code		*/
		src_adr++;
		dest_adr++;
	}

	acdata.checker		= 0;
	acdata.cycle_cnt	= 0;
	RMS_Enable();
}

/****** Function could be called, if ADS_TRAP was disabled,   *****/
/****** but AC power stays on                                 *****/
void	RMS_Enable(void)
{
	char	l;

	PIT_kill(0);

	/* Initialize ADS_TRAP data	*/
	ACTIVE_RMS	= rmsdata[0];
	AD_CH			= ACTIVE_RMS->ad_ch;
	ECAT_MAX180Write(acdata.acbox,AD_CH);			/* Start A/D conversion	*/

	RMSCNT		= 0;
	RMS_N			= 0;
	RMS_SUM		= 0;
	RMS_PEAKP	= -RMS_PEAK_INIT;
	RMS_PEAKM	= RMS_PEAK_INIT;

	for(l = 0; l < ac_linecnt; l++)
	{
		rmsdata[l]->zero		= 0;
		rmsdata[l]->rms_n		= 0;
		rmsdata[l]->rms_sum	= 0;
		rmsdata[l]->peakp		= -RMS_PEAK_INIT;
		rmsdata[l]->peakm		= RMS_PEAK_INIT;
	}
	SYNC_RISING_EDGE = 0;								/* Clear start of cycle		*/
	PIT_init_long(acdata.pit_cnt,0xC0100);			/* Start PIT with ADS_TRAP	*/
	acdata.measure_on = TRUE;
}

void	RMS_ProcessError(void)
{
	blue_off();
	switch(ecatmode)
	{
		case SRG_MODE:
			SRG_StopSystem();
			break;
		case EFT_MODE:
			EFT_StopSystem();
			break;
	}
	if(display.current != DISPLAY_REMOTE)
		display.request = DISPLAY_BAD_RMS;			/* Display AC error	*/
}

void	RMS_CheckAc(void)
{
	static uint	timer;

	if(acdata.checker >= ACSTART_PERIOD && acdata.checker != timer)
	{
		timer = acdata.checker;
		if(acdata.cycle_cnt == RMSCNT)
			TPU_delay(70);
		if(acdata.cycle_cnt == RMSCNT)
		{
			/* MODIFICATION - 12/14/95 (JMM) TEMP fix for AC/DC problem	*/
			/* IS DC option available and DC present? */
			if((moddata[(char)(acdata.acbox)].modinfo.options & OPT_DC_SURGE) &&
				((ECAT_ReadDINByte(acdata.acbox)) & 0x02))
					acdata.cycle_cnt = RMSCNT = 0;
			else
				acdata.error = NO_AC;							/* NO sync interrupt */
		}
		else
			acdata.cycle_cnt = RMSCNT;
	}
}

void	RMS_Disable(void)
{
	acdata.measure_on = FALSE;
	PIT_kill(0);
}

void	RMS_Stop(void)
{
	RMS_Disable();
	TPU_kill(PHASE_TPU);
}

/***** Main function to perform RMS calculations, when ADS_TRAP is on */  
void	RMS_Main(void)
{
	int			s,sum,l,num;
	ACLINEDATA	*acptr;
	RMSDATA		*rmsptr;
	static uint	cycle;

	if(!acdata.measure_on && cpldata.eutblue)
		RMS_Start();

	if(acdata.measure_on && !cpldata.eutblue)
		RMS_Stop();

	if(acdata.time == 0)
	{
		acdata.time = 4;
		if(eftdata.eftflag && display.current == DISPLAY_EFT)
			EFT_UpdateAcPeaks();
		else
			if(srgdata.srgflag && display.current == DISPLAY_SRG && acdata.line)
				SRG_UpdateAcPeaks();
	}
	if(acdata.error || !acdata.measure_on)
		return;

	RMS_CheckAc();
	if(!acdata.error)
		if(RMSCNT > ac_linecnt && cycle != RMSCNT)
		{
			cycle = RMSCNT;
			for (l = 0; l < ac_linecnt; l++)
			{
				rmsptr = rmsdata[l];
				acptr  = &acdata.ldata[l];
				acptr->peakm = rmsptr->peakm;
				acptr->peakp = rmsptr->peakp;
				do
				{
					sum= rmsptr->rms_sum;
					num= rmsptr->rms_n;
					s	= rmsptr->rms_sum;
				}
				while(s != sum);					/* Peaks over period from rmsdata */
				/* Calc RMS(sqrt only when output) */
				acptr->rms = (float)sum / (float)num;
				if(!acdata.error)
					if(rmsptr->peakmin <-ad_peaklim || rmsptr->peakmax >ad_peaklim ||
						acptr->rms > ad_rmsmaxlim    || acptr->rms < ad_rmsminlim)
						acdata.error = OVER_RMS;			/* Over limit RMS error	*/
			}
		}
	if(acdata.error)
		RMS_ProcessError();
}

int	RMS_PITR_pgm(P_PARAMT *params)		/* Set pit_cnt for ADS_TRAP	*/
{
	acdata.pit_cnt=EXTRACT_INT(0);
	return(0);
}

int	RMS_PLIM_pgm(P_PARAMT *params)		/* Set Peak limit					*/
{
	char	l;

	if(EXTRACT_INT(0) < 0)
		return(BAD_VALUE);

	acdata.peak_lim = EXTRACT_INT(0);
	RMS_UpdateAdLimits();			/* Update peak & RMS limits in A/D units */
	for(l = 0; l < ac_linecnt; l++)
	{
		rmsdata[l]->peakmin = RMS_PEAK_INIT;
		rmsdata[l]->peakmax = -RMS_PEAK_INIT;
	}
	return(0);
}

int	RMS_SMAX_pgm(P_PARAMT *params)		/* Set RMS max limit				*/
{
	if(EXTRACT_INT(0) < 0)
		return(BAD_VALUE);

	acdata.rmsmax_lim = EXTRACT_INT(0);
	RMS_UpdateAdLimits();			/* Update peak & RMS limits in A/D units */
	return(0);
}

int	RMS_SMIN_pgm(P_PARAMT *params)		/* Set RMS min limit				*/
{
	if(EXTRACT_INT(0) < 0)
		return(BAD_VALUE);

	acdata.rmsmin_lim = EXTRACT_INT(0);
	RMS_UpdateAdLimits();			/* Update peak & RMS limits in A/D units */
	return(0);
}

int	RMS_LIM_rpt(P_PARAMT *params)			/* Peak & RMS limits report	*/
{
	fprintf(destination,"%d,%d,%d",(int)acdata.peak_lim,
				(int)acdata.rmsmax_lim, (int)acdata.rmsmin_lim);
	return(0);
}

int	RMS_DEFLIM_rpt(P_PARAMT *params)		/* Default RMS limit report	*/
{
	fprintf(destination,"%d",RMS_DefLim());
	return(0);
}

int	RMS_BAY_rpt(P_PARAMT *params)			/* RMS bay report					*/
{
	fprintf(destination,"%d",acdata.acbox);
	return(0);
}

int	RMS_StatusRep(P_PARAMT *params)
{
	uchar	statflag;
	int	EUTV;
	int	rms[3],peak[3],l;
	float	peakmin,peakmax;

	if(cpldata.eutrequest && cpldata.eutblue)
		EUTV = 2;
	else
		EUTV = cpldata.eutrequest;
	statflag = ECAT_StatusFlag();

	if(display.current == DISPLAY_REMOTE)
	{
		if(acdata.error == NO_AC)
			statflag |= 0x08;
		if(acdata.error == OVER_RMS)
			statflag |= 0x10;
	}
	if((display.current != DISPLAY_REMOTE) || (acdata.acbox == GHOST_BOX) ||
		(acdata.error == NO_AC) || (!acdata.measure_on  && !acdata.error))
		fprintf(destination,"%d -1 -1 -1 %d -1 -1 -1 1234",statflag,EUTV);
	else
	{
		for(l = 0; l < ac_linecnt; l++)
		{
			if(rmsdata[l]->peakmin == RMS_PEAK_INIT)
				peakmin = 0;
			else
				peakmin = abs(rmsdata[l]->peakmin);
			if(rmsdata[l]->peakmax ==-RMS_PEAK_INIT)
				peakmax = 0;
			else
				peakmax = abs(rmsdata[l]->peakmax);
			if(peakmin > peakmax)
				peakmax = peakmin;

			peak[l] = (int) (peakmax / HIRANGE_FACTOR * 10 + 0.5);
			rms[l]  = (int)(sqrt(acdata.ldata[l].rms)/HIRANGE_FACTOR * 10 + 0.5);
			if(rmsdata[l]->peakmin >= -ad_peaklim && rmsdata[l]->peakmax <= ad_peaklim)
			{		/* NO limithit */
				rmsdata[l]->peakmin =  RMS_PEAK_INIT;
				rmsdata[l]->peakmax = -RMS_PEAK_INIT;
			}
		}
		for(l = ac_linecnt; l < 3; l++)
		{
			rms[l]  = -1;
			peak[l] = -1;
		}
		fprintf(destination,"%d %d %d %d %d %d %d %d 1234",statflag,
					peak[0],rms[0],peak[1],EUTV,rms[1],peak[2],rms[2]);
	}
	acdata.error = 0;
	return(0);
}

/********** Show AC error screen **********/  
void	show_state_rmsbad(void)
{
	float	peakmin,peakmax,rms;
	int	oy,l;

	KEY_beep();
	LCD_gotoxy(0,0);

	switch(ecatmode)
	{
		case SRG_MODE:
			noneTrack = DISPLAY_SRG;
			break;
		case EFT_MODE:
			noneTrack = DISPLAY_EFT;
			break;
		default:
			noneTrack = DISPLAY_LOCAL;
	}
	if(acdata.error == NO_AC)
		LCD_puts("\n\r\n\r\n\r   ERROR!   Input AC went away");
	else
	{
		fprintf(DISPLAY,"   ERROR! AC current exceeded limits:\n\r");
		for(l = 0; l<ac_linecnt; l++)
		{
			if(rmsdata[l]->peakmin == RMS_PEAK_INIT)
				peakmin = 0;
			else
				peakmin = AmpsPeak(rmsdata[l]->peakmin);
			if(rmsdata[l]->peakmax == -RMS_PEAK_INIT)
				peakmax = 0;
			else
				peakmax = AmpsPeak(rmsdata[l]->peakmax);

			rms = sqrt(acdata.ldata[l].rms);
			fprintf(DISPLAY,"\n\rL%d Peaks: %+7.1fA %+7.1fA  RMS:%6.1fA",l+1,
						peakmax,peakmin,AmpsPeak(rms));
		}
		for(l = 0, oy = 2; l < ac_linecnt; l++, oy += 2)
		{
			if(rmsdata[l]->peakmax > ad_peaklim)
				LCD_hilight(19,oy,26,oy);
			if(rmsdata[l]->peakmin <-ad_peaklim)
				LCD_hilight(10,oy,17,oy);
			if(acdata.ldata[l].rms > ad_rmsmaxlim || acdata.ldata[l].rms < ad_rmsminlim)
				LCD_hilight(33,oy,39,oy);
		}
	}
	acdata.error = 0;
	func_manager = none_funcs;
}

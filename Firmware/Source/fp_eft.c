#include <float.h>
#include <math.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "display.h"
#include "parser.h"
#include "front.h"
#include "module.h"
#include "ecat.h"
#include "intface.h"
#include "timer.h"
#include "eft.h"
#include "field.h"
#include "acrms.h"

extern uchar	saveft;
extern uchar	calibration;
extern uchar	(*func_manager)(char);
extern MODDATA	moddata[];
extern CPLDATA	cpldata;
extern EFTDATA	eftdata;
extern DISDATA	display;
extern ACDATA	acdata;
extern char	    ac_linecnt;
extern ONE_FIELD field_list[];
extern P_PARAMT	paraml[3];
extern char		*cplSRCnames[MAX_MODULES+1];
extern uchar	cplSRCindex[MAX_MODULES+1];
extern char		*listLSMmodes[];
extern char		*listPKSnames[5];

NUMERIC_REC	    fp_eftVLT;
NUMERIC_REC	    fp_eftFRQ;
NUMERIC_REC	    fp_eftDUR;
NUMERIC_REC	    fp_eftPER;
NUMERIC_REC	    fp_eftANG;
BOOL_REC		fp_eftMDE;
BOOL_REC		fp_eftWAV;
BOOL_REC		fp_eftUNT;
BOOL_REC		fp_eftEUT;
BOOL_REC		fp_eftCL1;
BOOL_REC		fp_eftCL2;
BOOL_REC		fp_eftCL3;
BOOL_REC		fp_eftCNU;
BOOL_REC		fp_eftCPE;
LIST_REC		fp_eftOUT;
LIST_REC		fp_eftEFT;
LIST_REC		fp_eftSRC;
LIST_REC		fp_eftLSM;
LIST_REC		fp_eftPKS;

extern NUMERIC_REC	fp_rmsPKL;
extern NUMERIC_REC	fp_rmsMIN;
extern NUMERIC_REC	fp_rmsMAX;

LIST_REC		fp_calEFT;
NUMERIC_REC	fp_cal_CLAMP_0250;
NUMERIC_REC	fp_cal_CLAMP_4400;
NUMERIC_REC	fp_cal_CLAMP_4401;
NUMERIC_REC	fp_cal_CLAMP_8000;
NUMERIC_REC	fp_cal_CPL_0250;
NUMERIC_REC	fp_cal_CPL_4400;
NUMERIC_REC	fp_cal_CPL_4401;
NUMERIC_REC	fp_cal_CPL_8000;

EFTDATA	savEFTdata;
CPLDATA	savLINEcpl;

char		*eftEFTnames[MAX_MODULES+1];
uchar	 eftEFTindex[MAX_MODULES+1];
char		*eftOUTnames[MAX_MODULES+3];
uchar	 eftOUTindex[MAX_MODULES+3];

void	EFT_EFTchanged();
void	EFT_OUTchanged();
void	EFT_MDEchanged();
void	EFT_WAVchanged();
void	EFT_VLTchanged();
void	EFT_FRQchanged();
void	EFT_UNTchanged();
void	EFT_DURchanged();
void	EFT_PERchanged();
void	EFT_LSMchanged();
void	EFT_ANGchanged();
void	EFT_SRCchanged();
void	EFT_EUTchanged();
void	EFT_CPLchanged();
void	EFT_PKSchanged();
void	RMS_PKLchanged();
void	RMS_MINchanged();
void	RMS_MAXchanged();
void	EFT_SetPeakNames(void);
void	EFT_UpdateAcPeaks(void);
void	CAL_EFTchanged();
void	CAL_EFTDUMchanged();
void	CAL_EFT9346WRITE();
void	CAL_EFT9346RESET();
void	CAL_EFT9346BASE();
void EFT_DURcheck(void);

void	EFT_EFTchanged()
{
	int	ox,oy;

	paraml[0].idata = eftEFTindex[fp_eftEFT.lv];
	if(EFT_BAY_pgm(paraml))
	{
		KEY_beep();
 		return;
	}
	/* Check for 4.4 kV / 8.0 kV MAX voltage conflict */
	fp_eftVLT.maxv = EFT_MAXVLT4400;
	if(eftdata.eftaddr != GHOST_BOX)
		if(moddata[(char)(eftdata.eftaddr)].modinfo.options & OPT_EFT8KV)
			fp_eftVLT.maxv = EFT_MAXVLT8000;
	if(abs(fp_eftVLT.f) > fp_eftVLT.maxv)
		fp_eftVLT.f = fp_eftVLT.maxv;

	/* Modification by James Murphy (JMM) on 07/29/94 */
	/* Check for 1 MHz / 2 MHz MAX Frequency conflict */
	fp_eftFRQ.maxv = 1000;
	if(moddata[(char)(eftdata.eftaddr)].modinfo.options & OPT_EFT2MHZ)
		fp_eftFRQ.maxv = 2000;
	if(fp_eftUNT.bv)								/* kHz mode */
	{
		if(fp_eftFRQ.f > fp_eftFRQ.maxv)
			fp_eftFRQ.f = fp_eftFRQ.maxv;
	}

	/* If NO-CHIRP option is set then CHIRP waves are NOT allowed */
	if(eftdata.eftaddr != GHOST_BOX)
		if(moddata[(char)(eftdata.eftaddr)].modinfo.options & OPT_EFTCHIRP)
			field_validation(&field_list[fp_eftWAV.field],1);
		else
			field_validation(&field_list[fp_eftWAV.field],0);

	/* Reset the OUTPUT field */
	fp_eftOUT.lv = 0;

	EFT_VLTchanged();
	EFT_FRQchanged();
	EFT_OUTchanged();

	ox = LCD_getx();
	oy = LCD_gety();
	LCD_gotoxy(FLD_EFTVLTx,FLD_EFTVLTy);
	fprintf(DISPLAY,"%+-d",fp_eftVLT.f);
	LCD_gotoxy(FLD_EFTOUTx,FLD_EFTOUTy);
	LCD_puts(eftOUTnames[fp_eftOUT.lv]);
	LCD_gotoxy(FLD_EFTFRQx,FLD_EFTFRQy);
	fprintf(DISPLAY,"%-d",fp_eftFRQ.f);
	LCD_gotoxy(ox,oy);
}

void	EFT_OUTchanged()
{
	if((moddata[(char)(eftdata.eftaddr)].modinfo.typePrimary & TYPE_CPLEFT)
		&& (fp_eftOUT.lv == 1))
	{
		if(fp_eftOUT.dir)
			fp_eftOUT.lv++;
		else
			fp_eftOUT.lv--;
		LCD_puts(eftOUTnames[fp_eftOUT.lv]);
	}
	if((moddata[(char)(eftdata.eftaddr)].modinfo.typePrimary != TYPE_CPLEFT)
		&& (moddata[(char)(eftOUTindex[fp_eftOUT.lv])].modinfo.typePrimary == TYPE_EFT))
	{
		if(fp_eftOUT.dir)
		{
			fp_eftOUT.lv++;
			if(eftOUTindex[fp_eftOUT.lv] == GHOST_BOX)
				fp_eftOUT.lv = 0;
		}
		else
			fp_eftOUT.lv--;
		LCD_puts(eftOUTnames[fp_eftOUT.lv]);
	}
	if(eftOUTindex[fp_eftOUT.lv] == GHOST_BOX)		/* Basic Output	*/
	{
		paraml[0].idata = 0;
		paraml[1].idata = fp_eftOUT.lv;
	}
	else															/* Coupler Output	*/
	{
		paraml[0].idata = 1;
		paraml[1].idata = eftOUTindex[fp_eftOUT.lv];
	}
	if(EFT_OUT_pgm(paraml))
	{
		KEY_beep();
	}

	/* PHASE field ONLY valid when output routed to COUPLER	*/
	/* ANGLE field ONLY valid when output routed to COUPLER
			AND PHASE NOT RANDOM (PHASE is RANDOM if (!fp_eftLSM.lv) */
	/* EFT LSM must be RESET to RANDOM when switching to FRONT PANEL	*/
	if(cpldata.outaddr != GHOST_BOX)
	{
		field_validation(&field_list[fp_eftLSM.field],1);

		if(!fp_eftLSM.lv)
			field_validation(&field_list[fp_eftANG.field],0);
		else
			field_validation(&field_list[fp_eftANG.field],1);
	}
	else
	{
		field_validation(&field_list[fp_eftLSM.field],0);
		field_validation(&field_list[fp_eftANG.field],0);
		fp_eftLSM.lv = 0;
		EFT_LSMchanged();
	}

	/* RESET - CPL data */
	fp_eftCL1.bv = 0;
	fp_eftCL2.bv = 0;
	fp_eftCL3.bv = 0;
	fp_eftCNU.bv = 0;
	fp_eftCPE.bv = 0;
	EFT_CPLvalidate();
	EFT_CPLchanged();
}

void	EFT_MDEchanged()
{
	paraml[0].idata = fp_eftMDE.bv;
	if(EFT_MDE_pgm(paraml))
		KEY_beep();
}

void	EFT_WAVchanged()
{
	paraml[0].idata = fp_eftWAV.bv;
	if(EFT_WAV_pgm(paraml))
		KEY_beep();
	if(eftdata.waveform)
	{
		field_validation(&field_list[fp_eftDUR.field],0);
		field_validation(&field_list[fp_eftFRQ.field],0);
		field_list[fp_eftUNT.field].valid = 0;
	}
	else
	{
		field_validation(&field_list[fp_eftDUR.field],1);
		field_validation(&field_list[fp_eftFRQ.field],1);
		field_list[fp_eftUNT.field].valid = 1;
	}
}

void	EFT_VLTchanged()
{
	int	ox,oy;

	paraml[0].idata = fp_eftVLT.f;
	if(EFT_VLT_pgm(paraml))
	{
		if(display.current == DISPLAY_EFT)
			display.request = DISPLAY_BAD_FRQ;
		return;
	}
	EFT_DURcheck();
}

void	EFT_FRQchanged()
{
	paraml[0].idata = fp_eftFRQ.f * ((eftdata.unit)?(1000):(1));
	if(EFT_FRQ_pgm(paraml))
	{
		if(display.current == DISPLAY_EFT)
			display.request = DISPLAY_BAD_FRQ;
		return;
	}
	EFT_DURcheck();
}

void	EFT_UNTchanged()
{
	int	ox,oy;

	ox = LCD_getx();
	oy = LCD_gety();
	LCD_gotoxy(FLD_EFTFRQx,FLD_EFTFRQy);

	eftdata.unit = fp_eftUNT.bv;
	if(eftdata.unit)						/* kHz*/
	{
		LCD_puts("1   ");
		fp_eftFRQ.f		= 1;
		fp_eftFRQ.minv = 1;
		fp_eftFRQ.maxv = 1000;
		if(eftdata.eftaddr != GHOST_BOX)
			if(moddata[(char)(eftdata.eftaddr)].modinfo.options & OPT_EFT2MHZ)
				fp_eftFRQ.maxv = 2000;
	}
	else										/* Hz	*/
	{
		LCD_puts("1000");
		fp_eftFRQ.f		= 1000;
		fp_eftFRQ.minv = 1000;
		fp_eftFRQ.maxv = 9999;
	}
	LCD_gotoxy(ox,oy);
	EFT_FRQchanged();
}

void	EFT_DURcheck(void)
{
	uint	freq;
	int	ox,oy;

	if(moddata[eftdata.eftaddr].modinfo.id == E421P_BOX)
	{
		freq = fp_eftFRQ.f;
		if(fp_eftUNT.bv)
			freq *= 1000;
		fp_eftDUR.maxv	= E421P_MaxDuration(abs(fp_eftVLT.f),freq,fp_eftPER.f);
		if(fp_eftDUR.f > fp_eftDUR.maxv)
		{
			fp_eftDUR.f = fp_eftDUR.maxv;
			ox = LCD_getx();
			oy = LCD_gety();
			LCD_gotoxy(FLD_EFTDURx,FLD_EFTDURy);
			LCD_puts("     ");
			LCD_gotoxy(FLD_EFTDURx,FLD_EFTDURy);
			fprintf(DISPLAY,"%-d",fp_eftDUR.f);
			LCD_gotoxy(ox,oy);
			EFT_DURchanged();
		}
	}
}

void	EFT_DURchanged()
{
	paraml[0].idata = fp_eftDUR.f;
	if(EFT_DUR_pgm(paraml))
		KEY_beep();
}

void	EFT_PERchanged()
{
	paraml[0].idata = fp_eftPER.f;
	if(EFT_PER_pgm(paraml))
		KEY_beep();
	EFT_DURcheck();
}

void	EFT_LSMchanged()
{
	paraml[0].idata = fp_eftLSM.lv;
	if(LI_MODE_pgm(paraml))
		KEY_beep();

	if(!fp_eftLSM.lv)
		field_validation(&field_list[fp_eftANG.field],0);
	else
		field_validation(&field_list[fp_eftANG.field],1);
}

void	EFT_ANGchanged()
{
	paraml[0].idata = fp_eftANG.f;
	if(LI_ANG_pgm(paraml))
		KEY_beep();
}

void	EFT_SRCchanged()
{
	int	ox,oy;

	paraml[0].idata = cplSRCindex[fp_eftSRC.lv];
	if(root_CPL_pgm(paraml))
		KEY_beep();

	if(cpldata.eutrequest == 0)
		fp_eftEUT.bv = 0;

	if(acdata.flag)
	{
		EFT_SetPeakNames();
		LCD_gotoxy(FLD_EFTPKSx,FLD_EFTPKSy);
		LCD_puts(fp_eftPKS.labels[fp_eftPKS.lv]);
		EFT_PKSchanged();
	}
	if(cpldata.srcaddr != GHOST_BOX)
	{
		ox = LCD_getx();
		oy = LCD_gety();
		field_validation(&field_list[fp_eftEUT.field],1);
		LCD_gotoxy(FLD_EFTEUTx,FLD_EFTEUTy);
		LCD_puts(((fp_eftEUT.bv)?(fp_eftEUT.true_label):(fp_eftEUT.false_label)));
		LCD_gotoxy(ox,oy);
	}
	else
		field_validation(&field_list[fp_eftEUT.field],0);

	EFT_EUTchanged();
}

void	EFT_EUTchanged()
{
	int	ox,oy;

	paraml[0].idata = fp_eftEUT.bv;
 	if(root_EUT_pgm(paraml))
	{
		KEY_beep();
		return;
	}
	ox = LCD_getx();
	oy = LCD_gety();
	LCD_gotoxy(FLD_EFTEUTx,FLD_EFTEUTy);
	if(cpldata.srcaddr != GHOST_BOX)
		field_validation(&field_list[fp_eftEUT.field],1);
	else
		field_validation(&field_list[fp_eftEUT.field],0);
	LCD_gotoxy(ox,oy);
}

void	EFT_CPLchanged()
{
	uchar	cplbits;

	if(eftdata.active)
	{
		KEY_beep();
		return;
	}
	cplbits = 0x00;
	if(fp_eftCL1.bv)
		cplbits |= CPL_L1BIT;
	if(fp_eftCL2.bv)
		cplbits |= CPL_L2BIT;
	if(fp_eftCL3.bv)
		cplbits |= CPL_L3BIT;
	if(fp_eftCNU.bv)
		cplbits |= CPL_NUBIT;
	if(fp_eftCPE.bv)
		cplbits |= CPL_PEBIT;
	paraml[0].idata = cplbits;
	if(EFT_CPL_pgm(paraml))
		KEY_beep();
}

void	EFT_CPLvalidate(void)
{
	int	ox,oy;
	uchar	tflg;
	uchar	tvar;

	if(fp_eftOUT.lv == 1 && fp_eftOUT.dir)
		return;

	ox = LCD_getx();
	oy = LCD_gety();
	LCD_gotoxy(0,FLD_EFTCL1y);

	tflg = 0;
	tvar = (eftOUTindex[fp_eftOUT.lv] != GHOST_BOX)?(1):(0);

	field_list[fp_eftCL1.field].valid = tvar ^ 1;
	field_list[fp_eftCL2.field].valid = tvar ^ 1;
	field_list[fp_eftCL3.field].valid = tvar ^ 1;
	field_list[fp_eftCNU.field].valid = tvar ^ 1;
	field_list[fp_eftCPE.field].valid = tvar ^ 1;

	if(tvar)
		if(moddata[eftOUTindex[fp_eftOUT.lv]].modinfo.typePrimary & TYPE_3PHASE)
			tflg = 1;
	if(tflg)
	{
		field_list[fp_eftCPE.field].y_pos = field_list[fp_eftCPE.field].y_end = FLD_EFTCPEy0;
		field_vertical(fp_eftCL1.field,fp_eftCPE.field,fp_eftCL2.field);
		field_vertical(fp_eftCL2.field,fp_eftCL1.field,fp_eftCL3.field);
		field_vertical(fp_eftCL3.field,fp_eftCL2.field,fp_eftCNU.field);
		field_vertical(fp_eftCNU.field,fp_eftCL3.field,fp_eftCPE.field);
		field_vertical(fp_eftCPE.field,fp_eftCNU.field,fp_eftCL1.field);
		field_horizontal(fp_eftDUR.field,fp_eftCL3.field,fp_eftPER.field);
		field_horizontal(fp_eftPER.field,fp_eftDUR.field,fp_eftCL3.field);
		field_horizontal(fp_eftCL3.field,fp_eftPER.field,fp_eftDUR.field);
		field_horizontal(fp_eftLSM.field,fp_eftCNU.field,fp_eftANG.field);
		field_horizontal(fp_eftANG.field,fp_eftLSM.field,fp_eftCNU.field);
		field_horizontal(fp_eftCNU.field,fp_eftANG.field,fp_eftLSM.field);
		field_horizontal(fp_eftSRC.field,fp_eftCPE.field,fp_eftEUT.field);
		field_horizontal(fp_eftEUT.field,fp_eftSRC.field,fp_eftCPE.field);
		field_horizontal(fp_eftCPE.field,fp_eftEUT.field,fp_eftSRC.field);
		LCD_gotoxy(35,3);
		LCD_puts("L3:");
		LCD_gotoxy(35,4);
		LCD_puts("N :");
		LCD_gotoxy(35,5);
		LCD_puts("PE:");

		listLSMmodes[0] = "RNDM";
		listLSMmodes[1] = "L1  ";
		listLSMmodes[2] = "L2  ";
		listLSMmodes[3] = "L3  ";
		listLSMmodes[4] = 0x00;
	}
	else
	{
		field_list[fp_eftCPE.field].y_pos = field_list[fp_eftCPE.field].y_end = FLD_EFTCPEy1;
		field_vertical(fp_eftCL1.field,fp_eftCPE.field,fp_eftCL2.field);
		field_vertical(fp_eftCL2.field,fp_eftCL1.field,fp_eftCPE.field);
		field_vertical(fp_eftCPE.field,fp_eftCL2.field,fp_eftCL1.field);
		field_horizontal(fp_eftDUR.field,fp_eftCPE.field,fp_eftPER.field);
		field_horizontal(fp_eftPER.field,fp_eftDUR.field,fp_eftCPE.field);
		field_horizontal(fp_eftCPE.field,fp_eftPER.field,fp_eftDUR.field);
		field_horizontal(fp_eftLSM.field,fp_eftANG.field,fp_eftANG.field);
		field_horizontal(fp_eftANG.field,fp_eftLSM.field,fp_eftLSM.field);
		field_horizontal(fp_eftSRC.field,fp_eftEUT.field,fp_eftEUT.field);
		field_horizontal(fp_eftEUT.field,fp_eftSRC.field,fp_eftSRC.field);
		field_validation(&field_list[fp_eftCL3.field],0);
		field_validation(&field_list[fp_eftCNU.field],0);
		LCD_gotoxy(35,3);
		LCD_puts("PE:");
		LCD_gotoxy(35,4);
		LCD_puts("     ");
		LCD_gotoxy(35,5);
		LCD_puts("     ");

		listLSMmodes[0] = "RNDM";
		listLSMmodes[1] = "L1  ";
		listLSMmodes[2] = 0x00;
	}
	field_validation(&field_list[fp_eftCL1.field],tvar);
	field_validation(&field_list[fp_eftCL2.field],tvar);
	field_validation(&field_list[fp_eftCPE.field],tvar);
	if(tvar)
		if(moddata[eftOUTindex[fp_eftOUT.lv]].modinfo.typePrimary & TYPE_3PHASE)
		{
			field_validation(&field_list[fp_eftCL3.field],tvar);
			field_validation(&field_list[fp_eftCNU.field],tvar);
		}
	LCD_gotoxy(ox,oy);
}

char	eftcheck(
uchar	field)
{
	if(eftdata.active)
		KEY_beep();
}

void	EFT_UpdateAcPeaks(void)
{
	int	ox,oy,l;
	float	rms;

	ox = LCD_getx();
	oy = LCD_gety();

	LCD_gotoxy(11,6);
	if(acdata.acbox == GHOST_BOX)
		LCD_puts("                             ");
	else
		if(acdata.measure_on)
		{
			l = fp_eftPKS.lv;
			rms = sqrt(acdata.ldata[l].rms);
			fprintf(DISPLAY,"%+6.1fA %+6.1fA  RMS: %+6.1fA",AmpsPeak(acdata.ldata[l].peakp),
 						AmpsPeak(acdata.ldata[l].peakm),AmpsPeak(rms));
		}
		else
			LCD_puts("  +0.0A   +0.0A  RMS:   +0.0A");
		LCD_gotoxy(ox,oy);
}

void	EFT_PKSchanged()
{
	if(acdata.acbox != GHOST_BOX)
		acdata.line = fp_eftPKS.lv + 1;
	EFT_UpdateAcPeaks();
}

void	EFT_SetPeakNames(void)
{ 
	if(acdata.acbox == GHOST_BOX)
	{
		listPKSnames[0] = "N/A   ";
		listPKSnames[1] = 0x00;
		fp_eftPKS.lv = 0;
	}
	else
	{
		listPKSnames[0] = "L1 AC Pks";
		listPKSnames[1] = "L2 AC Pks";
		listPKSnames[2] = "L3 AC Pks";
		listPKSnames[ac_linecnt] = 0x00;
		fp_eftPKS.lv = (acdata.line)?(acdata.line - 1):(0);
	}
}

void	EFT_FPDataSet(
EFTDATA	*eftptr,
CPLDATA	*cplptr)
{
	uint	tvar1,tvar2;

/*
 *	Set up array holdings for EFT modules.  The index into the
 *	array is same as the network id (no translation is needed).
 */
	fp_eftEFT.lv = 0;
	for(tvar1 = 0, tvar2 = 0; tvar1 < MAX_MODULES; tvar1++)
		if(moddata[tvar1].modinfo.typePrimary & TYPE_EFT)
		{
		 	eftEFTnames[tvar2] = moddata[tvar1].modinfo.name;
			eftEFTindex[tvar2] = tvar1;
			if(eftptr->eftaddr == tvar1)
				fp_eftEFT.lv = tvar2;
			tvar2++;
		}
	eftEFTnames[tvar2] = 0x00;
	eftEFTindex[tvar2] = GHOST_BOX;

/*
 *	Set up array holdings for EFT outputs.  The first TWO outputs
 *	will be OUT1 and OUT2.  The rest will be ALL COUPLERS in system
 */
	fp_eftOUT.lv	= 0;
	eftOUTnames[0] = "OUT1 ";
	eftOUTindex[0] = GHOST_BOX;
	eftOUTnames[1] = "OUT2 ";
	eftOUTindex[1] = GHOST_BOX;
	for(tvar1 = 0, tvar2 = 2; tvar1 < MAX_MODULES; tvar1++)
		if(moddata[tvar1].modinfo.typePrimary & TYPE_CPLEFT)
		{
			eftOUTnames[tvar2] = moddata[tvar1].modinfo.name;
			eftOUTindex[tvar2] = tvar1;
			if((eftptr->output == 1) && (cplptr->outaddr == tvar1))
				fp_eftOUT.lv = tvar2;
			tvar2++;
		}
	eftOUTnames[tvar2] = 0x00;
	eftOUTindex[tvar2] = GHOST_BOX;

/*
 *	Set up array holdings for EUT power sources
 */
	cplSRCnames[0] = "N/A  ";
	cplSRCindex[0] = GHOST_BOX;
	for(tvar1 = 0, tvar2 = 0; tvar1 < MAX_MODULES; tvar1++)
		if(moddata[tvar1].modinfo.typePrimary & TYPE_CPLLINE)
		{
		 	cplSRCnames[tvar2]= moddata[tvar1].modinfo.name;
			cplSRCindex[tvar2]= tvar1;
			if(cplptr->srcaddr == tvar1)
				fp_eftSRC.lv = tvar2;
			tvar2++;
		}
	if(tvar2 == 0)
		tvar2 = 1;
	cplSRCnames[tvar2] = 0x00;
	cplSRCindex[tvar2] = GHOST_BOX;

	fp_eftVLT.f		 = eftptr->voltage;
	fp_eftVLT.minv	 = EFT_MINVLT;
	fp_eftVLT.maxv	 = EFT_MAXVLT4400;
	fp_eftVLT.minus = 1;

	if(eftptr->eftaddr != GHOST_BOX)
		if(moddata[(char)(eftptr->eftaddr)].modinfo.options & OPT_EFT8KV)
			fp_eftVLT.maxv = EFT_MAXVLT8000;

	if(eftptr->unit)						/* kHz*/
	{
		fp_eftFRQ.f		= eftptr->frequency / 1000;
		fp_eftFRQ.minv = 1;
		fp_eftFRQ.maxv = 1000;
		if(eftptr->eftaddr != GHOST_BOX)
			if((moddata[(char)(eftptr->eftaddr)].modinfo.options & OPT_EFT2MHZ))
				fp_eftFRQ.maxv = 2000;
	}
	else										/* Hz	*/
	{
		fp_eftFRQ.f		= eftptr->frequency;
		fp_eftFRQ.minv = 1000;
		fp_eftFRQ.maxv = 9999;
	}
	fp_eftFRQ.minus = 0;

	fp_eftPER.f		 = eftptr->period;
	fp_eftPER.minv	 = EFT_MINPER;
	fp_eftPER.maxv	 = EFT_MAXPER;
	fp_eftPER.minus = 0;

	fp_eftDUR.minv	 = EFT_MINDUR;
 	fp_eftDUR.minus = 0;
	fp_eftDUR.f	    = eftptr->duration;
	if (moddata[eftdata.eftaddr].modinfo.id == E421P_BOX)
	{
		fp_eftDUR.maxv = E421P_MaxDuration(abs(eftptr->voltage),
								(uint)eftptr->frequency,(int)eftptr->period);
		if(fp_eftDUR.f > fp_eftDUR.maxv)
			fp_eftDUR.f = fp_eftDUR.maxv;
	}
	else
		fp_eftDUR.maxv = EFT_MAXDUR;

	fp_eftANG.f		 = cplptr->angle;
	fp_eftANG.minv	 = 0;
	fp_eftANG.maxv	 = 360;
	fp_eftANG.minus = 0;
	
	fp_eftUNT.bv = eftptr->unit;
	fp_eftMDE.bv = eftptr->mode;
	fp_eftWAV.bv = eftptr->waveform;

	fp_eftLSM.lv = cplptr->phasemode;

	fp_eftEUT.bv = cplptr->eutrequest;
	fp_eftCL1.bv = (cplptr->cplhigh & CPL_L1BIT)?(1):(0);
	fp_eftCL2.bv = (cplptr->cplhigh & CPL_L2BIT)?(1):(0);
	fp_eftCL3.bv = (cplptr->cplhigh & CPL_L3BIT)?(1):(0);
	fp_eftCNU.bv = (cplptr->cplhigh & CPL_NUBIT)?(1):(0);
	fp_eftCPE.bv = (cplptr->cplhigh & CPL_PEBIT)?(1):(0);

	if(acdata.flag)
		EFT_SetPeakNames();
}

void	show_state_eft(void)
{
	uint	tvar1,tvar2;

	listLSMmodes[0] = "RNDM";
	listLSMmodes[1] = "L1  ";
	listLSMmodes[2] = "L2  ";
	listLSMmodes[3] = "L3  ";
	listLSMmodes[4] = 0x00;

	EFT_FPDataSet(&eftdata,&cpldata);
	if(saveft)
	{
		savEFTdata.eftaddr	= eftdata.eftaddr;
		savEFTdata.duration	= eftdata.duration;
		savEFTdata.period		= eftdata.period;
		savEFTdata.voltage	= eftdata.voltage;
		savEFTdata.frequency	= eftdata.frequency;
		savEFTdata.unit		= eftdata.unit;
		savEFTdata.waveform	= eftdata.waveform;
		savEFTdata.mode		= eftdata.mode;
		savEFTdata.output		= eftdata.output;
		savLINEcpl.srcaddr	= cpldata.srcaddr;
		savLINEcpl.outaddr	= cpldata.outaddr;
		savLINEcpl.cplhigh	= cpldata.cplhigh;
		savLINEcpl.cpllow		= cpldata.cpllow;
		savLINEcpl.phasemode	= cpldata.phasemode;
		savLINEcpl.angle		= cpldata.angle;
		savLINEcpl.eutrequest= cpldata.eutrequest;
	}

	LCD_gotoxy(0,0);
	LCD_puts("EFT Gen  :        Mode  :      IDLE\n\r");
	LCD_puts("Output at:        Chirp :          L1:\n\r");
	LCD_puts("Voltage  :      V Freq  :          L2:\n\r");
	LCD_puts("Duration :      p Period:      mS  L3:\n\r");
	LCD_puts("Line Sync:        Angle :      deg N :\n\r");
	LCD_puts("EUT Mains:       EUT Pwr:          PE:\n\r\n\r");

	if(calibration)
		LCD_puts("_____ _____ _____ ___CALIBRATION__ MEAS");
	else
		LCD_puts("_____ _____ _____ _____ ____ _____ MEAS");

	init_list_field(FLD_EFTBOXx,FLD_EFTBOXy,0,&fp_eftEFT,EFT_EFTchanged,eftcheck,eftEFTnames,1);
	init_list_field(FLD_EFTOUTx,FLD_EFTOUTy,0,&fp_eftOUT,EFT_OUTchanged,eftcheck,eftOUTnames,1);
	init_bool_field(FLD_EFTMDEx,FLD_EFTMDEy,&fp_eftMDE,EFT_MDEchanged,eftcheck,"MOM","CON",1);
	init_bool_field(FLD_EFTWAVx,FLD_EFTWAVy,&fp_eftWAV,EFT_WAVchanged,eftcheck,"OFF","ON ",1);
	init_numeric_field(FLD_EFTVLTx,FLD_EFTVLTy,6,&fp_eftVLT,EFT_VLTchanged,eftcheck,1);
	init_numeric_field(FLD_EFTFRQx,FLD_EFTFRQy,4,&fp_eftFRQ,EFT_FRQchanged,eftcheck,1);
	init_bool_field(FLD_EFTUNTx,FLD_EFTUNTy,&fp_eftUNT,EFT_UNTchanged,eftcheck,"Hz ","kHz",1);
	init_numeric_field(FLD_EFTDURx,FLD_EFTDURy,5,&fp_eftDUR,EFT_DURchanged,eftcheck,1);
	init_numeric_field(FLD_EFTPERx,FLD_EFTPERy,4,&fp_eftPER,EFT_PERchanged,eftcheck,1);
	init_numeric_field(FLD_EFTANGx,FLD_EFTANGy,3,&fp_eftANG,EFT_ANGchanged,0x00,1);
	init_list_field(FLD_EFTLSMx,FLD_EFTLSMy,0,&fp_eftLSM,EFT_LSMchanged,eftcheck,listLSMmodes,1);
	init_list_field(FLD_EFTSRCx,FLD_EFTSRCy,0,&fp_eftSRC,EFT_SRCchanged,eftcheck,cplSRCnames,1);
	init_bool_field(FLD_EFTEUTx,FLD_EFTEUTy,&fp_eftEUT,EFT_EUTchanged,eftcheck,"OFF  ","READY",1);
	init_bool_field(FLD_EFTCL1x,FLD_EFTCL1y,&fp_eftCL1,EFT_CPLchanged,eftcheck,"--","ON",1);
	init_bool_field(FLD_EFTCL2x,FLD_EFTCL2y,&fp_eftCL2,EFT_CPLchanged,eftcheck,"--","ON",1);
	init_bool_field(FLD_EFTCL3x,FLD_EFTCL3y,&fp_eftCL3,EFT_CPLchanged,eftcheck,"--","ON",1);
	init_bool_field(FLD_EFTCNUx,FLD_EFTCNUy,&fp_eftCNU,EFT_CPLchanged,eftcheck,"--","ON",1);
	init_bool_field(FLD_EFTCPEx0,FLD_EFTCPEy0,&fp_eftCPE,EFT_CPLchanged,eftcheck,"--","ON",1);

	if(acdata.flag)
	{
		init_list_field(FLD_EFTPKSx,FLD_EFTPKSy,0,&fp_eftPKS,EFT_PKSchanged,0x00,listPKSnames,1);
		field_vertical(fp_eftEFT.field,fp_eftPKS.field,fp_eftOUT.field);
		field_vertical(fp_eftSRC.field,fp_eftLSM.field,fp_eftPKS.field);
		field_vertical(fp_eftPKS.field,fp_eftSRC.field,fp_eftEFT.field);
	}
	else
	{
		field_vertical(fp_eftEFT.field,fp_eftSRC.field,fp_eftOUT.field);
		field_vertical(fp_eftSRC.field,fp_eftLSM.field,fp_eftEFT.field);
	}
	/* First Column */
	field_vertical(fp_eftOUT.field,fp_eftEFT.field,fp_eftVLT.field);
	field_vertical(fp_eftVLT.field,fp_eftOUT.field,fp_eftDUR.field);
	field_vertical(fp_eftDUR.field,fp_eftVLT.field,fp_eftLSM.field);
	field_vertical(fp_eftLSM.field,fp_eftDUR.field,fp_eftSRC.field);

	/* Second Column */
	field_vertical(fp_eftMDE.field,fp_eftEUT.field,fp_eftWAV.field);
	field_vertical(fp_eftWAV.field,fp_eftMDE.field,fp_eftFRQ.field);
	field_vertical(fp_eftFRQ.field,fp_eftWAV.field,fp_eftPER.field);
	field_vertical(fp_eftPER.field,fp_eftFRQ.field,fp_eftANG.field);
	field_vertical(fp_eftANG.field,fp_eftPER.field,fp_eftEUT.field);
	field_vertical(fp_eftEUT.field,fp_eftANG.field,fp_eftMDE.field);

	/* Third Column */
	field_vertical(fp_eftCL1.field,fp_eftCPE.field,fp_eftCL2.field);
	field_vertical(fp_eftCL2.field,fp_eftCL1.field,fp_eftCL3.field);
	field_vertical(fp_eftCL3.field,fp_eftCL2.field,fp_eftCNU.field);
	field_vertical(fp_eftCNU.field,fp_eftCL3.field,fp_eftCPE.field);
	field_vertical(fp_eftCPE.field,fp_eftCNU.field,fp_eftCL1.field);

	field_horizontal(fp_eftEFT.field,fp_eftMDE.field,fp_eftMDE.field);
	field_horizontal(fp_eftMDE.field,fp_eftEFT.field,fp_eftEFT.field);

	field_horizontal(fp_eftOUT.field,fp_eftCL1.field,fp_eftWAV.field);
	field_horizontal(fp_eftWAV.field,fp_eftOUT.field,fp_eftCL1.field);
	field_horizontal(fp_eftCL1.field,fp_eftWAV.field,fp_eftOUT.field);

	field_horizontal(fp_eftVLT.field,fp_eftCL2.field,fp_eftFRQ.field);
	field_horizontal(fp_eftFRQ.field,fp_eftVLT.field,fp_eftUNT.field);
	field_horizontal(fp_eftUNT.field,fp_eftFRQ.field,fp_eftCL2.field);
	field_horizontal(fp_eftCL2.field,fp_eftUNT.field,fp_eftVLT.field);

	field_horizontal(fp_eftDUR.field,fp_eftCL3.field,fp_eftPER.field);
	field_horizontal(fp_eftPER.field,fp_eftDUR.field,fp_eftCL3.field);
	field_horizontal(fp_eftCL3.field,fp_eftPER.field,fp_eftDUR.field);

	field_horizontal(fp_eftLSM.field,fp_eftCNU.field,fp_eftANG.field);
	field_horizontal(fp_eftANG.field,fp_eftLSM.field,fp_eftCNU.field);
	field_horizontal(fp_eftCNU.field,fp_eftANG.field,fp_eftLSM.field);

	field_horizontal(fp_eftSRC.field,fp_eftCPE.field,fp_eftEUT.field);
	field_horizontal(fp_eftEUT.field,fp_eftSRC.field,fp_eftCPE.field);
	field_horizontal(fp_eftCPE.field,fp_eftEUT.field,fp_eftSRC.field);

	tvar1 = 0;
	for(tvar2 = 0; tvar2 < MAX_MODULES; tvar2++)
		if(moddata[tvar2].modinfo.typePrimary & TYPE_EFT)
			tvar1++;
	if(tvar1 <= 1)
		field_list[fp_eftEFT.field].valid = 0;

	tvar1 = 0;
	for(tvar2 = 0; tvar2 < MAX_CPLBOXES; tvar2++)
		if(moddata[tvar2].modinfo.typePrimary & TYPE_CPLLINE)
			tvar1++;
	if(tvar1 <= 1)
		field_list[fp_eftSRC.field].valid = 0;

/*
 *	If operating conditions changed and then an external event
 *	(i.e. interlock opened) happened, then restore correct values
 */
	if(saveft)
	{
		EFT_EFTchanged();
		EFT_FPDataSet(&savEFTdata,&savLINEcpl);
		EFT_OUTchanged();
		EFT_FPDataSet(&savEFTdata,&savLINEcpl);
		EFT_SRCchanged();
		EFT_CPLvalidate();
		EFT_CPLchanged();
		EFT_FPDataSet(&savEFTdata,&savLINEcpl);
		EFT_VLTchanged();
		EFT_UNTchanged();
		EFT_FPDataSet(&savEFTdata,&savLINEcpl);
		EFT_FRQchanged();
	}
	else
	{
		saveft = 1;
		EFT_EFTchanged();
		EFT_OUTchanged();
		EFT_SRCchanged();
		EFT_VLTchanged();
		EFT_UNTchanged();
		EFT_FRQchanged();
	}
	EFT_EUTchanged();
	EFT_MDEchanged();
	EFT_WAVchanged();
	EFT_PERchanged();
	EFT_DURchanged();

/* 
 *	Write in correct field labels instead of dummy labels used
 *	during field setup.
 */
	(*field_list[fp_eftSRC.field].left)(&field_list[fp_eftSRC.field]);
	(*field_list[fp_eftEUT.field].left)(&field_list[fp_eftEUT.field]);
	(*field_list[fp_eftEFT.field].left)(&field_list[fp_eftEFT.field]);
	(*field_list[fp_eftOUT.field].left)(&field_list[fp_eftOUT.field]);
	(*field_list[fp_eftMDE.field].left)(&field_list[fp_eftMDE.field]);
	(*field_list[fp_eftWAV.field].left)(&field_list[fp_eftWAV.field]);
	(*field_list[fp_eftEFT.field].left)(&field_list[fp_eftEFT.field]);
	(*field_list[fp_eftUNT.field].left)(&field_list[fp_eftUNT.field]);
	(*field_list[fp_eftFRQ.field].left)(&field_list[fp_eftFRQ.field]);
	(*field_list[fp_eftPER.field].left)(&field_list[fp_eftPER.field]);
	(*field_list[fp_eftDUR.field].left)(&field_list[fp_eftDUR.field]);
	(*field_list[fp_eftVLT.field].left)(&field_list[fp_eftVLT.field]);
	(*field_list[fp_eftLSM.field].left)(&field_list[fp_eftLSM.field]);
	(*field_list[fp_eftANG.field].left)(&field_list[fp_eftANG.field]);
	(*field_list[fp_eftCL1.field].left)(&field_list[fp_eftCL1.field]);
	(*field_list[fp_eftCL2.field].left)(&field_list[fp_eftCL2.field]);
	(*field_list[fp_eftCL3.field].left)(&field_list[fp_eftCL3.field]);
	(*field_list[fp_eftCNU.field].left)(&field_list[fp_eftCNU.field]);
	(*field_list[fp_eftCPE.field].left)(&field_list[fp_eftCPE.field]);

	if(acdata.flag)
	{
		EFT_PKSchanged();
		(*field_list[fp_eftPKS.field].left)(&field_list[fp_eftPKS.field]);
	}

	func_manager = eft_funcs;
}

uchar	eft_funcs(
char	c)
{
	if(display.current != DISPLAY_EFT)
		return(0);
	switch(c)
	{
		case 'G':	/* STRT	- Start EFT sequence	*/
			if(!eftdata.active)
			{
				EFT_StartSystem(0);
				EFT_StartSystem(1);
			}
			break;
		case 'X':	/* STOP	- Stop EFT sequence	*/
			EFT_StopSystem();
			break;
		case 'M':	/* MENU	- Main Menu	*/
			EFT_ResetModules(1);
			display.request = DISPLAY_LOCAL;
			break;
		case 'g':	/* F7	- Measurement	*/
			if(acdata.acbox != GHOST_BOX)
			{
				EFT_StopSystem();
				display.request = DISPLAY_MEASURE;
			}
			break;
		default:
			break;
	}
	return(0);
}

void	CAL_EFTchanged()
{
	fp_cal_CLAMP_0250.f = moddata[eftEFTindex[fp_calEFT.lv]].modinfo.calinfo.data[CAL_LLOOFF_CLAMP];
	fp_cal_CLAMP_4400.f = moddata[eftEFTindex[fp_calEFT.lv]].modinfo.calinfo.data[CAL_LHIOFF_CLAMP];
	fp_cal_CLAMP_4401.f = moddata[eftEFTindex[fp_calEFT.lv]].modinfo.calinfo.data[CAL_HLOOFF_CLAMP];
	fp_cal_CLAMP_8000.f = moddata[eftEFTindex[fp_calEFT.lv]].modinfo.calinfo.data[CAL_HHIOFF_CLAMP];

	fp_cal_CPL_0250.f = moddata[eftEFTindex[fp_calEFT.lv]].modinfo.calinfo.data[CAL_LLOOFF_CPL];
	fp_cal_CPL_4400.f = moddata[eftEFTindex[fp_calEFT.lv]].modinfo.calinfo.data[CAL_LHIOFF_CPL];
	fp_cal_CPL_4401.f = moddata[eftEFTindex[fp_calEFT.lv]].modinfo.calinfo.data[CAL_HLOOFF_CPL];
	fp_cal_CPL_8000.f = moddata[eftEFTindex[fp_calEFT.lv]].modinfo.calinfo.data[CAL_HHIOFF_CPL];

	(*field_list[fp_cal_CLAMP_0250.field].left)(&field_list[fp_cal_CLAMP_0250.field]);
	(*field_list[fp_cal_CLAMP_4400.field].left)(&field_list[fp_cal_CLAMP_4400.field]);
	(*field_list[fp_cal_CLAMP_4401.field].left)(&field_list[fp_cal_CLAMP_4401.field]);
	(*field_list[fp_cal_CLAMP_8000.field].left)(&field_list[fp_cal_CLAMP_8000.field]);

	(*field_list[fp_cal_CPL_0250.field].left)(&field_list[fp_cal_CPL_0250.field]);
	(*field_list[fp_cal_CPL_4400.field].left)(&field_list[fp_cal_CPL_4400.field]);
	(*field_list[fp_cal_CPL_4401.field].left)(&field_list[fp_cal_CPL_4401.field]);
	(*field_list[fp_cal_CPL_8000.field].left)(&field_list[fp_cal_CPL_8000.field]);
}

void	CAL_EFTDUMchanged()
{
	/* Not needed?? */
}

void	CAL_EFT9346RESET()
{
	moddata[eftEFTindex[fp_calEFT.lv]].modinfo.calinfo.data[CAL_LLOOFF_CLAMP]	= 250;
	moddata[eftEFTindex[fp_calEFT.lv]].modinfo.calinfo.data[CAL_LHIOFF_CLAMP]	= 4400;
	moddata[eftEFTindex[fp_calEFT.lv]].modinfo.calinfo.data[CAL_HLOOFF_CLAMP]	= 4401;
	moddata[eftEFTindex[fp_calEFT.lv]].modinfo.calinfo.data[CAL_HHIOFF_CLAMP]	= 8000;
	moddata[eftEFTindex[fp_calEFT.lv]].modinfo.calinfo.data[CAL_LLOOFF_CPL]	= 250;
	moddata[eftEFTindex[fp_calEFT.lv]].modinfo.calinfo.data[CAL_LHIOFF_CPL]	= 4400;
	moddata[eftEFTindex[fp_calEFT.lv]].modinfo.calinfo.data[CAL_HLOOFF_CPL]	= 4401;
	moddata[eftEFTindex[fp_calEFT.lv]].modinfo.calinfo.data[CAL_HHIOFF_CPL]	= 8000;
	moddata[eftEFTindex[fp_calEFT.lv]].modinfo.calinfo.data[CAL_LINTOFF_CLAMP]= 0;
	moddata[eftEFTindex[fp_calEFT.lv]].modinfo.calinfo.data[CAL_HINTOFF_CLAMP]= 0;
	moddata[eftEFTindex[fp_calEFT.lv]].modinfo.calinfo.data[CAL_LINTOFF_CPL]= 0;
	moddata[eftEFTindex[fp_calEFT.lv]].modinfo.calinfo.data[CAL_HINTOFF_CPL]= 0;
	CAL_EFTchanged();
	CAL_EFT9346BASE();
}

void	CAL_EFT9346WRITE()
{
	short	cept;

	cept = 104  - (((CAL_LDELTAY)/(fp_cal_CLAMP_4400.f - fp_cal_CLAMP_0250.f)) * fp_cal_CLAMP_0250.f) / 1000;
	moddata[eftEFTindex[fp_calEFT.lv]].modinfo.calinfo.data[CAL_LINTOFF_CLAMP] = cept;

	cept = 1833 - (((CAL_HDELTAY)/(fp_cal_CLAMP_8000.f - fp_cal_CLAMP_4401.f)) * fp_cal_CLAMP_4401.f) / 1000;
	moddata[eftEFTindex[fp_calEFT.lv]].modinfo.calinfo.data[CAL_HINTOFF_CLAMP] = cept;

	moddata[eftEFTindex[fp_calEFT.lv]].modinfo.calinfo.data[CAL_LLOOFF_CLAMP] = fp_cal_CLAMP_0250.f;
	moddata[eftEFTindex[fp_calEFT.lv]].modinfo.calinfo.data[CAL_LHIOFF_CLAMP] = fp_cal_CLAMP_4400.f;
	moddata[eftEFTindex[fp_calEFT.lv]].modinfo.calinfo.data[CAL_HLOOFF_CLAMP] = fp_cal_CLAMP_4401.f;
	moddata[eftEFTindex[fp_calEFT.lv]].modinfo.calinfo.data[CAL_HHIOFF_CLAMP] = fp_cal_CLAMP_8000.f;

	cept = 104  - (((CAL_LDELTAY)/(fp_cal_CPL_4400.f - fp_cal_CPL_0250.f)) * fp_cal_CPL_0250.f) / 1000;
	moddata[eftEFTindex[fp_calEFT.lv]].modinfo.calinfo.data[CAL_LINTOFF_CPL] = cept;

	cept = 1833 - (((CAL_HDELTAY)/(fp_cal_CPL_8000.f - fp_cal_CPL_4401.f)) * fp_cal_CPL_4401.f) / 1000;
	moddata[eftEFTindex[fp_calEFT.lv]].modinfo.calinfo.data[CAL_HINTOFF_CPL] = cept;

	moddata[eftEFTindex[fp_calEFT.lv]].modinfo.calinfo.data[CAL_LLOOFF_CPL] = fp_cal_CPL_0250.f;
	moddata[eftEFTindex[fp_calEFT.lv]].modinfo.calinfo.data[CAL_LHIOFF_CPL] = fp_cal_CPL_4400.f;
	moddata[eftEFTindex[fp_calEFT.lv]].modinfo.calinfo.data[CAL_HLOOFF_CPL] = fp_cal_CPL_4401.f;
	moddata[eftEFTindex[fp_calEFT.lv]].modinfo.calinfo.data[CAL_HHIOFF_CPL] = fp_cal_CPL_8000.f;
	CAL_EFT9346BASE();
}

void	CAL_EFT9346BASE()
{
	int	ox,oy;

	paraml[0].idata = eftEFTindex[fp_calEFT.lv];

	ox = LCD_getx();
	oy = LCD_gety();
	LCD_gotoxy(0,7);
	LCD_puts("Writing EEPROM NOW...  ");
	if(BAY_WRTE_pgm(paraml))
		KEY_beep();
	TPU_delay(250);
	LCD_gotoxy(0,7);
	LCD_puts("WRITE _____ RESET _____");
	LCD_gotoxy(ox,oy);
}

void	show_state_caleft(void)
{
	uint	tvar1,tvar2;

/*
 *	Set up array holdings for EFT modules.  The index into the
 *	array is same as the network id (no translation is needed).
 */
	for(tvar1 = 0, tvar2 = 0; tvar1 < MAX_MODULES; tvar1++)
		if(moddata[tvar1].modinfo.typePrimary & TYPE_EFT)
		{
		 	eftEFTnames[tvar2] = moddata[tvar1].modinfo.name;
			eftEFTindex[tvar2] = tvar1;
			if(eftdata.eftaddr == tvar1)
				fp_eftEFT.lv = tvar2;
			tvar2++;
		}
	eftEFTnames[tvar2] = 0x00;
	eftEFTindex[tvar2] = GHOST_BOX;

	fp_cal_CLAMP_0250.f		= moddata[eftEFTindex[fp_calEFT.lv]].modinfo.calinfo.data[CAL_LLOOFF_CLAMP];
	fp_cal_CLAMP_0250.minv	= 180;
	fp_cal_CLAMP_0250.maxv	= 320;
	fp_cal_CLAMP_0250.minus	= 0;

	fp_cal_CLAMP_4400.f		= moddata[eftEFTindex[fp_calEFT.lv]].modinfo.calinfo.data[CAL_LHIOFF_CLAMP];
	fp_cal_CLAMP_4400.minv	= 3500;
	fp_cal_CLAMP_4400.maxv	= 5200;
	fp_cal_CLAMP_4400.minus	= 0;

	fp_cal_CLAMP_4401.f		= moddata[eftEFTindex[fp_calEFT.lv]].modinfo.calinfo.data[CAL_HLOOFF_CLAMP];
	fp_cal_CLAMP_4401.minv	= 3500;
	fp_cal_CLAMP_4401.maxv	= 5200;
	fp_cal_CLAMP_4401.minus	= 0;

	fp_cal_CLAMP_8000.f		= moddata[eftEFTindex[fp_calEFT.lv]].modinfo.calinfo.data[CAL_HHIOFF_CLAMP];
	fp_cal_CLAMP_8000.minv	= 7000;
	fp_cal_CLAMP_8000.maxv	= 9000;
	fp_cal_CLAMP_8000.minus	= 0;

	fp_cal_CPL_0250.f		= moddata[eftEFTindex[fp_calEFT.lv]].modinfo.calinfo.data[CAL_LLOOFF_CPL];
	fp_cal_CPL_0250.minv	= 150;
	fp_cal_CPL_0250.maxv	= 320;
	fp_cal_CPL_0250.minus	= 0;

	fp_cal_CPL_4400.f		= moddata[eftEFTindex[fp_calEFT.lv]].modinfo.calinfo.data[CAL_LHIOFF_CPL];
	fp_cal_CPL_4400.minv	= 3000;
	fp_cal_CPL_4400.maxv	= 5200;
	fp_cal_CPL_4400.minus	= 0;

	fp_cal_CPL_4401.f		= moddata[eftEFTindex[fp_calEFT.lv]].modinfo.calinfo.data[CAL_HLOOFF_CPL];
	fp_cal_CPL_4401.minv	= 3500;
	fp_cal_CPL_4401.maxv	= 5200;
	fp_cal_CPL_4401.minus	= 0;

	fp_cal_CPL_8000.f		= moddata[eftEFTindex[fp_calEFT.lv]].modinfo.calinfo.data[CAL_HHIOFF_CPL];
	fp_cal_CPL_8000.minv	= 7000;
	fp_cal_CPL_8000.maxv	= 9000;
	fp_cal_CPL_8000.minus	= 0;

	LCD_gotoxy(11,0);
	LCD_puts("EFT CALIBRATION\n\r");
	LCD_puts("Network    :\n\r");
	LCD_puts("CLAMP  LOW : 250  V:       4400 V:\n\r");
	LCD_puts("CLAMP  HIGH: 4401 V:       8000 V:\n\r");
	LCD_puts("COUPLE LOW : 250  V:       4400 V:\n\r");
	LCD_puts("COUPLE HIGH: 4401 V:       8000 V:\n\r\n\r");
	LCD_puts("WRITE _____ RESET _____ ____ _____ _____");

	init_list_field(FLD_CALEFTx,FLD_CALEFTy,1,&fp_calEFT,CAL_EFTchanged,0x00,eftEFTnames,1);
	init_numeric_field(FLD_CAL_CLAMP0250Vx,FLD_CAL_CLAMP0250Vy,5,&fp_cal_CLAMP_0250,CAL_EFTDUMchanged,0x00,1);
	init_numeric_field(FLD_CAL_CLAMP4400Vx,FLD_CAL_CLAMP4400Vy,5,&fp_cal_CLAMP_4400,CAL_EFTDUMchanged,0x00,1);
	init_numeric_field(FLD_CAL_CLAMP4401Vx,FLD_CAL_CLAMP4401Vy,5,&fp_cal_CLAMP_4401,CAL_EFTDUMchanged,0x00,1);
	init_numeric_field(FLD_CAL_CLAMP8000Vx,FLD_CAL_CLAMP8000Vy,5,&fp_cal_CLAMP_8000,CAL_EFTDUMchanged,0x00,1);
	init_numeric_field(FLD_CAL_CPL_0250Vx,FLD_CAL_CPL_0250Vy,5,&fp_cal_CPL_0250,CAL_EFTDUMchanged,0x00,1);
	init_numeric_field(FLD_CAL_CPL_4400Vx,FLD_CAL_CPL_4400Vy,5,&fp_cal_CPL_4400,CAL_EFTDUMchanged,0x00,1);
	init_numeric_field(FLD_CAL_CPL_4401Vx,FLD_CAL_CPL_4401Vy,5,&fp_cal_CPL_4401,CAL_EFTDUMchanged,0x00,1);
	init_numeric_field(FLD_CAL_CPL_8000Vx,FLD_CAL_CPL_8000Vy,5,&fp_cal_CPL_8000,CAL_EFTDUMchanged,0x00,1);

	field_vertical(fp_calEFT.field, fp_cal_CPL_4401.field,fp_cal_CLAMP_0250.field);
	field_vertical(fp_cal_CLAMP_0250.field,fp_calEFT.field, fp_cal_CLAMP_4401.field);
	field_vertical(fp_cal_CLAMP_4401.field,fp_cal_CLAMP_0250.field,fp_cal_CPL_0250.field);
	field_vertical(fp_cal_CPL_0250.field,fp_cal_CLAMP_4401.field, fp_cal_CPL_4401.field);
	field_vertical(fp_cal_CPL_4401.field,fp_cal_CPL_0250.field,fp_calEFT.field);

	field_vertical(fp_cal_CLAMP_4400.field,fp_cal_CPL_8000.field,fp_cal_CLAMP_8000.field);
	field_vertical(fp_cal_CLAMP_8000.field,fp_cal_CLAMP_4400.field,fp_cal_CPL_4400.field);
	field_vertical(fp_cal_CPL_4400.field,fp_cal_CLAMP_8000.field,fp_cal_CPL_8000.field);
	field_vertical(fp_cal_CPL_8000.field,fp_cal_CPL_4400.field,fp_cal_CLAMP_4400.field);

	field_horizontal(fp_cal_CLAMP_0250.field,fp_cal_CLAMP_4400.field,fp_cal_CLAMP_4400.field);
	field_horizontal(fp_cal_CLAMP_4400.field,fp_cal_CLAMP_0250.field,fp_cal_CLAMP_0250.field);
	field_horizontal(fp_cal_CLAMP_4401.field,fp_cal_CLAMP_8000.field,fp_cal_CLAMP_8000.field);
	field_horizontal(fp_cal_CLAMP_8000.field,fp_cal_CLAMP_4401.field,fp_cal_CLAMP_4401.field);
	field_horizontal(fp_cal_CPL_0250.field,fp_cal_CPL_4400.field,fp_cal_CPL_4400.field);
	field_horizontal(fp_cal_CPL_4400.field,fp_cal_CPL_0250.field,fp_cal_CPL_0250.field);
	field_horizontal(fp_cal_CPL_4401.field,fp_cal_CPL_8000.field,fp_cal_CPL_8000.field);
	field_horizontal(fp_cal_CPL_8000.field,fp_cal_CPL_4401.field,fp_cal_CPL_4401.field);

	tvar1 = 0;
	for(tvar2 = 0; tvar2 < MAX_MODULES; tvar2++)
		if(moddata[tvar2].modinfo.typePrimary & TYPE_EFT)
			tvar1++;
	if(tvar1 <= 1)
		field_list[fp_calEFT.field].valid = 0;

	(*field_list[fp_calEFT.field ].left)(&field_list[fp_calEFT.field]);
	(*field_list[fp_cal_CLAMP_0250.field].left)(&field_list[fp_cal_CLAMP_0250.field]);
	(*field_list[fp_cal_CLAMP_4400.field].left)(&field_list[fp_cal_CLAMP_4400.field]);
	(*field_list[fp_cal_CLAMP_4401.field].left)(&field_list[fp_cal_CLAMP_4401.field]);
	(*field_list[fp_cal_CLAMP_8000.field].left)(&field_list[fp_cal_CLAMP_8000.field]);
	(*field_list[fp_cal_CPL_0250.field].left)(&field_list[fp_cal_CPL_0250.field]);
	(*field_list[fp_cal_CPL_4400.field].left)(&field_list[fp_cal_CPL_4400.field]);
	(*field_list[fp_cal_CPL_4401.field].left)(&field_list[fp_cal_CPL_4401.field]);
	(*field_list[fp_cal_CPL_8000.field].left)(&field_list[fp_cal_CPL_8000.field]);

	func_manager = eftcal_funcs;
}

uchar	eftcal_funcs(
char	c)
{
	ushort	tvar;

	switch(c)
	{
		case 'a':	/* F1	- Write to EEPROM	*/
			CAL_EFT9346WRITE();
			break;
		case 'c':	/* F3 - Reset Cal Data	*/
			CAL_EFT9346RESET();
			break;
		case 'M':	/* MENU - Main Menu		*/
			display.request = DISPLAY_LOCAL;
			break;
		default:
			break;
	}
	return(0);
}

void	show_state_eftmeas(void)
{
	fp_rmsPKL.f		= acdata.peak_lim;
	fp_rmsPKL.minv	= 0;
	fp_rmsPKL.maxv	= MAX_AC_PEAK;
	fp_rmsPKL.minus = 0;

	fp_rmsMIN.f		 = acdata.rmsmin_lim;
	fp_rmsMIN.minv	 = 0;
	fp_rmsMIN.maxv	 = MAX_AC_PEAK;
	fp_rmsMIN.minus = 0;

	fp_rmsMAX.f		 = acdata.rmsmax_lim;
	fp_rmsMAX.minv	 = 0;
	if ((fp_rmsMAX.maxv = moddata[acdata.acbox].modinfo.calinfo.data[CAL_RMSMAX_OFF]) <= 0)
		fp_rmsMAX.maxv = MAX_AC_PEAK;
	fp_rmsMAX.minus = 0;
	 
	LCD_gotoxy(0,0);
	LCD_puts("   AC MEASUREMENT CONFIGURATION\n\r\n\r\n\r");

	LCD_puts("Peak Max:      A\n\r");
	LCD_puts("RMS  Max:      A\n\r");
	LCD_puts("RMS  Min:      A");
	
	init_numeric_field(FLD_EFTPKLx,FLD_EFTPKLy,5,&fp_rmsPKL,RMS_PKLchanged,0x00,1);
	init_numeric_field(FLD_EFTMAXx,FLD_EFTMAXy,5,&fp_rmsMAX,RMS_MAXchanged,0x00,1);
	init_numeric_field(FLD_EFTMINx,FLD_EFTMINy,5,&fp_rmsMIN,RMS_MINchanged,0x00,1);

	field_vertical(fp_rmsPKL.field,fp_rmsMIN.field,fp_rmsMAX.field);
	field_vertical(fp_rmsMAX.field,fp_rmsPKL.field,fp_rmsMIN.field);
	field_vertical(fp_rmsMIN.field,fp_rmsMAX.field,fp_rmsPKL.field);

	func_manager = meas_funcs;
}

int	E421P_MaxDuration(
int	voltage,
uint	freq,
int	period)
{
	int	pulses;

	if(voltage > EFT_MAXVLT4400)
		pulses =  600;
	else
		if(voltage > EFT_MAXVLT3000)
			pulses = 2500;
		else
			if(voltage > EFT_MAXVLT2000)
				pulses = 5000;
			else
				pulses = 10000;

	if((freq > pulses) || (period > 1000))
		return(pulses);

	if(freq < pulses)
		pulses = freq;										/* Pulses per second		*/
	pulses = pulses * period / 1000;					/* total pulses in burst*/
	if(pulses > 10000)
		pulses = 10000;									/* Limit to 10000			*/
	return(pulses);
}

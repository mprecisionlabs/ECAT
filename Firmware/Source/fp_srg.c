#include <shortdef.h>
#include <float.h>
#include <math.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "display.h"
#include "parser.h"
#include "front.h"
#include "switches.h"
#include "module.h"
#include "ecat.h"
#include "intface.h"
#include "timer.h"
#include "tiocntl.h"
#include "field.h"
#include "acrms.h"
#include "data.h"

extern FILE		*destination;
extern uchar	noneTrack;
extern int		current_field;
extern uchar	calibration;
extern uchar	ecatmode;
extern uchar	(*func_manager)(char);
extern WAVDATA	wavdata[];
extern MODDATA	moddata[];
extern SRGDATA	srgdata;
extern SRGCHEAT	srgCheat;
extern CPLDATA	cpldata;
extern DISDATA	display;
extern PKDATA	peakdata;
extern ONE_FIELD field_list[];

extern volatile CHGDATA	chgdata;
extern ACDATA	acdata;
extern char	ac_linecnt;

char		SRG_CPLCheck(uchar);
uchar	SRG_CPLValidate(void);

void	SRG_VLTchanged();
void	SRG_WAVchanged();
void	SRG_DLYchanged();
void	SRG_ANGchanged();
void	SRG_OUTchanged();
void	SRG_NETchanged();
void	SRG_SRCchanged();
void	SRG_EUTchanged();
void	SRG_CPLchanged();
void	SRG_LSMchanged();
void	SRG_PKSchanged();
void	SRG_AUXchanged();
void	SRG_DTAchanged();
void	SRG_CMOchanged();
void	SRG_IMONchanged();
void	SRG_VMONHIchanged();
void	SRG_VMONLOchanged();
void	SRG_LSM_textset();
void	SRG_SetPeakNames(void);
void	SRG_UpdateSurgePeaks(char);
void	SRG_UpdateAcPeaks(void);

void	PK_SRCchanged();
void	PK_ICHchanged();
void	PK_VCHchanged();

void	RMS_PKLchanged();
void	RMS_MINchanged();
void	RMS_MAXchanged();

void	CAL_NETchanged();
void	CAL_WAVchanged();
void	CAL_DUMchanged();
void	CAL_WRTE9346();

NUMERIC_REC	fp_srgVLT;
NUMERIC_REC	fp_srgDLY;
NUMERIC_REC	fp_srgANG;
LIST_REC		fp_srgWAV;
LIST_REC		fp_srgNET;
LIST_REC		fp_srgOUT;
LIST_REC		fp_srgCL1;
LIST_REC		fp_srgCL2;
LIST_REC		fp_srgCL3;
LIST_REC		fp_srgCNU;
LIST_REC		fp_srgCPE;
LIST_REC		fp_srgSRC;
LIST_REC		fp_srgLSM;
LIST_REC		fp_srgAUX;
BOOL_REC		fp_srgEUT;
BOOL_REC		fp_srgDTA;
BOOL_REC		fp_srgCMO;
LIST_REC		fp_srgPKS;
P_PARAMT		paraml[3];

LIST_REC		fp_pkSRC;
LIST_REC		fp_pkVHI;
LIST_REC		fp_pkVLO;
LIST_REC		fp_pkICH;

NUMERIC_REC	fp_rmsPKL;
NUMERIC_REC	fp_rmsMIN;
NUMERIC_REC	fp_rmsMAX;

LIST_REC		fp_calNET;
LIST_REC		fp_calWAV;
NUMERIC_REC	fp_calK5D;
NUMERIC_REC	fp_calVFP;
NUMERIC_REC	fp_calCPL;
NUMERIC_REC	fp_calIO1;

uchar	notifyBell;
char	*listCPLnames[6];
char	*listWAVnames[MAX_WAVEFORMS+1];
char	*listSRGnames[MAX_SRGBOXES+1];
uchar	 listSRGindex[MAX_SRGBOXES+1];
char	*listOUTnames[MAX_CPLBOXES+2];
uchar	 listOUTindex[MAX_CPLBOXES+2];
char	*cplSRCnames[MAX_MODULES+1];
uchar	 cplSRCindex[MAX_MODULES+1];
char	*vmonNames[6];
char	*listLSMmodes[5];
char	*listPKSnames[5];
char	*listAUXmodes[4];

void	SRG_SetWaveNames(
char	boxaddr)
{
	listWAVnames[1] = 0x00;
	listWAVnames[2] = 0x00;
	listWAVnames[3] = 0x00;
	listWAVnames[4] = 0x00;
	switch(moddata[boxaddr].modinfo.id)
	{
        case E502_BOX:
        case E502A_BOX:
        case E502B_BOX:
        case E502C_BOX:
        case E502H_BOX:
            listWAVnames[0] = "0.5/700 us ";
            listWAVnames[1] = " 10/700 us ";
            if ( moddata[boxaddr].modinfo.options & OPT_SWE502 )
                listWAVnames[2] = "100/700 us ";
            break;
        case E502K_BOX:
            listWAVnames[0] = "1.2/50  us ";
            listWAVnames[1] = " 10/700 us ";
            if ( moddata[boxaddr].modinfo.options & OPT_SWE502 )
                listWAVnames[2] = "100/700 us ";
            break;
        case E503_BOX:
        case E503A_BOX:
            listWAVnames[0] = "100kHz 200A";
            listWAVnames[1] = "100kHz 500A";
            break;
        case E508_BOX:
            listWAVnames[0] = "10/360 100A";
            if ( cpldata.E50812Paddr < MAX_MODULES )
                listWAVnames[1] = "10/360  25A";
            break;
        case E509_BOX:
        case E509A_BOX:
            listWAVnames[0] = "600 V  100A";
            listWAVnames[1] = "1.0 kV 100A";
            listWAVnames[2] = "1.5 kV 100A";
            break;
        case E518_BOX:
            listWAVnames[0] = "600 V  100A";
            listWAVnames[1] = "1.0 kV 100A";
            listWAVnames[2] = "2.0 kV 200A";
            break;

        case E511_BOX:
            listWAVnames[0] = "1.2/50 us  ";
            break;
        case E506_BOX:
        case E506_4W_BOX:
            listWAVnames[0] = "800 V  100A";
            listWAVnames[1] = "1.5 kV 100A";
            listWAVnames[2] = "2.5 kV 500A";
            listWAVnames[3] = "5.0 kV 500A";
            break;
        case E505_BOX:
            listWAVnames[0] = "1.5 kV 200A";
            listWAVnames[1] = "800 V  100A";
            listWAVnames[2] = "800 V  200A";
            listWAVnames[3] = "2.5 kV 1 kA";
            break;
        case E505A_BOX:
            listWAVnames[0] = "1.5 kV 200A";
            listWAVnames[1] = "800 V  100A";
            listWAVnames[2] = "2.5 kV 1 kA";
            break;
        case E505B_BOX:
            listWAVnames[E505B_2500V] = "2.5 kV 1 kA";
            listWAVnames[E505B_1500V] = "1.5 kV 200A";
            listWAVnames[E505B_800V100A] = "800 V  100A";
            break;
        case E501_BOX:
        case E501A_BOX:
        case E501B_BOX:
        case E507_BOX:
        case E510_BOX:
        case E510A_BOX:
        case E521S_BOX:
        case E522S_BOX:
            listWAVnames[0] = " 2 Ohms    ";
            listWAVnames[1] = "12 Ohms    ";
            break;
        case E504A_BOX:
        case E504B_BOX:
            listWAVnames[2] = "1.2/50 125A";
        case E504_BOX:
            listWAVnames[0] = "1.2/50 500A";
            listWAVnames[1] = "1.2/50 750A";
            break;
        case E513_BOX:
            listWAVnames[0] = "0.1kV/us   ";
            listWAVnames[1] = "0.5kV/us   ";
            listWAVnames[2] = "1 kV/us    ";
            listWAVnames[3] = "5 kV/us    ";
            listWAVnames[4] = "10kV/us    ";
            break;
        case E514_BOX:
            listWAVnames[0] = "1.5kV   15A";
            listWAVnames[1] = "1kV/us  60A";
            listWAVnames[2] = "1kV    100A";
            listWAVnames[3] = "1kV/us 250A";
            break;
        case E515_BOX:
            listWAVnames[0] = "2 kV       ";
            listWAVnames[1] = "4 kV       ";
            break;
        default:
            listWAVnames[0] = "NET ERROR ";
            listWAVnames[1] = "NET ERROR ";
            break;
	}
}

void	SRG_VLTchanged()
{
	paraml[0].idata = fp_srgVLT.f;
	if(SRG_VLT_pgm(paraml))
		KEY_beep();
}

void	SRG_NETchanged()
{
	paraml[0].idata = listSRGindex[fp_srgNET.lv];
	if(SRG_NET_pgm(paraml))
	{
		KEY_beep();
 		return;
	}
/*
 *	The WAVEFORM will be RESET to the default waveform (WAVE-0)
 *	This means the Waveform List must be updated with the new surge
 *	network so the correct waveform names will appear in the list
 *	SRG_WAVchanged is then called to update any fields which may
 *	be affected by this change
 */
	if(fp_srgWAV.lv != srgdata.waveform)
	{
		fp_srgWAV.lv = 0;
		if(!notifyBell)
		{
			notifyBell = 1;
			KEY_beep();
		}
	}
	SRG_SetWaveNames((char)(srgdata.address));
	SRG_WAVchanged();
    SRG_SRCchanged();
	notifyBell = 0;
}

void	SRG_WAVchanged()
{
	int	ox,oy;
	int	tvar1,tvar2;

	if ( ( current_field == fp_srgWAV.field ) || ( moddata[srgdata.address].modinfo.id == E505B_BOX ) )
	{
        // need to call this for E505B to force RLY_RD23 to return to proper state following interlock error
		paraml[0].idata = fp_srgWAV.lv;
		if(SRG_WAV_pgm(paraml))
		{
			KEY_beep();
			return;
		}
	}
	ox = LCD_getx();
	oy = LCD_gety();
/*
 *	If this is being called when this is NOT the current field then
 *	the surge network has changed which means the waveform names have
 *	changed so the NEW waveform name must be written to the screen
 */
	if(current_field != fp_srgWAV.field)
	{
		LCD_gotoxy(FLD_SRGWAVx,FLD_SRGWAVy);
		LCD_puts(fp_srgWAV.labels[fp_srgWAV.lv]);
	}
/*
 *	Check the OUTPUT field to ensure the validity of the OUTPUT
 *	selection.  The SRG_WAV_pgm() command will have changed the
 *	data so it can be used to change to the appropriate field
 *	without having to search for a valid selection. Also, the
 *	SRG_OUTchanged function needs to be called so that the LINESYNC,
 *	ANGLE, and COUPLING fields will be properly dealt with and the
 *	OUTPUT field can be enabled or disabled accordingly
 */
	if(listOUTindex[fp_srgOUT.lv] != cpldata.outaddr)
	{
		for(tvar1 = 0; listOUTnames[tvar1]; tvar1++)
			if(listOUTindex[tvar1] == cpldata.outaddr)
				fp_srgOUT.lv = tvar1;
		if(!notifyBell)
		{
			notifyBell = 1;
			KEY_beep();
		}
	}
	listOUTnames[0] = listSRGnames[fp_srgNET.lv];
	if(moddata[(char)(srgdata.address)].modinfo.id == E508_BOX)
	{
		if(srgdata.waveform == E508_25A)
			listOUTnames[0] = "E508P";
	}
	LCD_gotoxy(FLD_SRGOUTx,FLD_SRGOUTy);
	LCD_puts(listOUTnames[fp_srgOUT.lv]);
	SRG_OUTchanged();
	LCD_gotoxy(ox,oy);
	notifyBell = 0;
}

void	SRG_OUTchanged()
{
	int	ox,oy;
	int	tvar1,tvar2;

	ox = LCD_getx();
	oy = LCD_gety();

	if(current_field == fp_srgOUT.field)
	{
		if(!(SRG_OutputValid(listOUTindex[fp_srgOUT.lv],srgdata.address,srgdata.waveform)))
		{
			tvar1 = fp_srgOUT.lv + 1;
			if(!listOUTnames[tvar1])
				tvar1 = 0;
			while(!SRG_OutputValid(listOUTindex[tvar1],srgdata.address,srgdata.waveform))
			{
				if(!listOUTnames[++tvar1])
					tvar1 = 0;
				if(fp_srgOUT.lv == tvar1)
					return;
			}
			fp_srgOUT.lv = tvar1;
			LCD_gotoxy(FLD_SRGOUTx,FLD_SRGOUTy);
			LCD_puts(listOUTnames[fp_srgOUT.lv]);
		}
		paraml[0].idata = listOUTindex[fp_srgOUT.lv];
		if(SRG_OUT_pgm(paraml))
		{
 			KEY_beep();
			return;
		}
	}
/*
 *	Check the VOLTAGE field to ensure the validity of the VOLTAGE
 *	selection.  The SRG_WAV_pgm() command will have changed the
 *	data so it can be used to change to the appropriate field
 *	without having to search for a valid selection. The SRG_VLTchanged
 *	function does not need to be called due to the SRG_WAV_pgm function
 */
	if(!calibration)
	{
		fp_srgVLT.maxv = SRG_MaximumVoltage(srgdata.address,srgdata.waveform);
		if(fp_srgVLT.maxv > MAX_FP_VOLTAGE)
			fp_srgVLT.maxv = MAX_FP_VOLTAGE;
		if(abs(fp_srgVLT.f) > fp_srgVLT.maxv)
		{
			fp_srgVLT.f = fp_srgVLT.maxv;
			LCD_gotoxy(FLD_SRGVLTx,FLD_SRGVLTy);
			fprintf(DISPLAY,"%+-6d",fp_srgVLT.f);
			if(!notifyBell)
			{
				notifyBell = 1;
				KEY_beep();
			}
		}
	}
/*
 *	The delay field is RESET so that the delay is always set to the
 *	minimum network/waveform/output.
 */
	fp_srgDLY.minv = SRG_MinimumDelay(srgdata.address,srgdata.waveform);
	fp_srgDLY.f = fp_srgDLY.minv;
	LCD_gotoxy(FLD_SRGDLYx,FLD_SRGDLYy);
	fprintf(DISPLAY,"%-3d",fp_srgDLY.f);

/*
 *	If the OUTPUT is going to the FRONT PANEL, then check to see if
 *	the current surge/wave combination can couple to any of the
 *	couplers and set the field to valid if it can.  However, the
 *	LINESYNC and ANGLE fields must be set to INVALID in this mode and
 *	the LINESYNC mode is set to default to RANDOM upon reactivation
 *
 *	Otherwise, if the OUTPUT is going to a coupler then the LINESYNC
 *	and ANGLE fields are made VALID.
 */
	if(cpldata.outaddr == GHOST_BOX)
	{
		field_list[fp_srgOUT.field].valid = 0;
		for(tvar1 = 1; listOUTnames[tvar1]; tvar1++)
		{
			if(SRG_OutputValid(listOUTindex[tvar1],srgdata.address,srgdata.waveform))
				field_list[fp_srgOUT.field].valid = 1;
		}
	}
/*
 *	Check to see if the coupling fields should be valid or not and if
 *	valid, check the validity of the coupling modes.  If there is a
 *	problem, the coupling modes are RESET for the next usage.  This
 *	will keep the coupling modes to stay constant until they need to
 *	be changed to a valid combination
 */
	SRG_CPLValidate();
/*
 *	VERIFY-JMM-12/28/94
 *	This has something to do with the peak measurement/AC data field of
 *	which I know nothing about.  I will look into this at a later date
 */
	if((peakdata.menuset == 0) && (peakdata.flag))
	{
		if((cpldata.outaddr != GHOST_BOX) && (moddata[(char)(cpldata.outaddr)].modinfo.viMonitors))
			tvar2 = cpldata.outaddr;
		else
			for(tvar1 = tvar2 = 0;(tvar1 < MAX_MODULES) && (!tvar2); tvar1++)
				if(moddata[tvar1].modinfo.viMonitors)
					tvar2 = tvar1;
		paraml[0].idata = tvar2;
		MEAS_BAY_pgm(paraml);
	}
	notifyBell = 0;
	LCD_gotoxy(ox,oy);
}

char	SRG_CPLCheck(
uchar	field)
{
	uchar	cplhi;
	uchar	cpllo;

	if(!field_list[fp_srgCL1.field].valid)
		return(0);

	cpllo = 0x00;
	cplhi = 0x00;

	if(fp_srgCL1.lv == 1)
		cpllo |= CPL_L1BIT;
	else
		if(fp_srgCL1.lv == 2)
			cplhi |= CPL_L1BIT;
			
	if(fp_srgCL2.lv == 1)
		cpllo |= CPL_L2BIT;
	else
 		if(fp_srgCL2.lv == 2)
			cplhi |= CPL_L2BIT;

	if(fp_srgCL3.lv == 1)
		cpllo |= CPL_L3BIT;
	else
		if(fp_srgCL3.lv == 2)
			cplhi |= CPL_L3BIT;

	if(fp_srgCNU.lv == 1)
		cpllo |= CPL_NUBIT;
	else
		if(fp_srgCNU.lv == 2)
			cplhi |= CPL_NUBIT;

	if(fp_srgCPE.lv == 1)
		cpllo |= CPL_PEBIT;
	else
		if(fp_srgCPE.lv == 2)
			cplhi |= CPL_PEBIT;

	paraml[0].idata = cplhi;
	paraml[1].idata = cpllo;
	if(SRG_CPL_pgm(paraml))
	{
		if(field_list[current_field].y_pos != FLD_SRGCL1y)
		{
			current_field = field;
			KEY_beep();
		}
		return(-1);
	}
	return(0);
}

void	SRG_SwitchLSMAUX(void)
{
	int	ox,oy;

	ox = LCD_getx();
	oy = LCD_gety();
	if((cpldata.outaddr >= MAX_MODULES) || (moddata[(char)(cpldata.outaddr)].modinfo.typePrimary & TYPE_CPLLINE))
	{
		field_vertical(fp_srgDLY.field,fp_srgEUT.field,fp_srgANG.field);
		field_vertical(fp_srgANG.field,fp_srgDLY.field,fp_srgEUT.field);
		field_vertical(fp_srgEUT.field,fp_srgANG.field,fp_srgDLY.field);
		field_vertical(fp_srgVLT.field,fp_srgOUT.field,fp_srgLSM.field);
		field_vertical(fp_srgLSM.field,fp_srgVLT.field,fp_srgSRC.field);
		field_vertical(fp_srgSRC.field,fp_srgLSM.field,fp_srgCL1.field);
		field_horizontal(fp_srgOUT.field,fp_srgOUT.field,fp_srgOUT.field);
		if(field_list[fp_srgDTA.field].valid)
		{
			LCD_gotoxy(21,1);
			LCD_puts("          ");
			LCD_gotoxy(0,3);
			LCD_puts("Phase Mode:         ");
			LCD_gotoxy(21,3);
			LCD_puts("Angle  :           ");
		}
		field_list[fp_srgDTA.field].valid = 0;
		field_list[fp_srgAUX.field].valid = 0;
		field_list[fp_srgCMO.field].valid = 0;
		SRG_LSM_textset();
		if((cpldata.outaddr < MAX_MODULES) && (!field_list[fp_srgLSM.field].valid))
		{
 			field_validation(&field_list[fp_srgLSM.field],1);

			/* Print "N/A" for the surge angle if line sync mode is "RNDM". */
			if(!fp_srgLSM.lv)
				field_validation(&field_list[fp_srgANG.field],0);
			else
				field_validation(&field_list[fp_srgANG.field],1);
		}
		else
		{
			field_validation(&field_list[fp_srgLSM.field],0);
			field_validation(&field_list[fp_srgANG.field],0);
		}
	}
	else
		if((moddata[(char)(cpldata.outaddr)].modinfo.typePrimary & TYPE_CPLDATA) &&
			(!field_list[fp_srgDTA.field].valid))
		{
			field_vertical(fp_srgDTA.field,fp_srgEUT.field,fp_srgDLY.field);
			field_vertical(fp_srgDLY.field,fp_srgDTA.field,fp_srgCMO.field);
			field_vertical(fp_srgCMO.field,fp_srgDLY.field,fp_srgEUT.field);
			field_vertical(fp_srgEUT.field,fp_srgCMO.field,fp_srgDTA.field);
			field_vertical(fp_srgVLT.field,fp_srgOUT.field,fp_srgAUX.field);
			field_vertical(fp_srgAUX.field,fp_srgVLT.field,fp_srgSRC.field);
			field_vertical(fp_srgSRC.field,fp_srgAUX.field,fp_srgCL1.field);
			field_horizontal(fp_srgOUT.field,fp_srgDTA.field,fp_srgDTA.field);
			field_horizontal(fp_srgDTA.field,fp_srgOUT.field,fp_srgOUT.field);
			LCD_gotoxy(21,1);
			LCD_puts("Data :");
			LCD_gotoxy(0,3);
			LCD_puts("AUX Clamp :");
			LCD_gotoxy(21,3);
			LCD_puts("C Mode :");
			field_list[fp_srgLSM.field].valid = 0;
			field_list[fp_srgANG.field].valid = 0;
 			field_validation(&field_list[fp_srgDTA.field],1);
 			field_validation(&field_list[fp_srgAUX.field],1);
			field_validation(&field_list[fp_srgCMO.field],1);
			if((moddata[(char)(srgdata.address)].modinfo.id == E508_BOX))
			{
				fp_srgCMO.bv = 1;
				paraml[0].idata = CMODE_SYMMETRIC;
				DATA_CMODE_pgm(paraml);
				(*field_list[fp_srgCMO.field].left)(&field_list[fp_srgCMO.field]);
				field_list[fp_srgCMO.field].valid = 0;
			}
		}
	LCD_gotoxy(ox,oy);
}

uchar	SRG_CPLValidate(void)
{
	int	ox,oy;
	uchar	tflg;
	uchar	tvar;

    ox = LCD_getx();
	oy = LCD_gety();
	LCD_gotoxy(0,FLD_SRGCL1y);

	tvar = (listOUTindex[fp_srgOUT.lv] != GHOST_BOX)?(1):(0);
	tflg = 0;
	if(tvar)
	{
        // 3-phase coupler
		if(moddata[listOUTindex[fp_srgOUT.lv]].modinfo.typePrimary & TYPE_3PHASE)
        {
			tflg = 1;
        }
		else
        {
            // data coupler
			if(moddata[listOUTindex[fp_srgOUT.lv]].modinfo.typePrimary & TYPE_CPLDATA)
            {
                if(moddata[(char)(srgdata.address)].modinfo.id == E508_BOX)
                    tflg = 3;  // E508 source only
				else
					tflg = 2;  // all other sources
            }
        }
	}
	if((!tflg) && (field_list[fp_srgCL3.field].valid) || (tflg == 3))
	{
		field_list[fp_srgCPE.field].x_pos = field_list[fp_srgCPE.field].x_end = FLD_SRGCPEx1;
		field_horizontal(fp_srgCL1.field,fp_srgCPE.field,fp_srgCL2.field);
		field_horizontal(fp_srgCL2.field,fp_srgCL1.field,fp_srgCPE.field);
		field_horizontal(fp_srgCPE.field,fp_srgCL2.field,fp_srgCL1.field);
		field_list[fp_srgCL3.field].valid = 0;
		field_list[fp_srgCNU.field].valid = 0;
		if(tflg == 3)
			LCD_puts("Coupling  : S1:   S2:    G:             ");
		else
			LCD_puts("Coupling  : L1:   L2:   PE:             ");
	}
	else
	{
		if(tflg)
		{
			field_list[fp_srgCPE.field].x_pos = field_list[fp_srgCPE.field].x_end = FLD_SRGCPEx0;
			field_horizontal(fp_srgCL1.field,fp_srgCPE.field,fp_srgCL2.field);
			field_horizontal(fp_srgCL2.field,fp_srgCL1.field,fp_srgCL3.field);
			field_horizontal(fp_srgCL3.field,fp_srgCL2.field,fp_srgCNU.field);
			field_horizontal(fp_srgCNU.field,fp_srgCL3.field,fp_srgCPE.field);
			field_horizontal(fp_srgCPE.field,fp_srgCNU.field,fp_srgCL1.field);
			if(tflg == 2)
			{
				field_list[fp_srgCNU.field].x_pos = field_list[fp_srgCNU.field].x_end = FLD_SRGCNUx1;
				LCD_puts("Coupling  : S1:   S2:   S3:   S4:   G:  ");
			}
			else
			{
				field_list[fp_srgCNU.field].x_pos = field_list[fp_srgCNU.field].x_end = FLD_SRGCNUx0;
				LCD_puts("Coupling  : L1:   L2:   L3:   N:   PE:  ");
			}
		}
	}
	if(tvar)
	{
		fp_srgCL1.lv = (cpldata.cpllow & CPL_L1BIT)?(1):((cpldata.cplhigh & CPL_L1BIT)?(2):(0));
		fp_srgCL2.lv = (cpldata.cpllow & CPL_L2BIT)?(1):((cpldata.cplhigh & CPL_L2BIT)?(2):(0));
		fp_srgCL3.lv = (cpldata.cpllow & CPL_L3BIT)?(1):((cpldata.cplhigh & CPL_L3BIT)?(2):(0));
		fp_srgCNU.lv = (cpldata.cpllow & CPL_NUBIT)?(1):((cpldata.cplhigh & CPL_NUBIT)?(2):(0));
		fp_srgCPE.lv = (cpldata.cpllow & CPL_PEBIT)?(1):(0);
	}
	field_validation(&field_list[fp_srgCL1.field],tvar);
	field_validation(&field_list[fp_srgCL2.field],tvar);
	field_validation(&field_list[fp_srgCPE.field],tvar);
	if((tflg) && (tflg != 3))
	{
		field_validation(&field_list[fp_srgCL3.field],tvar);
		field_validation(&field_list[fp_srgCNU.field],tvar);
	}
	SRG_SwitchLSMAUX();
	LCD_gotoxy(ox,oy);
	notifyBell = 0;
	return(field_list[fp_srgCL1.field].valid);
}

void	SRG_CPLchanged()
{
    ONE_FIELD *this_field = NULL;

    /* Handle custom validation for E505B coupler, for which L1 MUST be HI and L2 MUST be LO */
    if(moddata[(char)(cpldata.outaddr)].modinfo.id == E505B_BOX)
    {
        if( fp_srgCL1.lv != 2 )
        {
            this_field = &field_list[fp_srgCL1.field];
            fp_srgCL1.lv = 2;  // '2' is coupling to Surge HI
        }
        else if( fp_srgCL2.lv != 1 )
        {
            fp_srgCL2.lv = 1;  // '1' is coupling to Surge LO
            this_field = &field_list[fp_srgCL2.field];
        }
        else if( fp_srgCPE.lv != 0 )
        {
            fp_srgCPE.lv = 0;  // '0' is no coupling
            this_field = &field_list[fp_srgCPE.field];
        }

        if ( this_field )
        {
            LCD_gotoxy(this_field->x_pos,this_field->y_pos);
            LCD_puts(((LIST_REC *)(this_field->ref_rec))->labels[((LIST_REC *)(this_field->ref_rec))->lv]);
            LCD_gotoxy(this_field->x_pos,this_field->y_pos);
            LCD_hilight(this_field->x_pos,this_field->y_pos,this_field->x_pos+((LIST_REC *)(this_field->ref_rec))->loc_len,this_field->y_end);
            KEY_beep();
        }
    }
}

void	SRG_LSMchanged()
{
	paraml[0].idata = fp_srgLSM.lv;

	if(LI_MODE_pgm(paraml))
		KEY_beep();

	if(!fp_srgLSM.lv)
		field_validation(&field_list[fp_srgANG.field],0);
	else
		field_validation(&field_list[fp_srgANG.field],1);
}

void	SRG_ANGchanged()
{
	paraml[0].idata = fp_srgANG.f;
	if(LI_ANG_pgm(paraml))
		KEY_beep();
}

void	SRG_DLYchanged()
{
	paraml[0].idata = fp_srgDLY.f;
	if(SRG_DLY_pgm(paraml))
		KEY_beep();
}

// EUT (AC) source
void	SRG_SRCchanged()
{
	int	ox,oy;

    /* Special case:  E505B can ONLY use E505B source, NO OTHER module can use E505B source */

    if ( !SRG_SourceValid( cplSRCindex[fp_srgSRC.lv], srgdata.address ) )
    {
        int source_index = fp_srgSRC.lv + 1;
        while( !SRG_SourceValid( cplSRCindex[source_index], srgdata.address ) )
        {
            source_index++;
            if ( !cplSRCnames[source_index] )
                source_index = 0;  // wrap to start of source list
            if ( fp_srgSRC.lv == source_index )
                return;  // searched entire source list, didn't find valid entry
        }
        fp_srgSRC.lv = source_index;
        LCD_gotoxy( FLD_SRGSRCx,FLD_SRGSRCy );
        LCD_puts( cplSRCnames[fp_srgSRC.lv] );
    }

	paraml[0].idata = cplSRCindex[fp_srgSRC.lv];
	if(root_CPL_pgm(paraml))
		KEY_beep();

    SRG_SetPeakNames();
	fp_srgPKS.lv = (acdata.acbox == GHOST_BOX)?(0):(acdata.line);

	/* EUT POWER field is INVALID if NO MAINS is selected	*/
	if(cpldata.eutrequest == 0)
		fp_srgEUT.bv = 0;
	if(cpldata.srcaddr != GHOST_BOX)
	{
		ox = LCD_getx();
		oy = LCD_gety();
		field_validation(&field_list[fp_srgEUT.field],1);
		LCD_gotoxy(FLD_SRGEUTx,FLD_SRGEUTy);
		LCD_puts(((fp_srgEUT.bv)?(fp_srgEUT.true_label):(fp_srgEUT.false_label)));
		LCD_gotoxy(ox,oy);
	}
	else
	{
		field_validation(&field_list[fp_srgEUT.field],0);
	}
}

void	SRG_EUTchanged()
{
	paraml[0].idata = fp_srgEUT.bv;
 	if(root_EUT_pgm(paraml))
		KEY_beep();
}

void	SRG_PKSchanged()
{
	if(acdata.acbox != GHOST_BOX)
		acdata.line = fp_srgPKS.lv;
	if(fp_srgPKS.lv == 0)
		SRG_UpdateSurgePeaks(0);
	else
		SRG_UpdateAcPeaks();
}

void	SRG_SetPeakNames(void)
{
	listPKSnames[0] = "Surge Pks";
	if(acdata.acbox == GHOST_BOX)
	{
		listPKSnames[1] = 0x00;
		fp_srgPKS.lv = 0;
	}
	else
	{
		listPKSnames[1] = "L1 AC Pks";
		listPKSnames[2] = "L2 AC Pks";
		listPKSnames[3] = "L3 AC Pks";
		listPKSnames[ac_linecnt + 1] = 0x00;
		fp_srgPKS.lv = acdata.line;
	}
}

void	SRG_AUXchanged()
{
	paraml[0].idata = fp_srgAUX.lv;
	if(DATA_CLAMP_pgm(paraml))
	{
		puts("AUXchanged\n");
		KEY_beep();
	}
}

void	SRG_DTAchanged()
{
	paraml[0].idata = fp_srgDTA.bv;
	if(DATA_OUT_pgm(paraml))
	{
		puts("DTAchanged\n");
		KEY_beep();
	}
}

void	SRG_CMOchanged()
{
	paraml[0].idata = fp_srgCMO.bv;
	if(DATA_CMODE_pgm(paraml))
	{
		puts("CMOchanged\n");
		KEY_beep();
	}
}

void	SRG_UpdateSurgePeaks(
char	display_srg_peaks)
{
	int	ox,oy;
	int	px,py;

	ox = LCD_getx();
	oy = LCD_gety();

	LCD_gotoxy( 10, 6 );
	if( peakdata.peakaddr == GHOST_BOX )
	{
		LCD_puts( ":    NA/NA          NA/NA     " );
	}
	else
	{
		if( display_srg_peaks )
		{
			/* If the peak detector surge detection algorithm didn't see the
				surge happen, then print a semicolon instead of a colon,
				to indicate the uncertainty of the data. */
			if ( !SRG_GetSurgeDetected() )
			{
				LCD_puts( ";" );
			}
			else
			{
				LCD_puts( ":" );
			}
			fprintf( DISPLAY, "%+6d/%+6dV %+6d/%+6dA", peakdata.vpeakpos,
				peakdata.vpeakneg, peakdata.ipeakpos, peakdata.ipeakneg );
		}
		else
		{
			LCD_puts( ":    +0/    -0V     +0/    -0A" );
		}
	}
	LCD_gotoxy(ox,oy);
}

void	SRG_UpdateAcPeaks(void)
{
	int	ox,oy,l;
	float	rms;

	ox = LCD_getx();
	oy = LCD_gety();

	LCD_gotoxy(11,6);
	if(acdata.measure_on)
	{
		l = fp_srgPKS.lv - 1;
		rms = sqrt(acdata.ldata[l].rms);
		fprintf(DISPLAY,"%+6.1fA %+6.1fA  RMS: %+6.1fA",AmpsPeak(acdata.ldata[l].peakp),
 				AmpsPeak(acdata.ldata[l].peakm),AmpsPeak(rms));
	}
	else
		LCD_puts("  +0.0A   +0.0A  RMS:   +0.0A");
	LCD_gotoxy(ox,oy);
}

void	SRG_UpdateCharge(
ushort	time)
{
	int	ox,oy;

	if(display.current == DISPLAY_SRG)
	{
		ox = LCD_getx();
		oy = LCD_gety();
		LCD_gotoxy(32,1);
		if(time)
			fprintf(DISPLAY,"Time:%-3d",time/8);
		else
		{
			LCD_puts("        ");
			if(chgdata.flag == CHG_DELAY)
			{
				LCD_gotoxy(32,0);
				LCD_puts("IDLE    ");
				chgdata.flag = CHG_IDLE;
			}
		}
		LCD_gotoxy(ox,oy);
	}
	else
		if(chgdata.flag == CHG_DELAY && !time)
			chgdata.flag = CHG_IDLE;
}

char	SRG_StartSystem(void)
{
	int	ox,oy;
	uchar	tvar;

	if(chgdata.flag != CHG_IDLE)
		return(ERR_SRGIDLE);

	if ( !srgCheat.lineCheck )
	{
		if(display.current == DISPLAY_SRG)
			tvar = ECAT_CheckEUT(PHASE_TPU,1);
		else
			tvar = ECAT_CheckEUT(PHASE_TPU,0);
		if(tvar)
			return(tvar);
	}

	peakdata.vpeakpos = 0;
	peakdata.ipeakpos = 0;
	peakdata.vpeakneg = 0;
	peakdata.ipeakneg = 0;

	if(display.current == DISPLAY_SRG)
	{
		if(SRG_CPLCheck(current_field))
		{
			KEY_beep();
			return(0);
		}
		ox = LCD_getx();
		oy = LCD_gety();
		if(peakdata.flag && fp_srgPKS.lv == 0)  	/* Reset the PEAKS LCD data line */
			SRG_UpdateSurgePeaks(0);

		LCD_gotoxy(32,0);
		LCD_puts("CHARGING");
		LCD_gotoxy(ox,oy);
		if(field_list[current_field].left)
			(*field_list[current_field].left)(&field_list[current_field]);
		if(field_list[current_field].entered)
			(*field_list[current_field].entered)(&field_list[current_field]);
	}
	KEY_led(STOP_LED,0);						/* Turn OFF the STOP led				*/
	KEY_led(GO_LED,1);						/* Turn ON  the START led				*/

 	chgdata.flag = CHG_RAMP;				/* Set the flag to indicate RAMPING	*/
	chgdata.time = srgdata.delay * 8;	/* PIT number is multiplied by 8		*/
	SRG_UpdateCharge(chgdata.time);		/* Reflect ramping on LCD screen		*/
	return(SRG_SurgeStart());
}

char	SRG_StopSystem(void)
{
	int	ox,oy;

	if(display.current == DISPLAY_SRG)
		if(chgdata.flag == CHG_IDLE)
		{
			ox = LCD_getx();
			oy = LCD_gety();
			LCD_gotoxy(32,0);
			LCD_puts("IDLE    ");
			LCD_gotoxy(ox,oy);
			return(-1);
		}

	chgdata.flag = CHG_DELAY;
	if(chgdata.time <= 0)
		SRG_UpdateCharge(chgdata.time = 0);
	else
		chgdata.flag = CHG_DELAY;
	KEY_led(GO_LED,0);
	KEY_led(STOP_LED,1);
	if((display.current == DISPLAY_SRG) && (chgdata.flag == CHG_DELAY))
	{
		ox = LCD_getx();
		oy = LCD_gety();
		LCD_gotoxy(32,0);
		LCD_puts("WAITING ");
		LCD_gotoxy(ox,oy);
	}
	SRG_SurgeKill();
}

char	SRG_UpdateSystem(void)
{
	int	ox,oy;

	if((chgdata.flag != CHG_READY) || (display.current != DISPLAY_SRG))
		return(-1);

	ox = LCD_getx();
	oy = LCD_gety();
	LCD_gotoxy(32,0);
	LCD_puts("READY   ");
	LCD_gotoxy(ox,oy);
	chgdata.flag = CHG_READY;
}

char	SRG_SurgeSystem(void)
{
	int	ox,oy;
	char	retval;
	uchar	rms_on;

	if(chgdata.flag != CHG_READY)
		return(ERR_SRGRDY);

	if(display.current == DISPLAY_SRG)
	{
		ox = LCD_getx();
		oy = LCD_gety();
		LCD_gotoxy(32,0);
		LCD_puts("SURGING ");
		LCD_gotoxy(ox,oy);
	}
	if(rms_on = acdata.measure_on)
		RMS_Disable();					       	   /* disable rms measurement	*/

	retval = SRG_SurgeTrigger();

	SRG_StopSystem();

	if(rms_on)
		RMS_Enable();

	if(!retval)
	{
		if((peakdata.peakaddr != GHOST_BOX) && (display.current == DISPLAY_SRG) &&
			(fp_srgPKS.lv == 0))
			SRG_UpdateSurgePeaks(1);
	}

    return(retval);
}

/*
 *	Modification on 10/06/95 by James Murphy (JMM)
 *		The DATA fields were not being updated on the VFP screen
 *		from the current parameters due to the fact that the
 *		fields were not being initialized
 */
void	SRG_FPDataSet(
SRGDATA	*srgptr,
CPLDATA	*cplptr)
{
	uint	module_index, SRG_index, OUT_index, SRC_index, i;

	fp_srgVLT.f		= srgptr->voltage;
	fp_srgVLT.minv	= 0;
	fp_srgVLT.maxv	= SRG_MaximumVoltage(srgptr->address,srgptr->waveform);
	fp_srgVLT.minus= 1;

	fp_srgDLY.f		 = srgptr->delay;
	fp_srgDLY.minv	 = SRG_MinimumDelay(srgptr->address,srgptr->waveform);
	fp_srgDLY.maxv	 = MAX_SRGDELAY;
	fp_srgDLY.minus = 0;

	fp_srgANG.f		 = cplptr->angle;
	fp_srgANG.minv	 = 0;
	fp_srgANG.maxv	 = 360;
	fp_srgANG.minus = 0;

	fp_srgWAV.lv = srgptr->waveform;
	fp_srgEUT.bv = cplptr->eutrequest;
	fp_srgLSM.lv = cplptr->phasemode;

	/* Setup the DATA Coupler fields even though they may not be used */
	fp_srgAUX.lv = cplptr->clamp;
	fp_srgDTA.bv = cplptr->output;
	fp_srgCMO.bv = cplptr->cmode;

/*
 *	Set up array holdings for network modules.  The index into the
 *	array is same as the network id (no translation is needed).
 */
	for(module_index = 0, SRG_index = 0; module_index < MAX_MODULES; module_index++)
		if(moddata[module_index].modinfo.typePrimary & TYPE_SRG)
		{
		 	listSRGnames[SRG_index] = moddata[module_index].modinfo.name;
			listSRGindex[SRG_index] = module_index;
			if(srgptr->address == module_index)
				fp_srgNET.lv = SRG_index;
			SRG_index++;
		}
	listSRGnames[SRG_index] = 0x00;
	listSRGindex[SRG_index] = GHOST_BOX;

/*
 *	Set up array holdings for output modules.  The index into the
 *	array is the same as the output id+1.  This is due to the fact
 *	that the first slot (0-element) must be a network name.
 */
	listOUTnames[0] = listSRGnames[fp_srgNET.lv];
	listOUTindex[0] = GHOST_BOX;
	cplSRCnames[0]  = "N/A  ";
	cplSRCindex[0]  = GHOST_BOX;
	fp_srgOUT.lv = 0;
	fp_srgSRC.lv = 0;
	for(module_index = 0, OUT_index = 1, SRC_index = 0; module_index < MAX_MODULES; module_index++)
	{
		if((moddata[module_index].modinfo.typePrimary & TYPE_CPLSRG) &&
			(moddata[module_index].modinfo.id != E508P_BOX))
		{
		 	listOUTnames[OUT_index] = moddata[module_index].modinfo.name;
			listOUTindex[OUT_index] = module_index;
			if(cplptr->outaddr == module_index)
				fp_srgOUT.lv = OUT_index;
			OUT_index++;
		}
		if(moddata[module_index].modinfo.typePrimary & TYPE_CPLLINE)
		{
		 	cplSRCnames[SRC_index] = moddata[module_index].modinfo.name;
			cplSRCindex[SRC_index] = module_index;
			if(cplptr->srcaddr == module_index)
				fp_srgSRC.lv = SRC_index;
			SRC_index++;
		}
	}
	listOUTnames[OUT_index]	= 0x00;
	listOUTindex[OUT_index]	= GHOST_BOX;
	if(SRC_index == 0)
		SRC_index = 1;
	cplSRCindex[SRC_index]	= GHOST_BOX;
	cplSRCnames[SRC_index]	= 0x00;

	if(peakdata.flag)
		SRG_SetPeakNames();
}

void	show_state_srg(void)
{
	uint	tvar1,tvar2;

	notifyBell = 0;
	SRG_FPDataSet(&srgdata,&cpldata);

	SRG_SetWaveNames((char)(srgdata.address));
	listCPLnames[0] = "--";
	listCPLnames[1] = "LO";
	listCPLnames[2] = "HI";
	listCPLnames[3] = 0x00;
   
	listLSMmodes[0] = "RNDM";
	listLSMmodes[1] = "L1  ";
	listLSMmodes[2] = 0x00;

	listAUXmodes[0] = "INT-20V ";
	listAUXmodes[1] = "INT-220V";
	listAUXmodes[2] = "EXTERNAL";
 	listAUXmodes[3] = 0x00;

	LCD_gotoxy(0,0);
	LCD_puts("Network   :       -             IDLE\n\r");
	LCD_puts("Output at :\n\r");
	LCD_puts("Voltage   :       V  Delay  :     Secs\n\r");
	LCD_puts("Phase Mode:          Angle  :     degs\n\r");
	LCD_puts("EUT Mains :          EUT Pwr:\n\r");
	LCD_puts("Coupling  :\n\r");

	if(peakdata.flag)
		LCD_puts("Surge Pks :    +0/    -0V     +0/    -0A");
	else
		LCD_puts("\n\r");
	if(calibration)
		LCD_puts("CHRGE _____ PULSE ___CALIBRATION__ MEAS");
	else 
		LCD_puts("CHRGE _____ PULSE _____ ____ _____ MEAS");

	init_list_field(FLD_SRGNETx,FLD_SRGNETy,1,&fp_srgNET,SRG_NETchanged,0x00,listSRGnames,1);
	init_list_field(FLD_SRGWAVx,FLD_SRGWAVy,1,&fp_srgWAV,SRG_WAVchanged,0x00,listWAVnames,1);
	init_list_field(FLD_SRGOUTx,FLD_SRGOUTy,1,&fp_srgOUT,SRG_OUTchanged,0x00,listOUTnames,1);
	init_bool_field(FLD_SRGDTAx,FLD_SRGDTAy,&fp_srgDTA,SRG_DTAchanged,0x00,"OFF","ON ",0);
	init_numeric_field(FLD_SRGVLTx,FLD_SRGVLTy,6,&fp_srgVLT,SRG_VLTchanged,0x00,1);
	init_numeric_field(FLD_SRGDLYx,FLD_SRGDLYy,3,&fp_srgDLY,SRG_DLYchanged,0x00,1);
	init_list_field(FLD_SRGLSMx,FLD_SRGLSMy,1,&fp_srgLSM,SRG_LSMchanged,0x00,listLSMmodes,1);
	init_list_field(FLD_SRGLSMx,FLD_SRGLSMy,1,&fp_srgAUX,SRG_AUXchanged,0x00,listAUXmodes,0);
	init_numeric_field(FLD_SRGANGx,FLD_SRGANGy,3,&fp_srgANG,SRG_ANGchanged,0x00,1);
	init_bool_field(FLD_SRGANGx,FLD_SRGANGy,&fp_srgCMO,SRG_CMOchanged,0x00,"ASYMMETRIC","SYMMETRIC ",0);
	init_list_field(FLD_SRGSRCx,FLD_SRGSRCy,1,&fp_srgSRC,SRG_SRCchanged,0x00,cplSRCnames,1);
	init_bool_field(FLD_SRGEUTx,FLD_SRGEUTy,&fp_srgEUT,SRG_EUTchanged,0x00,"OFF  ","READY",1);
	init_list_field(FLD_SRGCL1x,FLD_SRGCL1y,0,&fp_srgCL1,SRG_CPLchanged,SRG_CPLCheck,listCPLnames,0);
	init_list_field(FLD_SRGCL2x,FLD_SRGCL2y,0,&fp_srgCL2,SRG_CPLchanged,SRG_CPLCheck,listCPLnames,0);
	init_list_field(FLD_SRGCL3x,FLD_SRGCL3y,0,&fp_srgCL3,SRG_CPLchanged,SRG_CPLCheck,listCPLnames,0);
	init_list_field(FLD_SRGCNUx0,FLD_SRGCNUy0,0,&fp_srgCNU,SRG_CPLchanged,SRG_CPLCheck,listCPLnames,0);
	init_list_field(FLD_SRGCPEx0,FLD_SRGCPEy0,0,&fp_srgCPE,SRG_CPLchanged,SRG_CPLCheck,listCPLnames,0);

	if(peakdata.flag)
		init_list_field(FLD_SRGPKSx,FLD_SRGPKSy,0,&fp_srgPKS,SRG_PKSchanged,0x00,listPKSnames,1);

/*
 *	Write in correct field labels instead of dummy labels used
 *	during field setup.
 */
	(*field_list[fp_srgNET.field].left)(&field_list[fp_srgNET.field]);
	(*field_list[fp_srgWAV.field].left)(&field_list[fp_srgWAV.field]);
	(*field_list[fp_srgOUT.field].left)(&field_list[fp_srgOUT.field]);
	(*field_list[fp_srgVLT.field].left)(&field_list[fp_srgVLT.field]);
	(*field_list[fp_srgDLY.field].left)(&field_list[fp_srgDLY.field]);
	(*field_list[fp_srgSRC.field].left)(&field_list[fp_srgSRC.field]);
	(*field_list[fp_srgEUT.field].left)(&field_list[fp_srgEUT.field]);
	if(peakdata.flag)
	{
		SRG_PKSchanged();
		(*field_list[fp_srgPKS.field].left)(&field_list[fp_srgPKS.field]);
	}

/*
 *	Check for invalid fields
 *		1.) NETWORK field is invalid for ONE or less surge networks
 *		2.) EUT SOURCE field is invalid for ONE or less coupler modules
 *		4.) OUTPUT field is invalid for ZERO couplers
 *		5.) EUT POWER field is invalid for ZERO couplers
 *		6.) PHASE MODE field is invalid for ZERO couplers
 */
	tvar1 = 0;
	for(tvar2 = 0; tvar2 < MAX_MODULES; tvar2++)
		if(moddata[tvar2].modinfo.typePrimary & TYPE_SRG)
			tvar1++;
	if(tvar1 <= 1)
		field_list[fp_srgNET.field].valid = 0;
	SRG_NETchanged();
/*
 *	Check to see whether or not the OUTPUT field should be VALID or INVALID
 *	This is checked by seeing if the current network/waveform combination
 *	is valid with any of the available couplers.  If it is then the field
 *	is designated VALID
 */
	field_list[fp_srgOUT.field].valid = 0;
	for(tvar1 = 1; listOUTnames[tvar1]; tvar1++)
		if(SRG_OutputValid(listOUTindex[tvar1],srgdata.address,srgdata.waveform))
			field_list[fp_srgOUT.field].valid = 1;

	if(!cpldata.cplflag)
	{
		field_validation(&field_list[fp_srgSRC.field],0);
		field_validation(&field_list[fp_srgEUT.field],0);
	}
	SRG_SRCchanged();

/*
 *	These three fields are initially non-existant so they must be
 *	disabled until they are needed for a DATA coupler output
 */
	field_list[fp_srgDTA.field].valid = 0;
	field_list[fp_srgAUX.field].valid = 0;
	field_list[fp_srgCMO.field].valid = 0;
	SRG_CPLValidate();

	if(peakdata.flag)
	{
		field_vertical(fp_srgNET.field,fp_srgPKS.field,fp_srgOUT.field);
		field_vertical(fp_srgPKS.field,fp_srgCL1.field,fp_srgNET.field);
		field_vertical(fp_srgCL1.field,fp_srgSRC.field,fp_srgPKS.field);
		field_vertical(fp_srgCL2.field,fp_srgSRC.field,fp_srgPKS.field);
		field_vertical(fp_srgCL3.field,fp_srgSRC.field,fp_srgPKS.field);
		field_vertical(fp_srgCNU.field,fp_srgSRC.field,fp_srgPKS.field);
		field_vertical(fp_srgCPE.field,fp_srgSRC.field,fp_srgPKS.field);
	}
	else
	{
		field_vertical(fp_srgNET.field,fp_srgCL1.field,fp_srgOUT.field);
		field_vertical(fp_srgCL1.field,fp_srgSRC.field,fp_srgNET.field);
		field_vertical(fp_srgCL2.field,fp_srgSRC.field,fp_srgNET.field);
		field_vertical(fp_srgCL3.field,fp_srgSRC.field,fp_srgNET.field);
		field_vertical(fp_srgCNU.field,fp_srgSRC.field,fp_srgNET.field);
		field_vertical(fp_srgCPE.field,fp_srgSRC.field,fp_srgNET.field);
	}
	field_vertical(fp_srgOUT.field,fp_srgNET.field,fp_srgVLT.field);

	field_horizontal(fp_srgNET.field,fp_srgWAV.field,fp_srgWAV.field);
	field_horizontal(fp_srgWAV.field,fp_srgNET.field,fp_srgNET.field);
	field_horizontal(fp_srgVLT.field,fp_srgDLY.field,fp_srgDLY.field);
	field_horizontal(fp_srgDLY.field,fp_srgVLT.field,fp_srgVLT.field);
	field_horizontal(fp_srgLSM.field,fp_srgANG.field,fp_srgANG.field);
	field_horizontal(fp_srgANG.field,fp_srgLSM.field,fp_srgLSM.field);
	field_horizontal(fp_srgLSM.field,fp_srgANG.field,fp_srgANG.field);
	field_horizontal(fp_srgAUX.field,fp_srgCMO.field,fp_srgCMO.field);
	field_horizontal(fp_srgCMO.field,fp_srgAUX.field,fp_srgAUX.field);
	field_horizontal(fp_srgSRC.field,fp_srgEUT.field,fp_srgEUT.field);
	field_horizontal(fp_srgEUT.field,fp_srgSRC.field,fp_srgSRC.field);

	func_manager = srg_funcs;
}

void	PK_SRCchanged()
{
	if(peakdata.peakaddr == cplSRCindex[fp_pkSRC.lv])
		return;

	paraml[0].idata = cplSRCindex[fp_pkSRC.lv];
	if(MEAS_BAY_pgm(paraml))
	{
		KEY_beep();
	}
	fp_pkVHI.lv = peakdata.vmonhi - 1;
	fp_pkVLO.lv = peakdata.vmonlo - 1;
	fp_pkICH.lv = peakdata.imon - 1;
	(*field_list[fp_pkVHI.field].left)(&field_list[fp_pkVHI.field]);
	(*field_list[fp_pkVLO.field].left)(&field_list[fp_pkVLO.field]);
	(*field_list[fp_pkICH.field].left)(&field_list[fp_pkICH.field]);
}

void	PK_ICHchanged()
{
	uchar	viI;

	if(peakdata.peakaddr != GHOST_BOX)
		viI = (moddata[(char)(peakdata.peakaddr)].modinfo.viMonitors) & 0x0f;
	else
		viI = 5;

	fp_pkICH.lv %= viI;
	LCD_gotoxy(FLD_PKICHx,FLD_PKICHy);
	LCD_puts(fp_pkICH.labels[fp_pkICH.lv]);

	paraml[0].idata = fp_pkICH.lv + 1;
	if(MEAS_ICH_pgm(paraml))
		KEY_beep();
}

void	PK_VCHchanged()
{
	uchar	viV;

	if(peakdata.peakaddr != GHOST_BOX)
		viV = ((moddata[(char)(peakdata.peakaddr)].modinfo.viMonitors >> 4) & 0x0f) + 1;
	else
		viV = 5;

	if(current_field == fp_pkVHI.field)
	{
		if(viV == 5)
			viV = 4;
		fp_pkVHI.lv %= viV;
		if(fp_pkVHI.lv == fp_pkVLO.lv)
			fp_pkVHI.lv = (fp_pkVHI.lv+1) % viV;
		LCD_gotoxy(FLD_PKVHIx,FLD_PKVHIy);
		LCD_puts(fp_pkVHI.labels[fp_pkVHI.lv]);
	}
	else
		if(current_field == fp_pkVLO.field)
		{
			fp_pkVLO.lv %= viV;
			if(fp_pkVHI.lv == fp_pkVLO.lv)
				fp_pkVLO.lv = (fp_pkVLO.lv+1) % viV;
			LCD_gotoxy(FLD_PKVLOx,FLD_PKVLOy);
			LCD_puts(fp_pkVLO.labels[fp_pkVLO.lv]);
		}

	paraml[0].idata = (((fp_pkVHI.lv+1) << 4) & 0xf0) + ((fp_pkVLO.lv+1) & 0x0f);
	if(MEAS_VCH_pgm(paraml))
		KEY_beep();
}

void	RMS_PKLchanged()
{
	paraml[0].idata = fp_rmsPKL.f;
	if(RMS_PLIM_pgm(paraml))
		KEY_beep();
}

void RMS_MINchanged()
{
	paraml[0].idata = fp_rmsMIN.f;
	if(RMS_SMIN_pgm(paraml))
		KEY_beep();
}

void RMS_MAXchanged()
{
	paraml[0].idata = fp_rmsMAX.f;
	if(RMS_SMAX_pgm(paraml))
		KEY_beep();
}

void	show_state_srgmeas(void)
{
	char	tvar1;
	char	tvar2;

	for(tvar1 = 0, tvar2 = 0; tvar1 < MAX_MODULES; tvar1++)
		if(moddata[tvar1].modinfo.viMonitors)
		{
		 	cplSRCnames[tvar2] = moddata[tvar1].modinfo.name;
			cplSRCindex[tvar2] = tvar1;
			if(peakdata.peakaddr == tvar1)
				fp_pkSRC.lv = tvar2;
			tvar2++;
		}
	cplSRCnames[tvar2] = 0x00;
	cplSRCindex[tvar2] = GHOST_BOX;

	fp_pkICH.lv = peakdata.imon - 1;
	fp_pkVHI.lv = peakdata.vmonhi - 1;
	fp_pkVLO.lv = peakdata.vmonlo - 1;

	vmonNames[0] = "A";
	vmonNames[1] = "B";
	vmonNames[2] = "C";
	vmonNames[3] = "D";
	vmonNames[4] = "E";
	vmonNames[5] = 0x00;

	listCPLnames[0] = "IMON1";
	listCPLnames[1] = "IMON2";
	listCPLnames[2] = "IMON3";
	listCPLnames[3] = "IMON4";
	listCPLnames[4] = "IMON5";
	listCPLnames[5] = 0x00;

	if(acdata.acbox != GHOST_BOX)
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
		if((fp_rmsMAX.maxv = moddata[acdata.acbox].modinfo.calinfo.data[CAL_RMSMAX_OFF]) <= 0)
			fp_rmsMAX.maxv = MAX_AC_PEAK;
		fp_rmsMAX.minus = 0;
	}

	LCD_gotoxy(0,0);
	LCD_puts("     MEASUREMENT CONFIGURATION\n\r\n\r");
	LCD_puts("Measurement Source:\n\r");
	LCD_puts("Voltage Monitor   :   vs  \n\r");
	LCD_puts("Current Monitor   :\n\r\n\r");

	if(acdata.acbox == GHOST_BOX)
		LCD_puts("Select the source for the peak\n\rmeasurements");
	else
	{
		LCD_puts("Peak Max:      A\n\r");
		LCD_puts("RMS  Max:      A    RMS  Min:      A");
	}
	init_list_field(FLD_PKSRCx,FLD_PKSRCy,1,&fp_pkSRC,PK_SRCchanged,0x00,cplSRCnames,1);
	init_list_field(FLD_PKVHIx,FLD_PKVHIy,0,&fp_pkVHI,PK_VCHchanged,0x00,vmonNames,1);
	init_list_field(FLD_PKVLOx,FLD_PKVLOy,0,&fp_pkVLO,PK_VCHchanged,0x00,vmonNames,1);
	init_list_field(FLD_PKICHx,FLD_PKICHy,1,&fp_pkICH,PK_ICHchanged,0x00,listCPLnames,1);

	if(acdata.acbox != GHOST_BOX)
	{
		init_numeric_field(FLD_RMSPKLx,FLD_RMSPKLy,5,&fp_rmsPKL,RMS_PKLchanged,0x00,1);
		init_numeric_field(FLD_RMSMAXx,FLD_RMSMAXy,5,&fp_rmsMAX,RMS_MAXchanged,0x00,1);
		init_numeric_field(FLD_RMSMINx,FLD_RMSMINy,5,&fp_rmsMIN,RMS_MINchanged,0x00,1);
	}

	field_horizontal(fp_pkVLO.field,fp_pkVHI.field,fp_pkVHI.field);
	field_horizontal(fp_pkVHI.field,fp_pkVLO.field,fp_pkVLO.field);
	field_vertical(fp_pkVHI.field,fp_pkSRC.field,fp_pkICH.field);
	field_vertical(fp_pkVLO.field,fp_pkSRC.field,fp_pkICH.field);

	if(acdata.acbox == GHOST_BOX)
	{
		field_vertical(fp_pkSRC.field,fp_pkICH.field,fp_pkVHI.field);
		field_vertical(fp_pkICH.field,fp_pkVHI.field,fp_pkSRC.field);
	}
	else
	{
		field_vertical(fp_pkSRC.field, fp_rmsMAX.field,fp_pkVHI.field);
		field_vertical(fp_pkICH.field, fp_pkVHI.field, fp_rmsPKL.field);
		field_vertical(fp_rmsPKL.field,fp_pkICH.field, fp_rmsMAX.field);
		field_vertical(fp_rmsMAX.field,fp_rmsPKL.field,fp_pkSRC.field);
		field_horizontal(fp_rmsMAX.field,fp_rmsMIN.field,fp_rmsMIN.field);
		field_horizontal(fp_rmsMIN.field,fp_rmsMAX.field,fp_rmsMAX.field);
	}

	PK_SRCchanged();
	PK_ICHchanged();
	PK_VCHchanged();

	LCD_gotoxy(FLD_PKSRCx,FLD_PKSRCy);
	LCD_puts(cplSRCnames[fp_pkSRC.lv]);
	LCD_gotoxy(FLD_PKICHx,FLD_PKICHy);
	LCD_puts(listCPLnames[fp_pkICH.lv]);
	LCD_gotoxy(FLD_PKVHIx,FLD_PKVHIy);
	LCD_puts(vmonNames[fp_pkVHI.lv]);
	LCD_gotoxy(FLD_PKVLOx,FLD_PKVLOy);
	LCD_puts(vmonNames[fp_pkVLO.lv]);

	peakdata.menuset = 1;
	func_manager = meas_funcs;
}

void	CAL_NETchanged()
{
	/* CHECK for ERRORS */

	/* RESET WAVEFORM to ZERO (FIRST WAVE) */
	fp_calWAV.lv= 0;
	fp_calK5D.f = (moddata[listSRGindex[fp_calNET.lv]].modinfo.calinfo.data[0] * 4) - 49000;
	fp_calK5D.f = (short)(((float)(fp_calK5D.f) + (float)(0.4)) * (float)(0.0103032));

	SRG_SetWaveNames(listSRGindex[fp_calNET.lv]);

	(*field_list[fp_calWAV.field].left)(&field_list[fp_calWAV.field]);
	(*field_list[fp_calK5D.field].left)(&field_list[fp_calK5D.field]);

	CAL_WAVchanged();
}

void	CAL_WAVchanged()
{
	fp_calVFP.f = moddata[listSRGindex[fp_calNET.lv]].modinfo.calinfo.data[WAVE_OFFSET+(FP_LINE_SIZE*fp_calWAV.lv)+FP_OFFSET];
	fp_calCPL.f = moddata[listSRGindex[fp_calNET.lv]].modinfo.calinfo.data[WAVE_OFFSET+(FP_LINE_SIZE*fp_calWAV.lv)+LINE_OFFSET];
	fp_calIO1.f = moddata[listSRGindex[fp_calNET.lv]].modinfo.calinfo.data[(DATA_OFFSET+(DATA_SIZE*fp_calWAV.lv)+0)];
	(*field_list[fp_calVFP.field].left)(&field_list[fp_calVFP.field]);
	(*field_list[fp_calCPL.field].left)(&field_list[fp_calCPL.field]);
	(*field_list[fp_calIO1.field].left)(&field_list[fp_calIO1.field]);
}

void	CAL_DUMchanged()
{
	/* Not needed?? */
}

void	CAL_WRTE9346()
{
	ushort	k5delay;
	int		ox,oy;

	/* K5DELAY --> EARLY/SUB -- LATE/ADD */
	if(fp_calK5D.f < 0)
		k5delay = (49000 + (short)(((float)(fp_calK5D.f) - (float)(0.4)) / (float)(0.0103032))) / 4;
	else
		k5delay = (49000 + (short)(((float)(fp_calK5D.f) + (float)(0.4)) / (float)(0.0103032))) / 4;

	moddata[listSRGindex[fp_calNET.lv]].modinfo.calinfo.data[0] = k5delay;
	moddata[listSRGindex[fp_calNET.lv]].modinfo.calinfo.data[(WAVE_OFFSET+(FP_LINE_SIZE*fp_calWAV.lv)+FP_OFFSET)]		= fp_calVFP.f;
	moddata[listSRGindex[fp_calNET.lv]].modinfo.calinfo.data[(WAVE_OFFSET+(FP_LINE_SIZE*fp_calWAV.lv)+LINE_OFFSET)]	= fp_calCPL.f;
	moddata[listSRGindex[fp_calNET.lv]].modinfo.calinfo.data[(DATA_OFFSET+(DATA_SIZE*fp_calWAV.lv)+0)]	= fp_calIO1.f;
	paraml[0].idata = listSRGindex[fp_calNET.lv];

	ox = LCD_getx();
	oy = LCD_gety();
	LCD_gotoxy(0,7);
	LCD_puts("Writing EEPROM NOW...  ");
	if(BAY_WRTE_pgm(paraml))
		KEY_beep();
	TPU_delay(250);
	LCD_gotoxy(0,7);
	LCD_puts("WRITE _____ _____ _____");
	LCD_gotoxy(ox,oy);
}

void	show_state_calsrg(void)
{
	uint	tvar1,tvar2;

/*
 *	Set up array holdings for network modules.  The index into the
 *	array is same as the network id (no translation is needed).
 */
	for(tvar1 = 0, tvar2 = 0; tvar1 < MAX_MODULES; tvar1++)
		if(moddata[tvar1].modinfo.typePrimary & TYPE_SRG)
		{
		 	listSRGnames[tvar2] = moddata[tvar1].modinfo.name;
			listSRGindex[tvar2] = tvar1;
			tvar2++;
		}
	listSRGnames[tvar2] = 0x00;
	listSRGindex[tvar2] = GHOST_BOX;

	fp_calNET.lv = 0;
	fp_calWAV.lv = 0;

	fp_calK5D.f		= (moddata[listSRGindex[fp_calNET.lv]].modinfo.calinfo.data[0] * 4) - 49000;
	fp_calK5D.f		= (short)(((float)(fp_calK5D.f) + (float)(0.4)) * (float)(0.0103032));
	fp_calK5D.minv	= 0;
	fp_calK5D.maxv	= 360;
	fp_calK5D.minus= 1;

	fp_calVFP.f		= moddata[listSRGindex[fp_calNET.lv]].modinfo.calinfo.data[WAVE_OFFSET+(FP_LINE_SIZE*fp_calWAV.lv)+FP_OFFSET];
	fp_calVFP.minv	= 0;
	fp_calVFP.maxv	= 15000;
	fp_calVFP.minus= 0;

	fp_calCPL.f		= moddata[listSRGindex[fp_calNET.lv]].modinfo.calinfo.data[WAVE_OFFSET+(FP_LINE_SIZE*fp_calWAV.lv)+LINE_OFFSET];
	fp_calCPL.minv	= 0;
	fp_calCPL.maxv	= 15000;
	fp_calCPL.minus= 0;

	fp_calIO1.f		= moddata[listSRGindex[fp_calNET.lv]].modinfo.calinfo.data[(DATA_OFFSET+(DATA_SIZE*fp_calWAV.lv)+0)];
	fp_calIO1.minv	= 0;
	fp_calIO1.maxv	= 15000;
	fp_calIO1.minus= 0;

	SRG_SetWaveNames(listSRGindex[fp_calNET.lv]);

	LCD_gotoxy(0,0);
	LCD_puts("SURGE  CALIBRATION\n\r");
	LCD_puts("Network     :       -\n\r");
	LCD_puts("Front Panel :      V  K5 Delay  :      d");
	LCD_puts("LINE Coupler:      V\n\r");
	LCD_puts("DATA Coupler:      V\n\r\n\r\n\r");
	LCD_puts("WRITE _____ _____ _____ ____ _____ _____");

	init_list_field(FLD_CALNETx,FLD_CALNETy,1,&fp_calNET,CAL_NETchanged,0x00,listSRGnames,1);
	init_list_field(FLD_CALWAVx,FLD_CALWAVy,1,&fp_calWAV,CAL_WAVchanged,0x00,listWAVnames,1);
	init_numeric_field(FLD_CALK5Dx,FLD_CALK5Dy,4,&fp_calK5D,CAL_DUMchanged,0x00,1);
	init_numeric_field(FLD_CALVFPx,FLD_CALVFPy,5,&fp_calVFP,CAL_DUMchanged,0x00,1);
	init_numeric_field(FLD_CALCPLx,FLD_CALCPLy,5,&fp_calCPL,CAL_DUMchanged,0x00,1);
	init_numeric_field(FLD_CALIO1x,FLD_CALIO1y,5,&fp_calIO1,CAL_DUMchanged,0x00,1);

	field_vertical(fp_calNET.field,fp_calIO1.field,fp_calVFP.field);
	field_vertical(fp_calVFP.field,fp_calNET.field,fp_calCPL.field);
 	field_vertical(fp_calCPL.field,fp_calVFP.field,fp_calIO1.field);
	field_vertical(fp_calIO1.field,fp_calCPL.field,fp_calNET.field);

	field_horizontal(fp_calNET.field,fp_calWAV.field,fp_calWAV.field);
	field_horizontal(fp_calWAV.field,fp_calNET.field,fp_calNET.field);
	field_horizontal(fp_calVFP.field,fp_calK5D.field,fp_calK5D.field);
	field_horizontal(fp_calK5D.field,fp_calVFP.field,fp_calVFP.field);

	tvar1 = 0;
	for(tvar2 = 0; tvar2 < MAX_MODULES; tvar2++)
		if(moddata[tvar2].modinfo.typePrimary & TYPE_SRG)
			tvar1++;
	if(tvar1 <= 1)
		field_list[fp_srgNET.field].valid = 0;

	CAL_WAVchanged();

	func_manager = cal_funcs;
}

#define CSBARBT_ADDR        0xFFFFFA48
#define CSBARBT_RW_BITMASK  0x0003   // 2-bit field
#define CSBARBT_RW_OFFSET   12        // bits 12-11
#define CSBARBT_RW_READWRITE 0x11   // 'read and write' setting
#define ROM_BASE            0x80000
#define COUNTS_PER_MILLISECOND  349 // empirically determined value; assumes empty 'for' loop, dependent upon CPU clock frequency, etc.
#define CRUDE_DELAY( delay_in_milliseconds )    {                                       \
    int count;                                                                          \
    for( count = 0; count < (delay_in_milliseconds * COUNTS_PER_MILLISECOND); count++ ) \
        ;                                                                               \
    }
#define BITMASK( value, mask, offset ) ( ( value >> offset ) & mask )

void read_eprom_id( ushort *manufacturer_id, ushort *device_id )
{
    int count;
    uint csbarbt_readwrite;
    uint csbarbt_original;

    //DUART_disable();
    INT_disable();
    // Disable ALL interrupts
    asm(" move.w #$2700,SR");

    /*
        This section is for reading Flash (not EPROM) ID codes
    */
    /* Change ROM chip select option to Read/Write (CSBARBT bits 12-11 to 11b) */
    csbarbt_original = (*(uint *)CSBARBT_ADDR);
    csbarbt_readwrite = csbarbt_original | (CSBARBT_RW_READWRITE && CSBARBT_RW_BITMASK) << CSBARBT_RW_OFFSET;
    (*(uint *)CSBARBT_ADDR) = csbarbt_readwrite;

    /* Enter Software ID mode */
    (*(ushort *)(ROM_BASE + (0x5555 * sizeof(ushort)))) = 0xAAAA;
    (*(ushort *)(ROM_BASE + (0x2AAA * sizeof(ushort)))) = 0x5555;
    (*(ushort *)(ROM_BASE + (0x5555 * sizeof(ushort)))) = 0x9090;
    /* Read manufacturer's ID */
    *manufacturer_id = (*(ushort *)ROM_BASE);
    /* Read device ID */
    *device_id = (*(ushort *)(ROM_BASE + (1 * sizeof(ushort))));
    /* Exit Software ID mode */
    (*(ushort *)(ROM_BASE + (0x5555 * sizeof(ushort)))) = 0xAAAA;
    (*(ushort *)(ROM_BASE + (0x2AAA * sizeof(ushort)))) = 0x5555;
    (*(ushort *)(ROM_BASE + (0x5555 * sizeof(ushort)))) = 0xF0F0;

    /* Restore ROM chip select option */
    (*(uint *)CSBARBT_ADDR) = csbarbt_original;

    DUART_enable();
    // Reenable interrupts
    INT_enable();

    return;

    /*
        This section is for reading EPROM (not Flash) ID codes
    */
    //DUART_disable();
    INT_disable();
    // Disable ALL interrupts
    asm(" move.w #$2700,SR");

    /* Set Program (Vpp) level on EPROM VPP pin (wait at least 1 ms for level change) */
    FLSH_Write();
    CRUDE_DELAY( 5 );

    /* Change ROM chip select option to Read/Write (CSBARBT bits 12-11 to 11b) */
    csbarbt_original = (*(uint *)CSBARBT_ADDR);
    csbarbt_readwrite = csbarbt_original | (CSBARBT_RW_READWRITE && CSBARBT_RW_BITMASK) << CSBARBT_RW_OFFSET;
    (*(uint *)CSBARBT_ADDR) = csbarbt_readwrite;
    
    /* Read manufacturer's ID */
    (*(ushort *)ROM_BASE) = 0x9090;
    *manufacturer_id = (*(ushort *)ROM_BASE);

    /* Read device ID */
    (*(ushort *)(ROM_BASE + 1)) = 0x9090;
    *device_id = (*(ushort *)(ROM_BASE + (1 * sizeof(ushort))));

    /* Restore ROM chip select option */
    (*(uint *)CSBARBT_ADDR) = csbarbt_original;
    
    /* Set Normal (Vdd) level on EPROM VPP pin (wait at least 1 ms for level change) */
    FLSH_Reset();
    CRUDE_DELAY( 5 );

    DUART_enable();
    // Reenable interrupts
    INT_enable();

    return;
}

void eprom_test()
{
    void (*read_eprom_id_in_ram)( ushort *, ushort * );
    int read_eprom_id_in_ram_size;
    char device_string[80];
    static ushort manufacturer_id = 0;
    static ushort device_id = 0;
    uchar single_mfr_id;
    uchar single_dev_id;
    int ox, oy;
    int index;
    char *source_ptr = (char *)read_eprom_id;
    char *destination_ptr;
    struct rom_id_struct {
        ushort manufacturer_id;
        char manufacturer_string[20];
        ushort device_id;
        char device_string[20];
    } rom_id[] = {
        { 0x31, "Catalyst",     0xB4,   "CAT28F010"     },
        { 0xBF, "Greenliant",   0xA5,   "GLS27SF010"    },
        { 0xBF, "SST",          0xB5,   "SST39SF010A"   },
        { 0xBF, "SST",          0xB7,   "SST39SF040"   }
    };
    int rom_id_size = sizeof( rom_id ) / sizeof( struct rom_id_struct );

    /* first try reading device IDs from Catalyst 28F010; expect MFG 31, Device B4 */

    /* copy read function to RAM and execute there */
    read_eprom_id_in_ram_size = (char *)eprom_test - (char *)read_eprom_id + 2;  // add 2 instead of 1 because memory is two bytes wide

    if ( ( read_eprom_id_in_ram = (void *)malloc( read_eprom_id_in_ram_size ) ) == NULL )
    {
        ox = LCD_getx();
        oy = LCD_gety();
        LCD_gotoxy( 0, 6 );
        fprintf( DISPLAY, "malloc error!" );
        LCD_gotoxy( ox, oy );
        return;
    }
    destination_ptr = (char *)read_eprom_id_in_ram;
    for( index = read_eprom_id_in_ram_size; index > 0; index-- )
        *destination_ptr++ = *source_ptr++;

    read_eprom_id_in_ram( &manufacturer_id, &device_id );

    /* The manufacturer and device IDs each consist of two single-byte codes, one for each ROM device.
       First check that both bytes are the same for both IDs, and print error and ID numbers if not.
       If the same manufacturer and device for both, then print names if known. */
    single_mfr_id = (uchar)BITMASK(manufacturer_id, 0xFF, 0);
    single_dev_id = (uchar)BITMASK(device_id, 0xFF, 0);

    if ( ( single_mfr_id != (uchar)BITMASK(manufacturer_id, 0xFF, 8) )
      || ( single_dev_id != (uchar)BITMASK(device_id, 0xFF, 8) ) )
    {
        sprintf( device_string, "Mismatched ROMs! (%x/%x & %x/%x)",
                 single_mfr_id, (uchar)BITMASK(manufacturer_id, 0xFF, 8),
                 single_dev_id, (uchar)BITMASK(device_id, 0xFF, 8) );
    }
    else
    {
        for( index = 0; index < rom_id_size; index++ )
        {
            if ( ( single_mfr_id == rom_id[index].manufacturer_id )
              && ( single_dev_id == rom_id[index].device_id ) )
            {
                sprintf( device_string, "ROM Mfr: %s, Dev: %s",
                         rom_id[index].manufacturer_string, rom_id[index].device_string );
                break;
            }
        }
        if ( index >= rom_id_size )
        {
            sprintf( device_string, "Unknown ROM devices, Mfr: %x, Dev: %x",
                     single_mfr_id, single_dev_id );
        }
    }

    // Display device string for 3 seconds, then clear
    ox = LCD_getx();
    oy = LCD_gety();
    LCD_gotoxy( 0, 6 );
    fprintf( DISPLAY, "%s", device_string );
    TPU_delay( 3000 );
    LCD_gotoxy( 0, 6 );
    for( index = 0; index <= 39; index++ )
        LCD_putc( ' ' );
    LCD_gotoxy( ox, oy );

    /* free space allocated for function in RAM */
    free( read_eprom_id_in_ram );

    return;
}

uchar	main_funcs(
char	c)
{
	switch(c)
	{
		case 'G':	/* Process start	*/
		case 'X':	/* Process stop	*/
			break;
		case 'a':	/* F1	- SRG Testing		*/
			display.request = DISPLAY_SRG;
			break;
		case 'b':	/* F2	*/
			if(calibration)
				display.request = DISPLAY_CALSRG;
			break;
		case 'c':	/* F3	- EFT/B Testing	*/
			display.request = DISPLAY_EFT;
			break;
		case 'd':	/* F4	*/
			if(calibration)
				display.request = DISPLAY_CALEFT;
			break;
		case 'e':	/* F5	- ESD Testing		*/
			display.request = DISPLAY_ESD;
			break;
        case 'f':	/* F6 */
#ifdef BETA_RELEASE
			if(calibration)
                eprom_test();
#endif
			break;
		case 'g':	/* F7	- PQF Testing		*/
			display.request = DISPLAY_PQF;
			break;
		default:
			break;
	}
	return(0);
}

uchar	srg_funcs(
char	c)
{
	if(display.current != DISPLAY_SRG)
		return(0);
	switch(c)
	{
		case 'G':	/* STRT	- Start CHARGE cycle		*/
			break;
		case 'a':	/* FUN1	- Start CHARGE cycle		*/
			if(chgdata.flag == CHG_IDLE)
				SRG_StartSystem();
			else
				if(chgdata.flag != CHG_DELAY)
					SRG_StopSystem();
			break;
		case 'X':	/* STOP	- KILL Surge operation	*/
			SRG_StopSystem();
			break;
		case 'c':	/* FUN3	- Trigger the SURGE!!!	*/
			if(chgdata.flag == CHG_READY)
				SRG_SurgeSystem();
			else
				SRG_StopSystem();
			break;
		case 'M':	/* MENU	- Main Menu	*/
			if(SRG_CPLCheck(current_field))
				KEY_beep();
			else
				display.request = DISPLAY_LOCAL;
			break;
		case 'g':	/* F7	- Measurement	*/
			chgdata.time = 0;
			SRG_StopSystem();
			if(SRG_CPLCheck(current_field))
				KEY_beep();
			else
				display.request = DISPLAY_MEASURE;
			break;
		default:
			break;
	}
	return(0);
}

uchar	meas_funcs(
char	c)
{
	switch(c)
	{
		case 'M':	/* MENU - Surge Menu	*/
		  	switch(ecatmode)
			{
				case SRG_MODE:
				   display.request = DISPLAY_SRG;
					break;
			 	case EFT_MODE:
					display.request = DISPLAY_EFT;
					break;
				default:
					display.request = DISPLAY_LOCAL;
					break;
			}
			break;
		default:
			break;
	}
	return(0);
}

uchar	cal_funcs(
char	c)
{
	ushort	tvar;

	switch(c)
	{
		case 'a':	/* F1	- Write to EEPROM */
			CAL_WRTE9346();
			break;
		case 'M':	/* MENU - Main Menu	*/
			display.request = DISPLAY_LOCAL;
			break;
		default:
			break;
	}
	return(0);
}

uchar	none_funcs(
char	c)
{
	switch(c)
	{
		case 'M':	/* MENU - Main Menu	*/
			display.request = noneTrack;
			break;
		default:
			break;
	}
	return(0);
}

void	SRG_LSM_textset()
{
	listLSMmodes[2] = "L2  ";
	listLSMmodes[3] = "L3  ";
	listLSMmodes[4] = 0x00;

	if(listOUTindex[fp_srgOUT.lv] != GHOST_BOX)
	{
		if((moddata[listOUTindex[fp_srgOUT.lv]].modinfo.typePrimary & TYPE_CPLSRG) &&
			!(moddata[listOUTindex[fp_srgOUT.lv]].modinfo.typePrimary & TYPE_3PHASE))
				listLSMmodes[2] = 0x00;
	}
}
/******* << END FP_SRG.C >> *******/

/**************************************************************************
// file: kt_hide.cpp
//
//
// Purpose: a Keytek hidden Win 3.0 communications window.
				Contains the ECAT command subroutines;   Most are sent in the
				window proc., with messages defined in KT_COMM.H
				and sent via WM_COMMAND.

	History: Starts with 4.00a version
		8/4/95   JFL   Line sync command fixes:
							Take out duplicate sends in EFT_INIT.
							Initialize eft_couple_raw.
		25 Aug 95  JEH Send the :LI Line Mode cmd only after COUPLE_SETSTATE.
		13 Sep 95  JFL Send the :LI:MO cmd in a new KT_SRG_PHASE_MODE.
							Call it in KT_SRG_RESETALL.
		20 Sep 95  JFL Set coupler to front panel properly in KT_SRG_RESETALL. 
		17 Oct 95  JFL Conditionalize previous change:  only if SetOutput=0
     10 Oct 95  JLR send measurement commands in KT_SRG_OUTPUT_SETSTATE
							if "auto" selected and output changed to FRONT.
     10 Oct 95  JLR In Set_Auto_Vmeas() and Set_Auto_Imeas() single phase 
		 					auto - only if AC coupler selected for measure.

**************************************************************************/
#include "stdafx.h"
#include "globals.h"
#include "kt_comm.h"
#include "kt_util.h"
#include "KT_HIDE.h"
#include "comm.h"
#include "message.h"
#include "801BITS.H"
#include "MODULE.h"
#include  <math.h>
#include "EcatApp.h"

#ifdef EXTERN
#undef EXTERN
#endif
#define EXTERN

const  int ILOCK_ECAT_BIT      = 1;
const  int POWER_UP_BIT        = 2;
const  int EFT_SETUP_BIT       = 4;
const  int ERR_AC_POWER_BIT    = 8;
const  int ERR_AC_LIMIT_BIT    = 16;
const  int BACK_FROM_LOCAL_BIT = 32;


#define device 0
EXTERN void PURGE(void);
EXTERN LPSTR Get_imeas_name(int i,int j);

// globals
static BOOL		EUT_Power;		// on, or off.
static WORD		SetNetwork;		//Network #
static WORD		SetOutput;		//Output location #
static WORD		SetWave;		//Waveform #
static float	SetPhase;		//Phase Angle
static int		PhaseMode;
static float	SetVoltage;		//Voltage
static long		couple_raw;
static WORD		vmeas_bay;
static long		vmeas_state;
static long		eft_couple_raw = 0;
static float	EftSetVoltage;
static float	EftFreq;
static float	EftDur;
static float	EftPer;
static BOOL		EftChirp;
static BOOL		EftRunning;
static WORD		EftDurUnits;

void PqfShowUploadDialog(BOOL bShow);

void Set_Auto_Vmeas(void);
void Set_Auto_Imeas(void);
void EFT_log_on_normal();
void EFT_log_on_export();
void SRG_log_on_export(int vpp,int vpm, int ipp,int ipm);
void SRG_log_on_normal(int vpp,int vpm, int ipp,int ipm);

void EFT_log_on()
{
	if (Log.normal()) 
		EFT_log_on_normal();
	else       
		EFT_log_on_export();
}

void EFT_log_on_normal()
{
	//XX:xx.xx:BURST  -xxxxx  xxxxKhz xxxxxmS  xxxx   RND   Exxxx  Exxxx L1,L2,L3,NU,PE
	char buffer[200];

	Log.logprintf("Burst  %6d",(int)EftSetVoltage);
	Log.logtime=FALSE;
	if (EftChirp)
	{
		Log.logprintf("   Chirp           ");
	}     
	else
	{
		if (EftFreq>10000) Log.logprintf("  %5dKhz",(int)(EftFreq/1000.0));
		else               Log.logprintf("  %5dHz ",(int)EftFreq);
		if (EftDurUnits)   Log.logprintf(" %5dmS ", EftDurUnits);
		else               Log.logprintf(" %5dPls", (int)(EftDur));
	}

	if (EftPer<0) Log.logprintf("  %4d",(int)(300));
	else          Log.logprintf("  %4d",(int)(EftPer));

	if (PhaseMode==0)	Log.logprintf("   RND");
	else
	if (PhaseMode==4)	Log.logprintf("   DC ");
	else				Log.logprintf("   %3d",(int)SetPhase);

	Log.logprintf("  %5Fs",(LPSTR)calset[EFT_MODULE].name);

	if (eft_couple_raw>=0)
	{
		Log.logprintf("  %5Fs",(LPSTR)calset[EFT_COUPLER].name);
		buffer[0]=0;
		buffer[1]=0;
		int i=(int)((eft_couple_raw>>8)&255);
		if (i & BIT_L1)  strcat(buffer,",L1");
		if (i & BIT_L2)  strcat(buffer,",L2");
		if (i & BIT_L3)  strcat(buffer,",L3");
		if (i & BIT_N)   strcat(buffer,",N");
		if (i & BIT_GND) strcat(buffer,",PE");
		buffer[0]=' ';
		Log.logprintf("%-15Fs",(LPSTR)buffer);
	}
	else
		Log.logprintf("  %5Fs Out 1%9c",(LPSTR)calset[EFT_MODULE].name,' ');

	if (AC_OPTION)
	{
		Log.logprintf(" %5Fs     ",(LPSTR)calset[SURGE_COUPLER].name);
		for (int i = 0; i<6; i++)   
		{
			if (rms_data[i] < 0) break;
			Log.logprintf("%5.1f  ",(float)(rms_data[i]/10.));
		}
	}
	Log.logprintf("\n");

	Log.logtime=TRUE;
}

void EFT_log_on_export()
{
	//XX:xx.xx:BURST -xxxxV xxxxKhz xxxxmS xxxPls   E501    Normal EXXXX 1,2,3,N,P
	char buffer[200];
	int i;
	Log.logprintf("Burst\t  %5d",(int)EftSetVoltage);
	Log.logtime=FALSE;
	if (EftChirp)
	{
		Log.logprintf("%13Fs",(LPSTR)" \tChirp\t\t    ");
	}
	else
	{
		if (EftFreq>10000) 
			Log.logprintf("\t%4d\tKhz",(int)(EftFreq/1000.0));
		else 
			Log.logprintf("\t%4d\tHz ",(int)EftFreq);

		if (EftDurUnits) 
			Log.logprintf(" \t%3d\tmS",EftDurUnits);
		else 
			Log.logprintf(" \t%3d\tPls",(int)(EftDur));
	}

	if (EftPer<0) 
		Log.logprintf(" \t%4d",(int)(300));
	else 
		Log.logprintf(" \t%4d",(int)(EftPer));

	if (PhaseMode==0) 
		Log.logprintf(" \tRND ");
	else if (PhaseMode==4) 
		Log.logprintf(" \tDC  ");
	else 
		Log.logprintf(" \t%3d ",(int)SetPhase);

	Log.logprintf(" \t%5Fs",(LPSTR)calset[EFT_MODULE].name);

	if (eft_couple_raw>=0)
	{
		Log.logprintf(" \t%5Fs",(LPSTR)calset[EFT_COUPLER].name);
		buffer[0]=0;
		buffer[1]=0;
		i=(int)((eft_couple_raw>>8)&255);
		if (i & BIT_L1)  strcat(buffer,",L1");
		if (i & BIT_L2)  strcat(buffer,",L2");
		if (i & BIT_L3)  strcat(buffer,",L3");
		if (i & BIT_N)   strcat(buffer,",N");
		if (i & BIT_GND) strcat(buffer,",PE");
		buffer[0]=' ';
		Log.logprintf("\t%-12Fs",(LPSTR)buffer);
	}
	else
		Log.logprintf("\t %5Fs    \tOut 1",(LPSTR)calset[EFT_MODULE].name);

	if (AC_OPTION)
	{
		Log.logprintf(" \t%5Fs",(LPSTR)calset[SURGE_COUPLER].name);

		for (int i = 0; i<6;i++)   
		{
			if (rms_data[i] < 0) break;
			Log.logprintf(" \t%5.1f",(float)(rms_data[i]/10.));
		}
	}

	Log.logprintf("\n");
	Log.logtime=TRUE;
}

void SRG_log_on(int vpp,int vpm, int ipp,int ipm)
{
	if (Log.normal()) 
		SRG_log_on_normal(vpp,vpm,ipp,ipm);
	else       
		SRG_log_on_export(vpp,vpm,ipp,ipm);
}

void SRG_log_on_normal(int vpp,int vpm, int ipp,int ipm)
{
	char buffer[200];                     

	Log.logprintf( "Surge  %5d",(int)SetVoltage);
	Log.logtime=FALSE;
	if (PhaseMode==0) 
		Log.logprintf("  RND");
	else if (PhaseMode==4) 
		Log.logprintf("  DC ");
	else 
		Log.logprintf("  %3d",(int)SetPhase);

	int i=0;
	while ((i<30) && (!((wave_list[i].mod_num ==SetNetwork) && (wave_list[i].waven==SetWave)))) i++;

	Log.logprintf("   %5Fs",(LPSTR)calset[SetNetwork].name);
	Log.logprintf("   %-26Fs",(LPSTR)wave_list[i].wave_name);

	if (SetOutput<16) 
		Log.logprintf("%5Fs ",(LPSTR)calset[SetOutput].name);
	else 
		Log.logprintf("      ");

	if (couple_raw>0 && SetOutput<16)
	{
		buffer[0]=0;
		buffer[1]=0;
		i=(int)((couple_raw>>8)&255);
		if (couple_raw & BIT_DATACPL)
		{
			if (i & BIT_L1)  strcat(buffer,",S1");
			if (i & BIT_L2)  strcat(buffer,",S2");
			if (i & BIT_L3)  strcat(buffer,",S3");
			if (i & BIT_N)   strcat(buffer,",S4");
			if (i & BIT_GND) strcat(buffer,",G");
		}
		else
		{
			if (i & BIT_L1)  strcat(buffer,",L1");
			if (i & BIT_L2)  strcat(buffer,",L2");
			if (i & BIT_L3)  strcat(buffer,",L3");
			if (i & BIT_N)   strcat(buffer,",N");
			if (i & BIT_GND) strcat(buffer,",PE");
		}
		buffer[0]=' ';
		Log.logprintf(" %-12Fs",(LPSTR)buffer+1);

		buffer[0]=0;
		buffer[1]=0;
		i=(int)((couple_raw)&255);
		if (couple_raw & BIT_DATACPL)
		{
			if (i & BIT_L1)  strcat(buffer,",S1");
			if (i & BIT_L2)  strcat(buffer,",S2");
			if (i & BIT_L3)  strcat(buffer,",S3");
			if (i & BIT_N)   strcat(buffer,",S4");
			if (i & BIT_GND) strcat(buffer,",G");
		}
		else
		{
			if (i & BIT_L1)  strcat(buffer,",L1");
			if (i & BIT_L2)  strcat(buffer,",L2");
			if (i & BIT_L3)  strcat(buffer,",L3");
			if (i & BIT_N)   strcat(buffer,",N");
			if (i & BIT_GND) strcat(buffer,",PE");
		}
		buffer[0]=' ';
		Log.logprintf("%-4Fs",(LPSTR)buffer+1);

		if (couple_raw & BIT_DATACPL)
		{
			if (couple_raw & BIT_CMODEON)
				Log.logprintf("%-7Fs",(LPSTR)" Symm  ");
			else
				Log.logprintf("%-7Fs",(LPSTR)" Asymm ");
		}
		else
			Log.logprintf("%-7Fs",(LPSTR)"       ");
	}
	else
		Log.logprintf("%-24Fs",(LPSTR)" Front Panel");

	Log.logprintf("%5d %5d  %5d %5d ",vpp,vpm,ipp,ipm);

	if (AC_OPTION)
	{
		Log.logprintf("  %5Fs     ",(LPSTR)calset[SURGE_COUPLER].name);

		for (int i = 0; i<6;i++)   
		{
			if (rms_data[i] < 0) break;
			Log.logprintf("%5.1f  ",(float)(rms_data[i]/10.));
		}
	}

	Log.logprintf("\n");
	Log.logtime=TRUE;
}

													  
void SRG_log_on_export(int vpp,int vpm, int ipp,int ipm)
{
	char buffer[200];                     
	int i;
	//XX:xx.xx:Surge  -xxxxV xxx XE501 1.2/50,8/20 Comb Auto 2/12  EXXXX: 1,2,3,N  2,3,N,P  -xxxx -xxxx -xxxx -xxxx
	Log.logprintf( "Surge  \t%5d",(int)SetVoltage);
	Log.logtime=FALSE;
	if (PhaseMode==0) 
		Log.logprintf(" \tRND ");
	else if (PhaseMode==4) 
		Log.logprintf(" \tDC  ");
	else 
		Log.logprintf(" \t%3d ",(int)SetPhase);

	i=0;
	while ((i<30) && (!((wave_list[i].mod_num ==SetNetwork) && (wave_list[i].waven==SetWave)))) i++;

	Log.logprintf("\t%5Fs",(LPSTR)calset[SetNetwork].name);
	Log.logprintf(" \t%-26Fs",(LPSTR)wave_list[i].wave_name);

	if (SetOutput<16) 
		Log.logprintf(" \t%5Fs",(LPSTR)calset[SetOutput].name);
	else 
		Log.loglpstr("\t     ");

	if (couple_raw>0 && SetOutput<16)
	{
		buffer[0]=0;
		buffer[1]=0;
		i=(int)((couple_raw>>8)&255);
		if (i & BIT_L1)  strcat(buffer,",L1");
		if (i & BIT_L2)  strcat(buffer,",L2");
		if (i & BIT_L3)  strcat(buffer,",L3");
		if (i & BIT_N)   strcat(buffer,",N");
		if (i & BIT_GND) strcat(buffer,",PE");
		buffer[0]=' ';
		Log.logprintf("\t%-12Fs",(LPSTR)buffer+1);

		buffer[0]=0;
		buffer[1]=0;
		i=(int)((couple_raw)&255);
		if (i & BIT_L1)  strcat(buffer,",L1");
		if (i & BIT_L2)  strcat(buffer,",L2");
		if (i & BIT_L3)  strcat(buffer,",L3");
		if (i & BIT_N)   strcat(buffer,",N");
		if (i & BIT_GND) strcat(buffer,",PE");
		buffer[0]=' ';
		Log.logprintf("\t%-3Fs",(LPSTR)buffer+1);

		if (couple_raw & BIT_DATACPL)
		{
			if (couple_raw & BIT_CMODEON)
				Log.logprintf("\t%-5Fs",(LPSTR)"Symm");
			else
				Log.logprintf("\t%-5Fs",(LPSTR)"Asymm");
		}
		else
			Log.logprintf("\t%-5Fs",(LPSTR)" ");
	}
	else
		Log.logprintf("\t%-15Fs\t\t",(LPSTR)"Front Panel");

	Log.logprintf("  \t%5d \t%5d \t%5d \t%5d",vpp,vpm,ipp,ipm);

	if (AC_OPTION)
	{
		Log.logprintf(" \t%5Fs",(LPSTR)calset[SURGE_COUPLER].name);

		for (int i = 0; i<6;i++)   
		{
			if (rms_data[i] < 0) break;
			Log.logprintf(" \t%5.1f",(float)(rms_data[i]/10.));
		}
	}

	Log.logprintf("\n");
	Log.logtime=TRUE;
}
		
EXTERN static int gError;
char SPIB_is_busy();

/************************************************************************/
/*                                                                      */
/* Keytek Hidden Window Procedure                                                */
/*                                                                      */
/************************************************************************/
LONG CALLBACK KT_HiddenWndProc(HWND hHideWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int scanErr = 0;
	char szQueryBuffer[MAX_PATH + 1];
	memset(szQueryBuffer, 0, sizeof(szQueryBuffer));
	float* fplParam=(float *)(&lParam);
	int result= 0;

	#define gError 0
	char *pCurrentInfoToken;               // Current *IDN? string token.
	DWORD serial;                          // device serial number.

	switch (message)
	{
	 case WM_COMMAND:
		/* The Windows messages for action bar and pulldown menu items */
		/* are processed here.                                         */
		break;        /* End of WM_COMMAND                             */

	 case WM_CREATE:
		break;       /*  End of WM_CREATE                              */

	case KT_INITIALIZE:
		break;
		  
	case KT_DEVICE_INFO:
		{
			if (SIMULATION) return FALSE;
			pkt_device_info pDvcInfo = (pkt_device_info) lParam;    // pointer to device info structure.
			if (IsValidAddress((void*) pDvcInfo, sizeof(kt_device_info), TRUE))
			{
				char numbuf[8];
				PURGE();
				QueryECAT(device, "*idn?", szQueryBuffer, MAX_PATH);

				pCurrentInfoToken =  strtok( szQueryBuffer, ",");  // expect "Keytek Instrument..." first.
				if (!pCurrentInfoToken) return FALSE;
				_fstrncpy( pDvcInfo->company_name, pCurrentInfoToken,MAX_DI_STR_LEN);

				pCurrentInfoToken =  strtok( NULL, ",");  // expect the machine type next.
				if (!pCurrentInfoToken) return FALSE;
				_fstrncpy(pDvcInfo->machine_type,pCurrentInfoToken,MAX_DI_STR_LEN);

				pCurrentInfoToken =  strtok( NULL, ",");  // expect serial number next.
				if (!pCurrentInfoToken) return FALSE;
				serial = atol(pCurrentInfoToken);
				pDvcInfo->serial_number = serial;
				pCurrentInfoToken =  strtok( NULL, ",");  // expect dev. firmware revision last.
				if (!pCurrentInfoToken) return FALSE;
				_fstrncpy (pDvcInfo->firmware,pCurrentInfoToken,MAX_DI_STR_LEN);
				numbuf[0]=pDvcInfo->firmware[0];
				numbuf[1]=pDvcInfo->firmware[1];
				numbuf[2]='.';
				numbuf[3]=pDvcInfo->firmware[2];
				numbuf[4]=pDvcInfo->firmware[3];
				numbuf[5]=pDvcInfo->firmware[4];
				numbuf[6]=pDvcInfo->firmware[5];
				numbuf[7]=0;
				pDvcInfo->firm_rev=(float)atof(numbuf);


				pDvcInfo->firmware[7]=pDvcInfo->firmware[5];
				pDvcInfo->firmware[6]=pDvcInfo->firmware[4];
				pDvcInfo->firmware[4]=pDvcInfo->firmware[3];
				pDvcInfo->firmware[3]=pDvcInfo->firmware[2];
				pDvcInfo->firmware[2]='.';
				pDvcInfo->firmware[5]='.';
				pDvcInfo->firmware[8]=0;
				pDvcInfo->three_phase=FALSE;
				pDvcInfo->eightKV=FALSE;

				// Upfate global firmware ID
				_fstrncpy(g_strFwId,pDvcInfo->firmware,MAX_DI_STR_LEN);
				return TRUE;
			}
			else
			{
				DBGOUT("Invalid kt_device_info pointer when getting device info.\n");
				return FALSE;
			}
		}

	case KT_EUT_ON:
		EUT_Power=TRUE;
		PrintfECAT(device,"EUT 1"); 
		ECAT_sync();
		return (gError);

	case KT_EUT_OFF:
		EUT_Power=FALSE;
		PrintfECAT(device,"EUT 0"); 
		ECAT_sync();
		return (gError);

	case KT_EUT_CHEAT:
		EUT_Power=FALSE;
		QueryECAT(device,"SRG:CHEAT",szQueryBuffer,100); 
		if (_fstrstr(szQueryBuffer,"Switch Open")) 
			return -1;
		else 
			return 0;

	case KT_PHASE_MODE:
		PhaseMode = lParam;
		return (gError);

	case KT_PHASE_ANGLE_SETSTATE:
		SetPhase   = *fplParam;
		PrintfECAT(device,":LI:AN %d",(int)((float)*fplParam));
		ECAT_sync();
		return (gError);

	case KT_EFT_SET:
		QueryECAT(device, "EFT:SET", szQueryBuffer, 100, 17000/*6000*/);
		if (SIMULATION) return 0;
		scanErr = sscanf(szQueryBuffer, "%d", &result);
		DBGOUT("EFT:SET. result: %d\n", result);
		
		// If we can't translate the returned data, most likely it's the interlock problem
		if ((scanErr == 0) || (scanErr == EOF))
		{
			result = ILOCK_EFT_INTERLOCK - ILOCK_EFT_BASE;
			DBGOUT("EFT:SET. forcing interlock error.\n");
		}
		return (result);

	case KT_EFT_COUPLE_SETSTATE:
		eft_couple_raw=lParam;
		if (lParam>0)
		{
			PrintfECAT(device,"EFT:OU 1 %d",EFT_COUPLER);
			ECAT_sync();
			PrintfECAT(device,"EFT:CO  %ld",lParam/512 );
			ECAT_sync();
			/////  Send Line Mode here  25 Aug 95  JEH
			PrintfECAT(device,":LI:MO %d", PhaseMode);
			ECAT_sync();
		}
		else
		{
			PrintfECAT(device,"EFT:OU 0 0");
			ECAT_sync();
		}
		return (gError);      

	case KT_EFT_INIT:
		PURGE();
		if (EFT_COUPLER >=0)
		{
			PrintfECAT(device,"CO %d",EFT_COUPLER);
			ECAT_sync();
			PrintfECAT(device,"EFT:OU 1 %d",EFT_COUPLER);
			ECAT_sync();
		}

		PrintfECAT(device,"EFT:BAY %d",EFT_MODULE);
		ECAT_sync();
		PrintfECAT(device,"EFT:MO 1");
		ECAT_sync();
		break;

	case KT_EFT_VOLTAGE_SETSTATE:
		PrintfECAT(device,"EFT:VOLT %d",(int)((float)*fplParam));
		ECAT_sync();
		EftSetVoltage = *fplParam;
		return (gError);

	case KT_EFT_FREQUENCY_SETSTATE  :
		PrintfECAT(device,"EFT:FR %ld",(DWORD)((float)*fplParam));
		ECAT_sync();
		EftFreq=*fplParam;
		return (gError);

	case KT_EFT_DURATION_SETSTATE   :
		EftDurUnits=wParam;
		PrintfECAT(device,"EFT:DU %d",(int)*fplParam);
		ECAT_sync();
		EftDur=*fplParam;
		return (gError);

	case KT_EFT_PERIOD_SETSTATE     :
		if (((float)*fplParam)<0.0)
		{ 
			PrintfECAT(device,"EFT:PER 300");
		}
		else
		{
			PrintfECAT(device,"EFT:PER %d",(int)*fplParam);
		}
		ECAT_sync();
		EftPer=*fplParam;
		return (gError);

	case KT_EFT_ON:
		QueryECAT(device,"*TRG 2",szQueryBuffer,100,2500);
		EFT_log_on();
		EftRunning=TRUE;
		if (SIMULATION) return 0;
		sscanf(szQueryBuffer,"%d ",&result);
		return (result);

	 case KT_EFT_OFF:
		if (EftRunning) 
			Log.loglpstr("EFT Step Ended\n");
		PrintfECAT(device,"ABORT");
		ECAT_sync();
		EftRunning=FALSE;
		return (gError);

	case KT_EFT_CHIRP:
		EftChirp=wParam;
		if (wParam) PrintfECAT(device,"EFT:WAVE 1");
		else        PrintfECAT(device,"EFT:WAVE 0");
		ECAT_sync();
		return (gError);

	case KT_SRG_INIT:
		vmeas_bay=0xFF;
		SetNetwork=0;
		SetOutput=0;
		SetWave=0;  
 
	case KT_SRG_LOCAL_INIT:
		if (SURGE_COUPLER>=0)
		{ 
			PrintfECAT(device,"CO %d",SURGE_COUPLER);
			ECAT_sync();
			PrintfECAT(device,"CO %d",SURGE_COUPLER);
			ECAT_sync();
            if ( calset[SURGE_COUPLER].id == E505B_BOXT )
            {
	        	PrintfECAT(device,"SRG:NETWORK %d",SURGE_COUPLER);
			    ECAT_sync();
    		    SetNetwork = SURGE_COUPLER;
                int default_waveform = 0;  // this is the coupled waveform (the other two are front panel only)
		        PrintfECAT(device,"SRG:WAVE %d",default_waveform);
		        ECAT_sync();
		        SetWave = default_waveform;
            }
		}
		return 0;

	case KT_SRG_COUPLE_SETSTATE:
		couple_raw=lParam;
		if (lParam < 0) 
			break;
		lParam &= ~BIT_DATACPL;
		lParam &= ~BIT_CMODEON;
		PrintfECAT(device,"SRG:CO %d %d",(int)((lParam/512)& 255),(int)(lParam &255)/2);
		ECAT_sync();
		if (LOWORD(vmeas_state)==0x00FF) Set_Auto_Imeas();
		if (HIWORD(vmeas_state)==0x00FF) Set_Auto_Vmeas();
		return (gError);

	case KT_SRG_PHASE_MODE:
		// Added by JFL 9/13/95:  simplest way to make sure SurgeWare isn't
		// affected by BurstWare change removing :LI:MO command -- duplicate
		// the routine here.  Don't want to miss any cases coming out of
		// PHASE class Refresh or Update.
		PhaseMode = lParam;
		PrintfECAT(device,":LI:MO %d", PhaseMode);
		ECAT_sync();
		return (gError);

	case KT_SRG_OUTPUT_SETSTATE:
		if (SURGE_COUPLER>=0)
		{ 
			PrintfECAT(device,"CO %d",SURGE_COUPLER);
			ECAT_sync();
		}
		PrintfECAT(device,"SRG:OUTPUT %d",wParam);

		// JLR, 10/17/95 code below added to send measurement commands to ECAT
		//					if "auto" selected and output changed to FRONT
		if ((wParam == 0xFF) && (SetOutput != wParam)) 	// JLR, 10/17/95
		{
			couple_raw=0;

			if (LOWORD(vmeas_state)==0x00FF) Set_Auto_Imeas();
			if (HIWORD(vmeas_state)==0x00FF) Set_Auto_Vmeas();
		}
		SetOutput = wParam;
		ECAT_sync();
		return (gError);


	case KT_SRG_OUTPUT_QUERY:
		if (SIMULATION) return SetOutput;
		QueryECAT(device, "SRG:OUT?", szQueryBuffer, 99);
		sscanf(szQueryBuffer, "%d", &result);
		return(result);

	case KT_CLAMP_STATE:
		PrintfECAT(device,"DATA:CL %d",wParam);
		ECAT_sync();
		return (gError);

	case KT_CLAMP_QUERY:
		QueryECAT(device,"DATA:CL?",szQueryBuffer,99);
		sscanf(szQueryBuffer,"%d",&result);
		return(result);

	case KT_DATA_ON:       
		PrintfECAT(device,"DATA:OU %d",wParam);
		ECAT_sync();
		return (gError);

	case KT_DATA_ON_QUERY:
		QueryECAT(device,"DATA:OU?",szQueryBuffer,99);
		sscanf(szQueryBuffer,"%d",&result);
		return(result);

	case KT_CMODE_STATE:   
		PrintfECAT(device,"DATA:CM %d",wParam);
		ECAT_sync();
		return (gError);

	case KT_SRG_NETWORK_SETSTATE:
		PrintfECAT(device,"SRG:NETWORK %d",wParam);
		ECAT_sync();
		SetNetwork  = wParam;
		return (gError);

	case KT_SRG_WAVE_SETSTATE:
		PrintfECAT(device,"SRG:WAVE %d",wParam);
		ECAT_sync();
		SetWave  = wParam;
		return (gError);

	case KT_SRG_VOLTAGE_SETSTATE:
		PrintfECAT(device,"SRG:VOLT %d",(int)((float)*fplParam));
		ECAT_sync();
		SetVoltage = *fplParam;
		return (gError);

	case KT_MEAS_SETSTATE:
		vmeas_bay=wParam;
		vmeas_state=lParam;
		if (wParam<16)
		{
			if ((HIWORD(lParam)!=0xFFFF) || (LOWORD(lParam)!=0xFFFF))
			{
				PrintfECAT(0,"MEAS:BAY %d",wParam);
				ECAT_sync();			// JLR, 10/17/95 - put ECAT_sync inside{}
			}

			if (HIWORD(lParam)==0x00FF) 
				Set_Auto_Vmeas();
			else if (HIWORD(lParam)!=0xFFFF)
			{
				PrintfECAT(0,"MEAS:VMON %d",HIWORD(lParam));
				ECAT_sync();
			}

			if (LOWORD(lParam)==0x00FF) 
				Set_Auto_Imeas();
			else if (LOWORD(lParam)!=0xFFFF)
			{
				PrintfECAT(0,"MEAS:IMON %d",LOWORD(lParam));
				ECAT_sync();
			}
		}
		break;

	case KT_EUT_QUERY:
		QueryECAT(device,"EUT?",szQueryBuffer,99);
		if (SIMULATION) return 0;
		sscanf(szQueryBuffer,"%d",&result);
		return(result);
			  
	case KT_SURGE:
		{
			int far *iptr;
			iptr=(int far *)lParam;
			QueryECAT(device,"*TRG 1",szQueryBuffer,100,10000/*2500*/);

			if (SIMULATION)
			{
				if (SetVoltage<0)
				{
					iptr[0]=(int)-SetVoltage/2;
					iptr[1]=(int)-SetVoltage/2;
					iptr[2]=(int)-SetVoltage/4;
					iptr[3]=(int)-SetVoltage/4;
				}
				else
				{
					iptr[0]=(int)SetVoltage/2;
					iptr[1]=(int)SetVoltage/2;
					iptr[2]=(int)SetVoltage/4;
					iptr[3]=(int)SetVoltage/4;
				}
			}
			else
			{
				//REal Surge
				int vpp,vpm,ipp,ipm;
				sscanf(szQueryBuffer,"%d %d %d %d %d",&result,&vpp,&vpm,&ipp,&ipm);
				iptr[0]=vpp;
				iptr[1]=vpm;
				iptr[2]=ipp;
				iptr[3]=ipm;
			}//REal Surge

			SRG_log_on(iptr[0],iptr[1],iptr[2],iptr[3]);
			return (result);
		} 

	case KT_ABORT:
		PrintfECAT(device,"ABORT");
		ECAT_sync();
		return (gError);

	case KT_SYS_STATUS:
		{
			int t[9];

			try
			{
				PURGE();
				SEND(device,0,"\005");
				GETRESPONSE("\005",szQueryBuffer,100,1000);

				if (SIMULATION) return 0;
				if (COMMERROR)  return MAKELONG((ILOCK_NOCOMM),0);

				sscanf(szQueryBuffer," %d %d %d %d %d %d %d %d %d",
				&t[7],&t[0],&t[1],&t[2],&t[3],&t[4],&t[5],&t[6],&t[8]);

				if (t[8]!=1234) 
				{
					SEND(device,0,"\030");
					ECAT_sync();
					return MAKELONG((ILOCK_NOCOMM),0);
				}
				if (!ERR_AC_LIMIT)
				{
					rms_data[0] = t[0];
					rms_data[1] = t[1];
					rms_data[2] = t[2];
					rms_data[3] = t[4];
					rms_data[4] = t[5];
					rms_data[5] = t[6];
				}

				if (t[7] & POWER_UP_BIT) POWER_UP=TRUE;

				EFT_SETUP=((t[7] & EFT_SETUP_BIT)!=0);

				if (t[7] & ERR_AC_POWER_BIT)    ERR_AC_POWER = TRUE;
				if (t[7] & ERR_AC_LIMIT_BIT)    ERR_AC_LIMIT = TRUE;
				if (t[7] & BACK_FROM_LOCAL_BIT) BACK_FROM_LOCAL = TRUE;

				if (t[7]& ILOCK_ECAT_BIT) 
				return MAKELONG((ILOCK_ECAT),t[3]);
				else  return MAKELONG((ILOCK_OK),t[3]);
			}
			catch(...)
			{
				return MAKELONG((ILOCK_NOCOMM),0);
			}
		}

	case KT_CHARGE:
		QueryECAT(device,"SRG:CHARGE",szQueryBuffer,100,2000);
		if (SIMULATION) return 0;
		sscanf(szQueryBuffer," %d ",&result);
		if (result) 
			result &=0xff;
		return (result);

	case  KT_EFT_LOG_START:
		if (Log.log_on())
		{
			Log.logtime=FALSE;
			if (Log.normal())
			{
				Log.loglpstr("  Time   Action Volts(V)  Freq   Duration Period Phase Source At    Cpl");
				if (AC_OPTION)
					Log.logprintf("%12cAC Pwr At  L1 Pk/ RMS+I  L2 Pk/ RMS+I  L3 Pk/ RMS+I\n",' ');
				else
					Log.loglpstr("\n");
			}
			else
			{
				Log.loglpstr("  Time   \tAction  \tVolts(V)  \tFreq\t    \tDur.\t \tPeriod \tPhase \tSource  \tAt     \tCpl");    
				if (AC_OPTION)
					Log.logprintf("\tAC Pwr At \tL1 Pk+I \tL1 Rms+I \tL2 Pk+I \tL2 Rms+I \tL3 Pk+I \tL3 Rms+I\n");
				else
					Log.loglpstr("\n");
			}
			Log.logtime=TRUE;
		}
		break;

	case KT_SRG_LOG_START:
		if (Log.log_on())
		{
			Log.logtime=FALSE;
			if (Log.normal())
			{
				Log.logprintf("  Time   Action Volts Phase  Source  Waveform%18cAt     Cpl Hi%6cLow  C Mode  Pk+V  Pk-V   Pk+I  Pk-I",' ',' ',' ');
				if (AC_OPTION)
					Log.logprintf("  AC Pwr At  L1 Pk/ RMS+I  L2 Pk/ RMS+I  L3 Pk/ RMS+I\n");
				else
					Log.loglpstr("\n");
			}
			else
			{                                                                                           
				Log.loglpstr("  Time   \tAction \tVolts \tPhase   \tSource   \tWaveform   \tAt    \tCpl Hi        \tLow \tC Mode \tPk+V  \tPk-V   \tPk+I  \tPk-I");
				if (AC_OPTION)
					Log.logprintf("\tAC Pwr At \tL1 Pk+I \tL1 Rms+I \tL2 Pk+I \tL2 Rms+I \tL3 Pk+I \tL3 Rms+I\n");
				else
					Log.loglpstr("\n");
			}
			Log.logtime=TRUE;
		}
		break;

	 case WM_DESTROY:
		// shutdown the hidden comm window, free up any memory, kill any timers.
		break;

	 case KT_ECAT_EXIT:
		PrintfECAT(device,"*RST");
		ECAT_sync();
		break;

	 case KT_EFT_RESETALL:
		{ 
			float eftfv;
			long far *eftlv=(long far *)&eftfv;

			PrintfECAT(device,"ABORT");
			ECAT_sync();
			SendMessage(hHideWnd,KT_EFT_INIT,0,0L);
			SendMessage(hHideWnd,KT_EFT_COUPLE_SETSTATE,0,eft_couple_raw);
			if (EUT_Power) 
				SendMessage(hHideWnd,KT_EUT_ON,0,0L);
			else if (EFT_COUPLER!=-1) 
				SendMessage(hHideWnd,KT_EUT_OFF,0,0L);

			if (fabs(EftSetVoltage)>3000.0)
			{//Freq First
				eftfv=EftFreq;
				SendMessage(hHideWnd,KT_EFT_FREQUENCY_SETSTATE,0,*eftlv);
				eftfv=EftSetVoltage;
				SendMessage(hHideWnd,KT_EFT_VOLTAGE_SETSTATE,0,*eftlv);
			}//Freq First
			else
			{//Voltage First 
				eftfv=EftSetVoltage;
				SendMessage(hHideWnd,KT_EFT_VOLTAGE_SETSTATE,0,*eftlv);
				eftfv=EftFreq;
				SendMessage(hHideWnd,KT_EFT_FREQUENCY_SETSTATE,0,*eftlv);
			}//Voltage First
			
			eftfv=EftPer;
			SendMessage(hHideWnd,KT_EFT_PERIOD_SETSTATE,0,*eftlv);
			eftfv=EftDur;
			SendMessage(hHideWnd,KT_EFT_DURATION_SETSTATE,EftDurUnits,*eftlv);
			SendMessage(hHideWnd,KT_EFT_CHIRP,EftChirp,0L);
		}
		break;

	 case KT_SRG_RESETALL:
		{ 
			float srgfv;
			long far *srglv=(long far *)&srgfv;
			PrintfECAT(device,"ABORT");
			ECAT_sync();
			SendMessage(hHideWnd,KT_SRG_LOCAL_INIT,0,0L);
			SendMessage(hHideWnd,KT_SRG_NETWORK_SETSTATE,SetNetwork,0L);
			SendMessage(hHideWnd,KT_SRG_WAVE_SETSTATE,SetWave,0L);

			// Conditionalize coupler parameter in following call;  0 for
			// SetOutput gives error message.  JFL 10/17/95
			SendMessage(hHideWnd,
			KT_SRG_OUTPUT_SETSTATE,
			SetOutput == 0 ? 0xFF : SetOutput,0L);
			if (EUT_Power) 
			SendMessage(hHideWnd,KT_EUT_ON,0,0L);
			else if (SURGE_COUPLER!=-1) 
			SendMessage(hHideWnd,KT_EUT_OFF,0,0L);

			srgfv=SetVoltage;
			SendMessage(hHideWnd,KT_SRG_VOLTAGE_SETSTATE,0,*srglv);
			if (SetOutput==SURGE_COUPLER)
			{
				SendMessage(hHideWnd,KT_SRG_COUPLE_SETSTATE,0,couple_raw);
				SendMessage(hHideWnd,KT_SRG_PHASE_MODE,0,PhaseMode);

				if ((PhaseMode>0) && (PhaseMode<4))
				{
					srgfv=SetPhase;
					SendMessage(hHideWnd,KT_PHASE_ANGLE_SETSTATE,0,*srglv);
				}
			}
			SendMessage(hHideWnd,KT_MEAS_SETSTATE,vmeas_bay,vmeas_state);
		}
		break;

	case KT_AUX_ON:
		PrintfECAT(device,"PQF:AUX 1"); 
		ECAT_sync();
		return (gError);

	case KT_AUX_OFF:
		PrintfECAT(device,"PQF:AUX 0"); 
		ECAT_sync();
		return (gError);

    case KT_PQF_IDLE:
        // set idle (standby) power level, wParam = 0 (Open-0%), 100 (100%) 
		PrintfECAT(device,"PQF:IDLE %d",wParam);
		ECAT_sync(5000);              // add delay JFL 9/1/95
		break;     

    case KT_PQF_START:
		PrintfECAT(device,"PQF:RUN %d",wParam);
		ECAT_sync (5000);
		 //          sprintf(sendbuf,"PQF:RUN %d",wParam);
		//      QueryECAT(device,sendbuf,szQueryBuffer,100,2500); */
		//      QueryECAT(device,"PQF:RUN 0 ",szQueryBuffer,100,2500);
		//      sscanf(szQueryBuffer,"%d ",&result);
		//      return result;
		return 0;

	case KT_PQF_SETV:
		PrintfECAT(device,"PQF:NOM %d",wParam);
		ECAT_sync();
		break;     

	case KT_PQF_READ_DATA:
	{
		int i;
		result=0;
		for (i=0; i<16; i++)
		{
			// Reworked by JFL 11/1/95:  limits wrong for EP61;  EP3 above
			// other PQF modules caused bad limit value to be returned.

			if (NOT_GHOST(i) && (calset[i].type & TYPE_PQF))
				if (calset[i].id==EP62_BOXT) 
					return MAKELONG(320,100);
				else if (calset[i].id==EP61_BOXT) 
					return MAKELONG(160,100);
		}
		return MAKELONG(0,0);  // EP3 only
	}

	case KT_PQF_STOP:
		try
		{
			PURGE();
			SEND(device,0,"\004");
			ECAT_sync();
		}
		catch(...)
		{
		}
		break;     

	case KT_PQF_RESET:
		PrintfECAT(device,"PQF:RESET");
		ECAT_sync();
		break;    

	case KT_PQF_EXIT:
		//		PQF:EXIT -- tell ECAT leaving PQF mode
		PrintfECAT(device, "PQF:EXIT");
		ECAT_sync();
		break;

	case KT_PQF_STATUS:
	{
		try
		{
			int t[8];
			PURGE();
			SEND(device,0,"\005");
			GETRESPONSE("\005",szQueryBuffer,100,1000);
			/*    QueryECAT(device,"PQF:S? ",szQueryBuffer,100,2500);
			*/      sscanf(szQueryBuffer," %d %d %d %d %d %d %d %d %d",&t[7],&t[0],&t[1],&t[2],&t[3],&t[4],&t[5],&t[6], &t[8]);
			((LPPQFSTATUS)(lParam))->pqfs_nIlock=(t[7]&1);
			((LPPQFSTATUS)(lParam))->pqfs_rms_iread=t[0];
			((LPPQFSTATUS)(lParam))->pqfs_peak_iread=t[1];
			((LPPQFSTATUS)(lParam))->pqfs_step_number=t[2];
			((LPPQFSTATUS)(lParam))->pqfs_eut_status=t[3];
			((LPPQFSTATUS)(lParam))->pqfs_error=t[4];
			((LPPQFSTATUS)(lParam))->pqfs_line_freq=t[5];
			((LPPQFSTATUS)(lParam))->pqfs_line_voltage=t[6];
			if (t[8]!=1234) 
			{
				SEND(device,0,"\030");
				ECAT_sync();
				return 0;
			}
			return lParam;
		}
		catch(...)
		{
			return 0;
		}
	}

	case KT_PQF_GET_ERROR:
		sprintf(szQueryBuffer,"PQF:ERR? %d",wParam);
		QueryECAT(device,szQueryBuffer,(LPSTR)lParam,255,2500);
		DBGOUT("KT_PQF_GET_ERROR: %s\n", (LPSTR)lParam);
		break;

	case KT_PQF_SEND_LIST: /* wParam is hPqfTestPacketList, lParam=testlen */
	{
		LPPQFLIST plist = (LPPQFLIST) wParam;
        if (plist)
		{
			PrintfECAT(device,"PQF:RESET");
			ECAT_sync();

			// Show uploading list dialog
			PqfShowUploadDialog(TRUE);
			CEcatApp::BeginWaitCursor();
			for (int i=0; i<lParam; i++)
			{
				if (PrintfECAT(device,"PQF:SET %d %ld %d %d %d",
							(int) plist[i].tp_action,
							plist[i].tp_duration,
							plist[i].tp_start_angle,
							plist[i].tp_RMS_limit,
							plist[i].tp_PEAK_limit))
					ECAT_sync();
				else
				{
					CEcatApp::EndWaitCursor();
					PqfShowUploadDialog(FALSE);
					Message("Communication Error", "Fatal error: Unable to upload the test plan to ECAT");
					return 1L;
				}
			}

			// Hide the dialog
			CEcatApp::EndWaitCursor();
			PqfShowUploadDialog(FALSE);
		}
	}
	break;

	case KT_PQF_QUALIFY:
		int ipp,ipm;
		QueryECAT(device,"PQF:QUALIFY",szQueryBuffer,100,12000);
		sscanf(szQueryBuffer,"%d %d ",&ipp,&ipm);  /* ipp=peakpos, ipm=peakminus */
		return (MAKELONG(ipp,ipm));

	case KT_PQF_SET_PHASE:
		PrintfECAT(device,"PQF:PHASE %d",wParam);
		ECAT_sync(5000);			// JFL 6/23/95 increase delay
		break;     

	 default:
			/* For any message for which you don't specifically provide a  */
			/* service routine, you should return the message to Windows   */
			/* for default mesSURGE_COUPLERsage processing.                             */
			return DefWindowProc(hHideWnd, message, wParam, lParam);
	}

	return 0L;
}     /* End of KT_HiddenWndProc                                         */

void Set_Auto_Vmeas(void)
{
	int i;
	int h = 1;
	int l = 2;
	UINT ch_val;

	//JLR, 10/17/95 - single phase auto - only if AC coupler selected for measure
	if (couple_raw>0)
	{
		if  ((vmeas_bay==AC_COUPLER) && !SURGE_THREE_PHASE) // JLR, 10/17/95
		{
			i=(int)(couple_raw&255);

			if (i & BIT_L1)  		l=1; 
			else if (i & BIT_L2) l=2; 
			else if (i & BIT_L3) l=5; 
			else if (i & BIT_N)  l=4; 
			else l=3;

			i=(int)((couple_raw>>8)&255);

			if (i & BIT_L1)       h=1; 
			else if (i & BIT_L2)  h=2; 
			else if (i & BIT_L3)  h=5; 
			else if (i & BIT_N)   h=4; 
			else h=3;
		}
		else
		{
			i=(int)(couple_raw&255);

			if (i & BIT_L1)  		l=1; 
			else if (i & BIT_L2) l=2; 
			else if (i & BIT_L3) l=3; 
			else if (i & BIT_N)  l=4; 
			else l=5;

			i=(int)((couple_raw>>8)&255);

			if (i & BIT_L1)       h=1; 
			else if (i & BIT_L2)  h=2; 
			else if (i & BIT_L3)  h=3;
			else if (i & BIT_N)   h=4; 
			else h=5;
		}
	}

	ch_val=l+(16*h);
	PrintfECAT(0,"MEAS:VMON %d",ch_val);
	ECAT_sync();
}

void Set_Auto_Imeas(void)
{
	int i;
	int h = 1;
	UINT ch_val;

	//JLR, 10/17/95 - single phase auto - only if AC coupler selected for measure
	if (couple_raw > 0) 
	{
		if  ((vmeas_bay==AC_COUPLER) && !SURGE_THREE_PHASE) //JLR, 10/17/95
		{
			i=(int)((couple_raw>>8)&255);

			if (i & BIT_L1)      h=1; 
			else if (i & BIT_L2) h=2; 
			else if (i & BIT_L3) h=5; 
			else if (i & BIT_N)  h=4; 
			else h=3;
		}
		else
		{
			i=(int)((couple_raw>>8)&255);

			if (i & BIT_L1)      h=1; 
			else if (i & BIT_L2) h=2; 
			else if (i & BIT_L3) h=3; 
			else if (i & BIT_N)  h=4; 
			else h=5;
		}
	}

	ch_val = h;
	PrintfECAT(0,"MEAS:IMON %d",ch_val);
	ECAT_sync();
}


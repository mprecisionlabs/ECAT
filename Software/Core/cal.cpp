/******************************************************************************

	CAL.CPP --	Calibration functions. Includes module type (capability)
					definitions.  Supports CAL dialog in BurstWare, SurgeWare.

	History:		Start with 4.0A version.

		5/17/95		E521, E521C added to fill[] array.  	 			JFL
						CPLHV attribute added to E522.
		5/23/95		E509 added to fill[] array, copy from 4.0B.		JFL
		7/12/95		EP91/2/3/4 added for fill[].							JFL
		9/5/95		CE40/50 added in fill[].                        JFL
						Set IS_CE_BOX when have CE module.
						Set CE_EP_OPTION based on AC_OPTION bit.
		9/12/95		E4556 added in fill[].									JFL
		4/19/96		E45XX couplers added in fill[]. 	 				  	JFL
		4/29/96		Change names of E455xKV modules to 455xK.			JFL

******************************************************************************/

#include "stdafx.h"
#include "module.h"
#include "dlgctl.h"
#include "globals.h"
#include "calinfo.h"
#include "comm.h"
#include "EcatApp.h"
#include "Shlwapi.h"
#pragma comment(lib, "Shlwapi.lib")  // pass lib to linker

#define CAL_FILE_NAME			"sim.cfg"
#define MAX_BUFFER_LEN			128

const DWORD CAL_FILE_COOKIE = 0x4C414345;	// ECAL
const DWORD CAL_FILE_VERSION = 101;			// 1.01

// Forward declaration
void Get_Cal_Record(int i);
void Cal_Write();

struct DECODE_STRUCT
{
	char mod_name[8];
	long mod_type;               
	long mod_type2;
	char modid;
	char vi_byte;
	long options;
	long serial;
};

struct DECODE_STRUCT_SRC
{
	char mod_name[16];
	char mod_type[8];
	char mod_type2[8];
	char modid[2];
	char vi_byte[2];
	char options[8];
	char serial[8];
};

#define CAL_SETUP_BITS  (WM_USER+100)
#define CAL_SETUP_OPT   (WM_USER+101)
#define CAL_SETUP_N     (WM_USER+102)

#define PQF_AUX_BIT     (1)

#define VIMNAMES_STRING_SIZE    (10)  // size of each character string in vmnames and imnames arrays
const char mnames[12][4]   ={"JAN","FEB","MAR","APR","MAY","JUN","JUL","AUG","SEP","OCT","NOV","DEC"};
const char vmnames[][VIMNAMES_STRING_SIZE]  =   {"None","A,B","A,B,C","None","None"};
const char vmnames5[][VIMNAMES_STRING_SIZE] =   {"None","A,B","A,B,C","A,B,C,D","A,B,C,D,E"};
const char imnames[][VIMNAMES_STRING_SIZE]  =   {"None","1","1,2","1,2,3","None","None"};
const char imnames5[][VIMNAMES_STRING_SIZE] =   {"None","1","1,2","1,2,3","1,2,3,4","1,2,3,4,5"};
const char *ErrorEeprom[] ={"Error writing Serial EEPROM",
									  "Illegal Module Address",
									  "No Module Exists in designated slot",
									  "Data Block too large",
									  "Temporary storage failed",
									  "Erase operation failed",
									  "Write operation failed"};


typedef struct
{
	char           *name;
	unsigned char  id;
	unsigned long  type_mod;
	unsigned char  def_vmeas;
} FILLINS;

const FILLINS fill[] =
{ 
	{"E501 ",E501_BOXT, TYPE_SRG,0x00},
	{"E501A",E501A_BOXT,TYPE_SRG,0x00},
	{"E501B",E501B_BOXT,TYPE_SRG,0x00},
	{"CE50 ",CE40_BOXT, TYPE_SRG|TYPE_CPLSRG|TYPE_CPLAC|TYPE_CE,0x22},
	{"E502 ",E502_BOXT, TYPE_SRG,0x00},
	{"E502A",E502A_BOXT,TYPE_SRG,0x00},
	{"E502B",E502B_BOXT,TYPE_SRG,0x00},
	{"E502C",E502C_BOXT,TYPE_SRG,0x00},
	{"E502H",E502H_BOXT,TYPE_SRG,0x00},
	{"E502K",E502K_BOXT,TYPE_SRG,0x00},
	// **** 		{"E502AK",E502K_BOXT,TYPE_SRG,0x00},
	{"E503 ",E503_BOXT, TYPE_SRG,0x00},
	{"E503A",E503A_BOXT, TYPE_SRG,0x00},
	{"E504 ",E504_BOXT, TYPE_SRG,0x00},
	{"E504A",E504A_BOXT,TYPE_SRG,0x00},
	{"E504B",E504B_BOXT,TYPE_SRG,0x00},
	{"E505 ",E505_BOXT, TYPE_SRG,0x00},
	{"E505A",E505A_BOXT, TYPE_SRG,0x00},
	{"E505B",E505B_BOXT, TYPE_SRG|TYPE_CPLSRG|TYPE_CPLAC,0x00},
	{"E506 ",E506_BOXT, TYPE_SRG,0x00},
	{"506-4",E506_4W_BOXT, TYPE_SRG,0x00},
	{"E507 ",E507_BOXT, TYPE_SRG,0x00},
	{"E508 ",E508_BOXT, TYPE_SRG,0x00},
	{"E509 ",E509_BOXT, TYPE_SRG,0x00},
	{"E509A",E509A_BOXT, TYPE_SRG,0x00},
	{"E510 ",E510_BOXT, TYPE_SRG,0x00},
	{"E510A",E510A_BOXT,TYPE_SRG,0x00},
	{"E511 ",E511_BOXT, TYPE_SRG,0x00},
	{"E513 ",E513_BOXT, TYPE_SRG,0x00},
	{"E514 ",E514_BOXT, TYPE_SRG,0x00},
	{"E515 ",E515_BOXT, TYPE_SRG,0x00},
	{"E518 ",E518_BOXT, TYPE_SRG,0x00},
	{"E521 ",E521_BOXT, TYPE_SRG,0x00},
	{"E522 ",E522_BOXT, TYPE_SRG,0x00},

	{"E551 ",E551_BOXT, TYPE_CPLSRG|TYPE_CPLAC,0x22},
	{"E552 ",E552_BOXT, TYPE_CPLSRG|TYPE_CPLAC,0x22},
	{"E553 ",E553_BOXT, TYPE_CPLSRG|TYPE_CPLTRI|TYPE_CPLAC,0x33},
	{"E554 ",E554_BOXT, TYPE_CPLSRG|TYPE_CPLTRI|TYPE_CPLAC,0x33},
	{"E4551",E4551_BOXT,TYPE_CPLSRG|TYPE_CPLEFT|TYPE_CPLAC,0x22},
	{"4551A",E4551_BOXT,TYPE_CPLSRG|TYPE_CPLEFT|TYPE_CPLAC|TYPE_CPL10KV,0x22},
	{"4551K",E4551_BOXT,TYPE_CPLSRG|TYPE_CPLEFT|TYPE_CPLAC|TYPE_CPL10KV,0x22},
	{"E4552",E4552_BOXT,TYPE_CPLSRG|TYPE_CPLEFT|TYPE_CPLAC,0x22},
	{"4552A",E4552_BOXT,TYPE_CPLSRG|TYPE_CPLEFT|TYPE_CPLAC|TYPE_CPL10KV,0x22},
	{"4552K",E4552_BOXT,TYPE_CPLSRG|TYPE_CPLEFT|TYPE_CPLAC|TYPE_CPL10KV,0x22},
	{"E4553",E4553_BOXT,TYPE_CPLSRG|TYPE_CPLTRI|TYPE_CPLEFT|TYPE_CPLAC,0x33},
	{"4553A",E4553_BOXT,TYPE_CPLSRG|TYPE_CPLTRI|TYPE_CPLEFT|TYPE_CPLAC|TYPE_CPL10KV,0x33},
	{"4553K",E4553_BOXT,TYPE_CPLSRG|TYPE_CPLTRI|TYPE_CPLEFT|TYPE_CPLAC|TYPE_CPL10KV,0x33},
	{"E4554",E4554_BOXT,TYPE_CPLSRG|TYPE_CPLTRI|TYPE_CPLEFT|TYPE_CPLAC,0x33},
	{"4554A",E4554_BOXT,TYPE_CPLSRG|TYPE_CPLTRI|TYPE_CPLEFT|TYPE_CPLAC|TYPE_CPL10KV,0x33},
	{"4554K",E4554_BOXT,TYPE_CPLSRG|TYPE_CPLTRI|TYPE_CPLEFT|TYPE_CPLAC|TYPE_CPL10KV,0x33},
	{"E4556",E4556_BOXT,TYPE_CPLSRG|TYPE_CPLTRI|TYPE_CPLEFT|TYPE_CPLAC,0x33},
	{"E522C",E522C_BOXT, TYPE_CPLSRG|TYPE_CPLTRI|TYPE_CPLAC|TYPE_CPLHV,0x33},
	{"E50812P",E50812P_BOXT, TYPE_CPLSRG,0x00},
	{"E571 ",E571_BOXT, TYPE_CPLSRG|TYPE_CPLIO,0x00},
	{"E521C",E521C_BOXT, TYPE_CPLSRG|TYPE_CPLAC|TYPE_CPLHV,0x33},

	{"E411 ",E411_BOXT, TYPE_EFT,0x00},
	{"CE40 ",CE40_BOXT, TYPE_EFT|TYPE_CPLEFT|TYPE_CPLAC|TYPE_CE,0x00},
	{"E412 ",E412_BOXT, TYPE_EFT|TYPE_CPLEFT|TYPE_CPLAC,0x00},
	{"E421 ",E421_BOXT, TYPE_EFT,0x00},
	{"E421P",E421P_BOXT,TYPE_EFT,0x00},
	{"E422 ",E422_BOXT, TYPE_EFT|TYPE_CPLEFT|TYPE_CPLAC,0x00},
	{"E423 ",E423_BOXT, TYPE_EFT|TYPE_CPLEFT|TYPE_CPLTRI|TYPE_CPLAC,0x00},
	{"E424 ",E424_BOXT, TYPE_EFT|TYPE_CPLEFT|TYPE_CPLTRI|TYPE_CPLAC,0x00},
	{"E433 ",E433_BOXT, TYPE_EFT|TYPE_CPLEFT|TYPE_CPLTRI|TYPE_CPLAC,0x00},
	{"E434 ",E434_BOXT, TYPE_EFT|TYPE_CPLEFT|TYPE_CPLTRI|TYPE_CPLAC,0x00},

	{"EP61 ",EP61_BOXT, TYPE_PQF,  0x00},
	{"EP62 ",EP62_BOXT, TYPE_PQF,  0x00},
	{"EP3  ",EP3_BOXT,  TYPE_PQF,  0x00},

	{"EHV2 ",EHV2_BOXT, TYPE_POWER,0x00},
	{"EHV2Q",EHV2Q_BOXT,TYPE_POWER,0x00},
	{"EHV10",EHV10_BOXT,TYPE_POWER,0x00},

	{"EP71 ",EP71_BOXT, TYPE_AMP,  0x00},
	{"EP72 ",EP72_BOXT, TYPE_AMP,  0x00},
	{"EP73 ",EP73_BOXT, TYPE_AMP|TYPE_CPLTRI,  0x00},
	{"EP91 ",EP91_BOXT, TYPE_AMP,  0x00},
	{"EP92 ",EP92_BOXT, TYPE_AMP,  0x00},
	{"EP93 ",EP93_BOXT, TYPE_AMP|TYPE_CPLTRI,  0x00},
	{"EP94 ",EP94_BOXT, TYPE_AMP|TYPE_CPLTRI,  0x00},
	{"ERI1 ",ERI1_BOXT, TYPE_AMP,  0x00},
	{"ERI3 ",ERI3_BOXT, TYPE_AMP|TYPE_CPLTRI,  0x00},

	{"EOP1 ",EOP1_BOXT, TYPE_CLAMP,  0x00},
	{"EOP3 ",EOP3_BOXT, TYPE_CLAMP|TYPE_CPLTRI, 0x00},

	{"",0,0}
};

static int calsetup;
static HFONT hFixedFont = NULL;
static unsigned long choose_mask;

// PQF Ep3 specific
static BOOL bIsEP3 = FALSE;
BOOL check_type() {return bIsEP3;}

void InitGhostBox(int i)
{
	if ((i < 0) || (i >= MAX_CAL_SETS))
		return;

	calset[i].id		= 0;
	calset[i].type		= 0;
	calset[i].options	= 0;
	calset[i].vmeas		= 0;
	calset[i].imeas		= 0;
	calset[i].serial	= 0;
	calset[i].cal_valid = 0;

	memset(calset[i].name, 0, sizeof(calset[i].name));
	memset(calset[i].cal, 0xFF, sizeof(calset[i].cal));
}

static void ResetCalData(BOOL bSetSerial = FALSE)
{
	for (int i = 0; i < MAX_CAL_SETS; i++)
	{
		InitGhostBox(i);
		if (bSetSerial)
			calset[i].serial = i;
	}
}

void Get_Calinfo_Name(int i, LPSTR BayName)
{
	char local_buffer[MAX_BUFFER_LEN];
	memset(local_buffer, 0, sizeof(local_buffer));
	char temp_buf[40];
	memset(temp_buf, 0, sizeof(temp_buf));
	unsigned long type;
	int j;

	char *side = i & 1 ? "Left" : "Right";

	sprintf(temp_buf,"SN %.7ld Row %d %s",calset[i].serial,(i/2)+1,side);
	sprintf(local_buffer,"%-5s ",calset[i].name);

	type=calset[i].type;
	if (type & TYPE_CPLTRI) _fstrcat(local_buffer,"Three Phase: ");

	if (type & TYPE_CPLHV)  _fstrcat(local_buffer,"HV ");

	if ((type & TYPE_EFT)   && (type & TYPE_CPLEFT))_fstrcat(local_buffer,"EFT Gen & Coupler");
	else
	if ((type & TYPE_CPLSRG)&& (type & TYPE_CPLEFT))_fstrcat(local_buffer,"SURGE & EFT Coupler");
	else
	if ((type & TYPE_CPLIO) && (type & TYPE_CPLSRG))_fstrcat(local_buffer,"SURGE DATA Coupler");
	else
	if ((type & TYPE_CPLAC) && (type & TYPE_CPLSRG))_fstrcat(local_buffer,"SURGE LINE Coupler");
	else
	if (type & TYPE_CPLIO) _fstrcat(local_buffer,"DATA Coupler");
	else
	if (type & TYPE_CPLSRG)_fstrcat(local_buffer,"SURGE Coupler"); 
	else
	if (type & TYPE_CPLEFT)_fstrcat(local_buffer,"EFT Coupler"); 
	else
	if (type & TYPE_SRG)   _fstrcat(local_buffer,"SURGE Generator"); 
	else
	if (type & TYPE_EFT)   _fstrcat(local_buffer,"EFT Generator"); 
	else
	if (type & TYPE_PQF)   _fstrcat(local_buffer,"PQF Simulator");
	else
	if (type & TYPE_POWER) _fstrcat(local_buffer,"Charge Booster");
	else
	if (type & TYPE_AMP)   _fstrcat(local_buffer,"Amplifier");
	else
	if (type & TYPE_ESD)   _fstrcat(local_buffer,"ESD Generator"); 
	else
	if (type & TYPE_IMPED) _fstrcat(local_buffer,"Impedance network"); 
	else
	if (type & TYPE_CLAMP) _fstrcat(local_buffer,"EMC Clamp"); 

	for (j=_fstrlen(local_buffer); j<44; j++) 
		local_buffer[j]=' ';
	local_buffer[44]=0;

	_fstrcat(local_buffer,temp_buf);
	strcpy(BayName,local_buffer);
}
   
BOOL __declspec(dllexport) CALLBACK ChooseProc(HWND hMainDlg, UINT message, UINT wParam, LONG lParam);

void Choose_Coupler(HWND hwnd)
{
	if (SURGE_COUPLER!=99) return;
	SURGE_COUPLER=-1;
	SURGE_THREE_PHASE=FALSE;
	DLGPROC	lpfnChoose = (DLGPROC) ChooseProc;
	choose_mask=TYPE_CPLSRG | TYPE_CPLAC;
	DialogBox(ghinst, MAKEINTRESOURCE(IDD_CHOOSE_COUPLER), hwnd, lpfnChoose);
}

void Choose_EFT_Coupler(HWND hwnd)
{
	if (EFT_COUPLER!=99) return;
	EFT_COUPLER=-1;
	EFT_THREE_PHASE=FALSE;
	choose_mask=TYPE_CPLEFT;
	DLGPROC	lpfnChoose = (DLGPROC) ChooseProc;
	DialogBox(ghinst, MAKEINTRESOURCE(IDD_CHOOSE_COUPLER), hwnd, lpfnChoose);
}

void Choose_EFT_Module(HWND hwnd)
{
	if (EFT_MODULE!=99) return;

	EFT_MODULE=-1;
	EFT_2MHZ=FALSE;
	EFT_8KV=FALSE;
	EFT_CHIRP=FALSE;
	DC_SURGE=FALSE;
	choose_mask=TYPE_EFT;
	DLGPROC	lpfnChoose = (DLGPROC)ChooseProc;
	DialogBox(ghinst, MAKEINTRESOURCE(IDD_CHOOSE_EFT_MODULE), hwnd, lpfnChoose);
}

BOOL __declspec(dllexport) CALLBACK ChooseProc(HWND hMainDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	char BayName[128];
	int i,rv;

	switch (message)
	{
	case WM_INITDIALOG:
		hFixedFont = CreateFont(
			12,							// Height
			0,							// Width 
			0,							// Escapement
			0,							// Orientation
			FW_NORMAL,					// Weight
			FALSE,						// Italic
			FALSE,						// Underline
			FALSE,						// StrikeOut
			ANSI_CHARSET,				// CharSet
			OUT_DEFAULT_PRECIS,			// OutPrecision
			CLIP_DEFAULT_PRECIS,		// ClipPrecision 
			DEFAULT_QUALITY,			// Quality
			FIXED_PITCH | FF_MODERN,	// PitchAndFamily
			"Lucida Console"			// FaceName
		);

		SendDlgItemMessage(hMainDlg,IDD_CHO_LIST,WM_SETFONT,(WPARAM)hFixedFont,0L);
		SendDlgItemMessage(hMainDlg, IDD_CHO_LIST, LB_RESETCONTENT,	0, 0L);

		for (i=0; i<MAX_CAL_SETS; i++)
		{  
			if (!(calset[i].type & TYPE_CPLIO))
			if ((calset[i].type & choose_mask)	== choose_mask)
			{
				Get_Calinfo_Name(i,BayName);
				rv=SendDlgItemMessage(hMainDlg, IDD_CHO_LIST, LB_ADDSTRING,	0, (LONG)(LPSTR)BayName);
				SendDlgItemMessage(hMainDlg, IDD_CHO_LIST, LB_SETITEMDATA,	rv, (LONG)i);
			}
		}
		SendDlgItemMessage(hMainDlg,IDD_CHO_LIST,LB_SETCURSEL,0,0L);
		return (FALSE);

	case WM_COMMAND:
		if (LOWORD(wParam)!=IDOK) return (FALSE);

		// Close the dialog
		rv=SendDlgItemMessage(hMainDlg, IDD_CHO_LIST, LB_GETCURSEL,	0,0L);
		if (rv==LB_ERR) rv=0;
		rv=SendDlgItemMessage(hMainDlg, IDD_CHO_LIST, LB_GETITEMDATA,	rv, 0L);

		if (calset[rv].type & TYPE_CPLEFT)
		{
			EFT_COUPLER=rv;
			if (calset[rv].options & DC_SURGE_BIT)DC_EFT=TRUE;
		}
		
		if ((calset[rv].type & (TYPE_CPLEFT | TYPE_CPLTRI))==(TYPE_CPLEFT | TYPE_CPLTRI))
			EFT_THREE_PHASE=TRUE;

		if (calset[rv].type & TYPE_CPLEFT) EFT_COUPLER=rv;

		if ((calset[rv].type & TYPE_CPLSRG ) && (calset[rv].type & TYPE_CPLAC))
		{
			SURGE_COUPLER=rv;
			if (calset[rv].options & DC_SURGE_BIT)DC_SURGE=TRUE;
		}

		if ((calset[rv].type & (TYPE_CPLSRG | TYPE_CPLTRI))==(TYPE_CPLSRG | TYPE_CPLTRI))
			SURGE_THREE_PHASE=TRUE;
		if (calset[rv].type & TYPE_EFT) 
		{
			if (calset[rv].options & EFT_2M_BIT)   EFT_2MHZ=TRUE;
			if (calset[rv].options & EFT_8K_BIT)   EFT_8KV=TRUE;
			if (calset[rv].options & EFT_CHIRP_BIT)EFT_CHIRP=TRUE;
			EFT_MODULE=rv;
		}

		if (hFixedFont != NULL)
		{
			DeleteObject(hFixedFont);
			hFixedFont = NULL;
		}

		EndDialog(hMainDlg,FALSE);
		return (FALSE);
	}//case

	return (FALSE);
}

int GetModuleNumber(HWND hMainDlg)
{
	int iSpinNdx = LOWORD(SendDlgItemMessage(hMainDlg, IDD_MOD_SPIN, UDM_GETPOS, 0, 0L));
	DBGOUT("GetModuleNumber: spin index: %d.\n", iSpinNdx);
	iSpinNdx = (iSpinNdx - 1) * 2;
	if (SendDlgItemMessage(hMainDlg, IDD_LEFT_MOD, BM_GETCHECK, 0, 0L)) 
		iSpinNdx++;
	DBGOUT("GetModuleNumber: module number: %d.\n", iSpinNdx);
	return iSpinNdx;
}

BOOL __declspec(dllexport) CALLBACK	CalProc(HWND hMainDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	int i,j,error;
	unsigned int ui;
	unsigned long l;
	HCURSOR old_cursor;
	char buff[40];

	switch (message)
	{
	case WM_INITDIALOG:
		// Fill in Module Name combo
		for (i = 0; (::fill[i].type_mod); i++)
			SendDlgItemMessage(hMainDlg, IDD_NAME_COMBO, CB_ADDSTRING,	0, (LONG) ((LPSTR) ::fill[i].name));
		SendDlgItemMessage(hMainDlg, IDD_NAME_COMBO, CB_SETCURSEL, 0, 0L);

		// Set edit boxes  Row, Cal Values, VM, IM
		SetDlgItemText(hMainDlg, IDD_MODULE_EDIT, "1");
		SetDlgItemText(hMainDlg, IDD_CAL_EDIT, "");
		SetDlgItemText(hMainDlg, IDD_VM_TEXT, "VM");
		SetDlgItemText(hMainDlg, IDD_IM_TEXT, "IM");

		// set ranges and initial positions for spin buttons
		SendDlgItemMessage(hMainDlg, IDD_MOD_SPIN,UDM_SETRANGE ,0, MAKELONG(8,1));
		SendDlgItemMessage(hMainDlg, IDD_MOD_SPIN,UDM_SETPOS, 0, MAKELONG(1, 0));

		SendDlgItemMessage(hMainDlg, IDD_VM_SPIN, UDM_SETRANGE, 0, MAKELONG(4,0));
		SendDlgItemMessage(hMainDlg, IDD_VM_SPIN, UDM_SETPOS, 0, 0L);

		SendDlgItemMessage(hMainDlg, IDD_IM_SPIN, UDM_SETRANGE, 0, MAKELONG(5,0));
		SendDlgItemMessage(hMainDlg, IDD_IM_SPIN, UDM_SETPOS, 0, 0L);
		
		// Select Right radio button
		SendDlgItemMessage(hMainDlg, IDD_RIGHT_MOD, BM_SETCHECK, 1, 0);

		// Get the data
		Cal_Read();

		// Select the first row on the right
		SendMessage(hMainDlg, CAL_SETUP_N, 0, 0L);
		break;

	case CAL_SETUP_N:
		// Sanity check, so we won't get crashes because of the bad index
		i = (int) wParam;
		if ((i < 0) || (i >= MAX_CAL_SETS))
			break;

		DBGOUT("CAL_SETUP_N cmnd. selected module: %d.\n", i);
		if (!calset[wParam].cal_valid) 
			Get_Cal_Record(wParam);

		if (strlen(calset[wParam].name) == 0)
			SendDlgItemMessage(hMainDlg, IDD_NAME_COMBO, CB_SETCURSEL, (WPARAM) -1, 0); 
		else
			SendDlgItemMessage(hMainDlg, IDD_NAME_COMBO, CB_SELECTSTRING, (WPARAM) -1, (LPARAM) calset[wParam].name); 

		sprintf(buff, "%.7ld", calset[wParam].serial);
		SetDlgItemText(hMainDlg, IDD_SERN_EDIT, buff);
		
		SetDlgItemInt(hMainDlg, IDD_ID_EDIT, calset[wParam].id, 0);
		SendDlgItemMessage(hMainDlg, IDD_VM_SPIN, UDM_SETPOS, 0, MAKELPARAM(calset[wParam].vmeas, 0));
		SendMessage(hMainDlg, WM_VSCROLL, MAKEWPARAM(SB_ENDSCROLL, calset[wParam].vmeas), (LPARAM) GetDlgItem(hMainDlg, IDD_VM_SPIN));
		SendDlgItemMessage(hMainDlg, IDD_IM_SPIN, UDM_SETPOS, 0, MAKELPARAM(calset[wParam].imeas, 0));
		SendMessage(hMainDlg, WM_VSCROLL, MAKEWPARAM(SB_ENDSCROLL, calset[wParam].imeas), (LPARAM) GetDlgItem(hMainDlg, IDD_IM_SPIN));
		
		SendDlgItemMessage(hMainDlg, IDD_CAL_LIST, LB_RESETCONTENT, 0, 0L);
		SendDlgItemMessage(hMainDlg, IDD_CAL_LIST, LB_ADDSTRING, 0, (LPARAM)(LPSTR)"Start of List");
		for (j = 0; j < CALDATASIZE; j++)
		{
			if (calset[wParam].cal[j] != 0xffff)
				sprintf(buff, "CAL[%2d]=%u", j+1, calset[wParam].cal[j]);
			else 
				sprintf(buff, "CAL[%2d]", j+1);

			SendDlgItemMessage(hMainDlg, IDD_CAL_LIST, LB_ADDSTRING, 0, (LPARAM)(LPSTR)buff);
			SendDlgItemMessage(hMainDlg, IDD_CAL_LIST, LB_SETITEMDATA, j+1, calset[wParam].cal[j]);
		}
		SendDlgItemMessage(hMainDlg, IDD_CAL_LIST, LB_ADDSTRING, 0, (LPARAM)(LPSTR)"End of List");
		SendDlgItemMessage(hMainDlg, IDD_CAL_LIST, LB_SETCURSEL, -1, 0L);
		SendDlgItemMessage(hMainDlg, IDD_CAL_LIST, LB_SETTOPINDEX, 0, 0L);

		SendMessage(hMainDlg, CAL_SETUP_OPT, 0, calset[wParam].options);
		SendMessage(hMainDlg, CAL_SETUP_BITS, 0, calset[wParam].type);

		SetDlgItemInt(hMainDlg, IDD_MODULE_EDIT, (UINT) LOWORD(SendDlgItemMessage(hMainDlg, IDD_MOD_SPIN, UDM_GETPOS, 0, 0L)), FALSE);
		break;

	case CAL_SETUP_OPT:
		SendDlgItemMessage(hMainDlg,IDD_EFT2M,    BM_SETCHECK,(lParam & EFT_2M_BIT)!=0,   0);
		SendDlgItemMessage(hMainDlg,IDD_EFT_8KV,  BM_SETCHECK,(lParam & EFT_8K_BIT)!=0,   0);
		SendDlgItemMessage(hMainDlg,IDD_EFT_CHIRP,BM_SETCHECK,(lParam & EFT_CHIRP_BIT)!=0,0);
		SendDlgItemMessage(hMainDlg,IDD_SRGDC,    BM_SETCHECK,(lParam & DC_SURGE_BIT)!=0, 0);
		SendDlgItemMessage(hMainDlg,IDD_SW_E502,  BM_SETCHECK,(lParam & E502_SW_BIT)!=0,  0);
		SendDlgItemMessage(hMainDlg,IDD_ACOPT,    BM_SETCHECK,(lParam & AC_OPT_BIT)!=0,   0);
		SendDlgItemMessage(hMainDlg,IDD_5CHAN,    BM_SETCHECK,(lParam & INSTR_5CH_BIT)!=0,   0);
		SendDlgItemMessage(hMainDlg,IDD_EHVSUPPORT,BM_SETCHECK,(lParam & EHV_BIT)!=0,   0);
		break;

	// See, if our spin controls are communicating
	case WM_VSCROLL:
		if (LOWORD(wParam) == SB_ENDSCROLL)
		{
			if ((HWND) lParam == GetDlgItem(hMainDlg, IDD_VM_SPIN))
			{
				int iPos = (int) HIWORD(wParam);
				if (SendDlgItemMessage(hMainDlg,IDD_5CHAN,BM_GETCHECK,0,0L)) 
					SetDlgItemText(hMainDlg,IDD_VM_TEXT,vmnames5[iPos % 5]);
				else
					SetDlgItemText(hMainDlg,IDD_VM_TEXT,vmnames[iPos % 5]);
			}
			else if ((HWND) lParam == GetDlgItem(hMainDlg, IDD_IM_SPIN))
			{
				int iPos = (int) HIWORD(wParam);
				if (SendDlgItemMessage(hMainDlg,IDD_5CHAN,BM_GETCHECK,0,0L)) 
					SetDlgItemText(hMainDlg,IDD_IM_TEXT,imnames5[iPos % 6]);
				else
					SetDlgItemText(hMainDlg,IDD_IM_TEXT,imnames[iPos % 6]);
			}
			else if ((HWND) lParam == GetDlgItem(hMainDlg, IDD_MOD_SPIN))
				// Update the rest of the stuff
				SendMessage(hMainDlg, CAL_SETUP_N, GetModuleNumber(hMainDlg), 0L);
		}
		break;

	case CAL_SETUP_BITS:
		DBGOUT("Setting up type bits. lParam: %u.\n", lParam);
		SendDlgItemMessage(hMainDlg,IDD_SRGCPL_TYPE , BM_SETCHECK,(lParam & TYPE_CPLSRG)!=0,0);
		SendDlgItemMessage(hMainDlg,IDD_SURGE_TYPE  , BM_SETCHECK,(lParam & TYPE_SRG)!=0,   0);

		SendDlgItemMessage(hMainDlg,IDD_EFT_CPL      ,BM_SETCHECK,(lParam & TYPE_CPLEFT)!=0,0);
		SendDlgItemMessage(hMainDlg,IDD_EFT_TYPE     ,BM_SETCHECK,(lParam & TYPE_EFT)!=0,   0);

		SendDlgItemMessage(hMainDlg,IDD_ACCPL_TYPE,   BM_SETCHECK,(lParam & TYPE_CPLAC)!=0,0);
		SendDlgItemMessage(hMainDlg,IDD_PQF_TYPE     ,BM_SETCHECK,(lParam & TYPE_PQF)!=0,   0);

		SendDlgItemMessage(hMainDlg,IDD_HVCPL_TYPE,   BM_SETCHECK,(lParam & TYPE_CPLHV)!=0,0);
		SendDlgItemMessage(hMainDlg,IDD_POWER_TYPE   ,BM_SETCHECK,(lParam & TYPE_POWER)!=0, 0);

		SendDlgItemMessage(hMainDlg,IDD_DATACPL_TYPE, BM_SETCHECK,(lParam & TYPE_CPLIO)!=0,0);
		SendDlgItemMessage(hMainDlg,IDD_AMP_TYPE     ,BM_SETCHECK,(lParam & TYPE_AMP)!=0,   0);

		SendDlgItemMessage(hMainDlg,IDD_THREE_PHASE  ,BM_SETCHECK,(lParam & TYPE_CPLTRI)!=0,0);
		SendDlgItemMessage(hMainDlg,IDD_IMPED_TYPE	 , BM_SETCHECK,(lParam & TYPE_IMPED)!=0,0);

		SendDlgItemMessage(hMainDlg,IDD_CLAMP_TYPE  , BM_SETCHECK,(lParam & TYPE_CLAMP)!=0,0);
		SendDlgItemMessage(hMainDlg,IDD_ESD_TYPE	 , BM_SETCHECK,(lParam & TYPE_ESD)!=0,0);

		_ultoa(lParam,buff,10);
		SetDlgItemText(hMainDlg,IDD_TYPE_TEXT,buff);
		break;

	case WM_COMMAND:
		switch (LOWORD(wParam))
		{ 
		// Type checkboxes in order of "appearance" (14 total)
		case IDD_SRGCPL_TYPE:		case IDD_SURGE_TYPE:			
		case IDD_EFT_CPL:			case IDD_EFT_TYPE:
		case IDD_ACCPL_TYPE:		case IDD_PQF_TYPE:
		case IDD_HVCPL_TYPE:		case IDD_POWER_TYPE:
		case IDD_DATACPL_TYPE:		case IDD_AMP_TYPE:
		case IDD_THREE_PHASE:		case IDD_IMPED_TYPE:	 
		case IDD_CLAMP_TYPE:		case IDD_ESD_TYPE:	 

			l=0;
			if( SendDlgItemMessage(hMainDlg,IDD_SRGCPL_TYPE,BM_GETCHECK,0,0L))  l|=TYPE_CPLSRG;
			if( SendDlgItemMessage(hMainDlg,IDD_SURGE_TYPE,BM_GETCHECK,0,0L))	l|=TYPE_SRG;

			if( SendDlgItemMessage(hMainDlg,IDD_EFT_CPL,BM_GETCHECK,0,0L))		l|=TYPE_CPLEFT;
			if( SendDlgItemMessage(hMainDlg,IDD_EFT_TYPE,BM_GETCHECK,0,0L))		l|=TYPE_EFT;

			if( SendDlgItemMessage(hMainDlg,IDD_ACCPL_TYPE,BM_GETCHECK,0,0L))   l|=TYPE_CPLAC;
			if( SendDlgItemMessage(hMainDlg,IDD_PQF_TYPE,BM_GETCHECK,0,0L))		l|=TYPE_PQF;

			if( SendDlgItemMessage(hMainDlg,IDD_HVCPL_TYPE,BM_GETCHECK,0,0L))   l|=TYPE_CPLHV;
			if( SendDlgItemMessage(hMainDlg,IDD_POWER_TYPE,BM_GETCHECK,0,0L))	l|=TYPE_POWER;

			if( SendDlgItemMessage(hMainDlg,IDD_DATACPL_TYPE,BM_GETCHECK,0,0L)) l|=TYPE_CPLIO;
			if( SendDlgItemMessage(hMainDlg,IDD_AMP_TYPE,BM_GETCHECK,0,0L))		l|=TYPE_AMP;

			if( SendDlgItemMessage(hMainDlg,IDD_THREE_PHASE ,BM_GETCHECK,0,0L)) l|=TYPE_CPLTRI;
			if( SendDlgItemMessage(hMainDlg,IDD_IMPED_TYPE,BM_GETCHECK,0,0L))   l|=TYPE_IMPED;

			if( SendDlgItemMessage(hMainDlg,IDD_CLAMP_TYPE,BM_GETCHECK,0,0L))   l|=TYPE_CLAMP;
			if( SendDlgItemMessage(hMainDlg,IDD_ESD_TYPE,BM_GETCHECK,0,0L))     l|=TYPE_ESD;

			_ultoa(l,buff,10);
			SetDlgItemText(hMainDlg,IDD_TYPE_TEXT,buff);
			break;

		case IDCANCEL: 
			EndDialog(hMainDlg, FALSE);
			break;

		case IDD_TRY:
			i = GetModuleNumber(hMainDlg);
			old_cursor=SetCursor(LoadCursor(NULL,IDC_WAIT));
			calset[i].id = GetDlgItemInt(hMainDlg, IDD_ID_EDIT, &error, 0);
			GetDlgItemText(hMainDlg, IDD_TYPE_TEXT, buff, 40);
			calset[i].type = atol(buff);
			GetDlgItemText(hMainDlg, IDD_SERN_EDIT, buff, 40);
			calset[i].serial = atol(buff);

			l = 0;
			if( SendDlgItemMessage(hMainDlg,IDD_EFT2M ,   BM_GETCHECK,0,0L)) l|=EFT_2M_BIT;
			if( SendDlgItemMessage(hMainDlg,IDD_EFT_8KV , BM_GETCHECK,0,0L)) l|=EFT_8K_BIT;
			if( SendDlgItemMessage(hMainDlg,IDD_EFT_CHIRP,BM_GETCHECK,0,0L)) l|=EFT_CHIRP_BIT;
			if( SendDlgItemMessage(hMainDlg,IDD_SRGDC ,   BM_GETCHECK,0,0L)) l|=DC_SURGE_BIT;
			if( SendDlgItemMessage(hMainDlg,IDD_SW_E502,  BM_GETCHECK,0,0L)) l|=E502_SW_BIT;
			if( SendDlgItemMessage(hMainDlg,IDD_ACOPT ,   BM_GETCHECK,0,0L)) l|=AC_OPT_BIT;

			if( SendDlgItemMessage(hMainDlg,IDD_5CHAN,    BM_GETCHECK,0,0L)) l|=INSTR_5CH_BIT;
			if( SendDlgItemMessage(hMainDlg,IDD_EHVSUPPORT,BM_GETCHECK,0,0L))l|=EHV_BIT;

			calset[i].options=l;

			GetDlgItemText(hMainDlg,IDD_NAME_COMBO,(LPSTR)calset[i].name,8);

			j=LOWORD(SendDlgItemMessage(hMainDlg,IDD_VM_SPIN,UDM_GETPOS,0,0L));
			calset[i].vmeas=j % 5;

			j=LOWORD(SendDlgItemMessage(hMainDlg,IDD_IM_SPIN,UDM_GETPOS,0,0L));
			calset[i].imeas=j % 6;

			if (!(calset[i].options & INSTR_5CH_BIT))
			{
				if (calset[i].vmeas > 2) calset[i].vmeas = 0;	 // none
				if (calset[i].imeas > 3) calset[i].imeas = 0;	 // none
			}

			for (j=0; j<CALDATASIZE; j++)
			{
				ui=SendDlgItemMessage(hMainDlg,IDD_CAL_LIST,LB_GETITEMDATA,j+1,0L);
				calset[i].cal[j]=ui;
			}

			if (!SIMULATION)
			{
				PrintfECAT(0,"BAY:ID %d %d",i,calset[i].id);          ECAT_sync();
				PrintfECAT(0,"BAY:SERIAL %d %.7ld",i,calset[i].serial); ECAT_sync();
				PrintfECAT(0,"BAY:OPTION %d %ld",i,calset[i].options);ECAT_sync();
				PrintfECAT(0,"BAY:TYPE %d %ld 0",i,calset[i].type);   ECAT_sync();
				PrintfECAT(0,"BAY:NAME %d '%s'",i,calset[i].name);    ECAT_sync();
				PrintfECAT(0,"BAY:MEAS %d %d",i,(int)(calset[i].vmeas*16+calset[i].imeas ));
				ECAT_sync();

				for (j=0; j<CALDATASIZE; j++)
				{
					PrintfECAT(0,"BAY:CAL %d %d %d",i,j,calset[i].cal[j]);
					ECAT_sync();
				}
			}
			SetCursor(old_cursor);
			break;

		case IDD_SAVE_CFG:
			SendMessage(hMainDlg,WM_COMMAND,IDD_SET_CAL,0L);
			Cal_Write();
			break;

		case IDD_SET_CAL:
			ui=GetDlgItemInt(hMainDlg,IDD_CAL_EDIT,&error,0);
			j=SendDlgItemMessage(hMainDlg,IDD_CAL_LIST,LB_GETCURSEL,0,0L);
			if (j < 1 || j > CALDATASIZE)
			{
				SendDlgItemMessage(hMainDlg,IDD_CAL_LIST,LB_SETCURSEL,-1,0L);
				break;
			}

			if (ui!=0xffff)
				sprintf(buff,"CAL[%2d]=%u",j,ui);
			else 
				sprintf(buff,"CAL[%2d]",j);

			SendDlgItemMessage(hMainDlg,IDD_CAL_LIST,LB_DELETESTRING,j,0L);
			SendDlgItemMessage(hMainDlg,IDD_CAL_LIST,LB_INSERTSTRING,j,(LPARAM)(LPSTR)buff);
			SendDlgItemMessage(hMainDlg,IDD_CAL_LIST,LB_SETITEMDATA,j,ui);
			if (j < CALDATASIZE)
			{
				SendDlgItemMessage(hMainDlg,IDD_CAL_LIST,LB_SETCURSEL,++j,0L);
				ui=SendDlgItemMessage(hMainDlg,IDD_CAL_LIST,LB_GETITEMDATA,j,0L);
				if (ui!=0xffff) 
					SetDlgItemInt(hMainDlg,IDD_CAL_EDIT, ui,0);
				else	
					SetDlgItemText(hMainDlg,IDD_CAL_EDIT,"");
				SetFocus(GetDlgItem(hMainDlg,IDD_CAL_EDIT));
			}
			break;

		case IDD_CAL_LIST:
			if (HIWORD(wParam)==LBN_SELCHANGE)
			{
				j=SendDlgItemMessage(hMainDlg,IDD_CAL_LIST,LB_GETCURSEL,0,0L);
				SendDlgItemMessage(hMainDlg,IDD_CAL_LIST,LB_SETCURSEL,-1,0L);

				if (j < 1 || j > CALDATASIZE) 
					break;
				ui=SendDlgItemMessage(hMainDlg,IDD_CAL_LIST,LB_GETITEMDATA,j,0L);
				SendDlgItemMessage(hMainDlg,IDD_CAL_LIST,LB_SETCURSEL,j,0L);
				if (ui!=0xffff) 
					SetDlgItemInt(hMainDlg,IDD_CAL_EDIT, ui,0);
				else	
					SetDlgItemText(hMainDlg,IDD_CAL_EDIT,"");
				SetFocus(GetDlgItem(hMainDlg,IDD_CAL_EDIT));
			}
			break;

		case IDD_NAME_COMBO:
			if (HIWORD(wParam)==CBN_SELCHANGE)
			{
				i= SendDlgItemMessage(hMainDlg,IDD_NAME_COMBO,CB_GETCURSEL,0,0L);
				if (i==CB_ERR) break;

				SendDlgItemMessage(hMainDlg,IDD_NAME_COMBO,CB_GETLBTEXT,i,(LPARAM)(LPSTR)buff);
				for (i=0; ((::fill[i].type_mod)&& (strcmp(buff,::fill[i].name)!=0)); i++);

				if (::fill[i].type_mod)
				{
					SetDlgItemInt(hMainDlg,IDD_ID_EDIT,::fill[i].id,0);
					SendMessage(hMainDlg,CAL_SETUP_BITS,0,::fill[i].type_mod);
				}
			}
			break;
		
		case IDD_LEFT_MOD:
		case IDD_RIGHT_MOD:
			SendMessage(hMainDlg, CAL_SETUP_N, GetModuleNumber(hMainDlg), 0L);
			break;


		case IDD_SET:
			old_cursor=SetCursor(LoadCursor(NULL,IDC_WAIT));
			SendMessage(hMainDlg,WM_COMMAND,IDD_TRY,0L);
			if (!SIMULATION)
			{
				i = GetModuleNumber(hMainDlg);
				j = QueryPrintfEcatInt(error,"BAY:UP %d",i);
				if (error | j)
				if (-j >= 1 && -j < (sizeof(ErrorEeprom) / sizeof (char *)))
					MessageBox(NULL,ErrorEeprom[-j],"Serial EEPROM Write Error",MB_SYSTEMMODAL);
				else
					MessageBox(NULL,ErrorEeprom[0],"Serial EEPROM Error",MB_SYSTEMMODAL);
			}
			SetCursor(old_cursor);
			break;

		} // switch (wParam)
		break; //WM_COMMAND

	} // switch (message)

	return (FALSE);
}

void show_cal(HINSTANCE ghinst, HWND hMainDlg)
{
	 DialogBox(ghinst, MAKEINTRESOURCE(IDD_CALIBRATE), hMainDlg, (DLGPROC)CalProc);
}

void Get_Cal_Record(int i)
{
	char rxbuff[500];
	int j;
	char oc;
	HCURSOR old_cursor;
	if (SIMULATION) 
	{ 
		calset[i].cal_valid = 1; 
		return;
	}

	old_cursor = SetCursor(LoadCursor(NULL,IDC_WAIT));
	QueryPrintfEcat(rxbuff, 499, "BAY:CAL? %d -1", i);
	j = _fstrlen(rxbuff);						//************
	for (j=0; j<CALDATASIZE; j++)					  
	{
		oc=rxbuff[(j+1)*4];
		rxbuff[(j+1)*4]=0;
		sscanf(&rxbuff[j*4],"%hX",&calset[i].cal[j]);
		rxbuff[(j+1)*4]=oc;
	}
	calset[i].cal_valid = 1;
	SetCursor(old_cursor);
}

void Get_Record(int i)
{ 
	char rxbuff[100];
	BYTE data[32];
	int  id,err,j;
	char oc;

	id=QueryPrintfEcatInt(err,"BAY:ID? %d",i);

	if (id == 255)
		err=1;

	if (err)
		InitGhostBox(i);
	else
	{
		calset[i].id=id;
		QueryPrintfEcat(rxbuff,99,"BAY:DU? %d",i);
		j =_fstrlen(rxbuff);					//*****
		for (j=0; j<32; j++)					  
		{
            unsigned int datum;
			oc=rxbuff[(j+1)*2];
			rxbuff[(j+1)*2]=0;
			sscanf(&rxbuff[j*2],"%X",&datum);
            data[j] = (BYTE)datum;
			rxbuff[(j+1)*2]=oc;
		}
	
		DECODE_STRUCT *dcp=(DECODE_STRUCT*)data;
		dcp->mod_name[MODNAMESIZE-1]=0;
		strcpy(calset[i].name,dcp->mod_name);

		calset[i].id      = dcp->modid;
		calset[i].vmeas   = (dcp->vi_byte /16) & 15;
		calset[i].imeas   = dcp->vi_byte & 15;

		DECODE_STRUCT_SRC *dcps=(DECODE_STRUCT_SRC*)rxbuff;

		oc = dcps->mod_type[8];
		dcps->mod_type[8] = 0;
		sscanf(dcps->mod_type,"%lX",&calset[i].type);
		dcps->mod_type[8] = oc;

		oc = dcps->options[8];
		dcps->options[8] = 0;
		sscanf(dcps->options,"%lX",&calset[i].options);
		dcps->options[8] = oc;

		oc = dcps->serial[8];
		dcps->serial[8] = 0;
		sscanf(dcps->serial,"%lX",&calset[i].serial);
		dcps->serial[8] = oc;
	}

	calset[i].cal_valid  =0;
}

static LPCSTR GetCalFilePath()
{
	static char lpszCalPath[MAX_PATH + 1] = "";
	if (lpszCalPath[0] == '\0')
        PathCombine(lpszCalPath, lpszCalPath,  get_app_data_file_path(CAL_FILE_NAME));

	return lpszCalPath;
}

void Cal_Write()
{  
	DWORD dwByteCnt;
	HANDLE hFile = CreateFile(
					GetCalFilePath(),
					GENERIC_WRITE,			// open to write
					0,						// deny shared access
					NULL,					// no security
					CREATE_ALWAYS,			// always overwrite
					FILE_ATTRIBUTE_NORMAL,
					NULL);
	
	if (hFile == NULL)
		goto OnError;

	// ECAT calibration file cookie (ECAL)
	dwByteCnt = sizeof(CAL_FILE_COOKIE);
	if (!WriteFile(hFile, &CAL_FILE_COOKIE, dwByteCnt, &dwByteCnt, NULL))
		goto OnError;

	// ECAT calibration file version
	dwByteCnt = sizeof(CAL_FILE_VERSION);
	if (!WriteFile(hFile, &CAL_FILE_VERSION, dwByteCnt, &dwByteCnt, NULL))
		goto OnError;

	// now the data
	dwByteCnt = sizeof(calset);
	if (!WriteFile(hFile, calset, dwByteCnt, &dwByteCnt, NULL))
		goto OnError;

	FlushFileBuffers(hFile);
	CloseHandle(hFile);
	if (SIMULATION)
		MessageBox(NULL, "Calibration data was successfully saved into 'SIM.CFG' file.\nYou'll need to restart the program for the changes to take effect.", "Calibration File", MB_OK | MB_ICONINFORMATION);
	return;

OnError:
	if (hFile != NULL)
		CloseHandle(hFile);
	MessageBox(NULL, "ERROR: Unable to write calibration data to 'SIM.CFG'.", "Calibration File", MB_OK | MB_ICONEXCLAMATION);
}

static void ShowReadErr(HANDLE hFile)
{
	if (hFile != NULL)
		CloseHandle(hFile);

	MessageBox(NULL, "ERROR: Unable to open/read 'SIM.CFG'. Simulation data for an ECAT system is unavailable.", "Calibration File", MB_OK | MB_ICONEXCLAMATION);
	ResetCalData(TRUE);
	calsetup = 1;
}

void Cal_Read()
{  
	int i;
	int coupler_count;
	int eft_coupler_count;
	int eft_module_count;
	HCURSOR old_cursor;
	if (calsetup) return;
	DWORD dwFileVer = 0;

	EFT_EXIST			=FALSE;
	bIsEP3				=FALSE;
	SURGE_EXIST			=FALSE;
	EFT_THREE_PHASE		=FALSE;
	SURGE_THREE_PHASE	=FALSE;
	EFT_2MHZ			=FALSE;
	EFT_8KV				=FALSE;
	EFT_CHIRP			=FALSE;
	DC_SURGE			=FALSE;
	IS_CE_BOX			=FALSE;
	CE_EP_OPTION		=FALSE;

	EFT_COUPLER			=-1;
	EFT_MODULE			=-1;
	SURGE_COUPLER		=-1;
	IO_COUPLER 			=-1;
	AC_COUPLER 	   		=-1;

	PQF_EXIST			=FALSE;
	PQF_AUX				=FALSE;

	coupler_count		=0;
	eft_coupler_count	=0;
	eft_module_count	=0;

	ResetCalData(SIMULATION);

	if (SIMULATION)
	{
		HANDLE hFile = CreateFile(
						GetCalFilePath(),
						GENERIC_READ,			// open for read
						0,						// deny shared access
						NULL,					// no security
						OPEN_EXISTING,			// should exist
						FILE_ATTRIBUTE_NORMAL,
						NULL);
		if (hFile == NULL)
		{
			ShowReadErr(hFile);
			return;
		}

		// ECAT calibration file cookie (ECAL)
		DWORD dwByteCnt = sizeof(CAL_FILE_COOKIE);
		DWORD dwTemp;
		if (!ReadFile(hFile, &dwTemp, dwByteCnt, &dwByteCnt, NULL))
		{
			ShowReadErr(hFile);
			return;
		}

		// See if it's a cookie (ECAL)
		if (dwTemp != CAL_FILE_COOKIE)
		{
			// Old version?
			dwByteCnt = sizeof(calset);
			if ((SetFilePointer(hFile, 0, NULL, FILE_BEGIN) == (DWORD) -1) ||
				!ReadFile(hFile, calset, dwByteCnt, &dwByteCnt, NULL))
			{
				ShowReadErr(hFile);
				return;
			}
		}
		else
		{
			// ECAT calibration file version
			dwByteCnt = sizeof(dwFileVer);
			if (!ReadFile(hFile, &dwFileVer, dwByteCnt, &dwByteCnt, NULL))
			{
				ShowReadErr(hFile);
				return;
			}

			// now the data
			dwByteCnt = sizeof(calset);
			if (!ReadFile(hFile, calset, dwByteCnt, &dwByteCnt, NULL))
			{
				ShowReadErr(hFile);
				return;
			}
		}

		for (int i = 0; i < MAX_CAL_SETS; i++)
		{
			char *c = strchr(calset[i].name, ' ');
			if (c) *c = 0;
		}
		CloseHandle(hFile);
	}
	else // Not Simulation
	{
		//do real stuff here
		bIsEP3 = FALSE;
		old_cursor=SetCursor(LoadCursor(NULL,IDC_WAIT));
		for (i=0; i<MAX_CAL_SETS; i++)
		{
			Get_Record(i);
			if (calset[i].options & AC_OPT_BIT)
				Get_Cal_Record(i);
		}
		SetCursor(old_cursor);
	}

	for (i=0; i<MAX_CAL_SETS; i++)
	{
		if (calset[i].type & TYPE_EFT)
		{
			EFT_EXIST=TRUE; 
			EFT_MODULE=i; 
			eft_module_count++;

			if (calset[i].options & EFT_2M_BIT)		EFT_2MHZ =TRUE;
			if (calset[i].options & EFT_8K_BIT)		EFT_8KV  =TRUE;
			if (calset[i].options & EFT_CHIRP_BIT)	EFT_CHIRP=TRUE;
		}

		if ((calset[i].type & TYPE_CPLEFT) && (calset[i].type & TYPE_CPLAC))
		{ 
			EFT_COUPLER=i;
			eft_coupler_count++;
			if (calset[i].options & DC_SURGE_BIT)	DC_EFT =TRUE;
			if (calset[i].type & TYPE_CPLTRI)		EFT_THREE_PHASE =TRUE;
		}

		if (calset[i].type & TYPE_CPLIO)
            IO_COUPLER=i;
		if (calset[i].type & TYPE_SRG)
            SURGE_EXIST=TRUE;
		if ((calset[i].type & TYPE_CPLSRG ) && (calset[i].type & TYPE_CPLAC))
		{
			SURGE_COUPLER=i;
			coupler_count++;
			if (calset[i].options & DC_SURGE_BIT)	DC_SURGE =TRUE;
			if (calset[i].type & TYPE_CPLTRI)		SURGE_THREE_PHASE =TRUE;
		}

		if (calset[i].type & TYPE_PQF) 
		{
			PQF_EXIST=TRUE;
			if (calset[i].options && PQF_AUX_BIT)
				PQF_AUX=TRUE;
		
			if (calset[i].id == EP3_BOXT)
				bIsEP3 = TRUE;
		}

		// detect CE modules and set CE_BOX flag if have one; is also set
		// in main dlg proc by hardware type detection.

		if (calset[i].type & TYPE_CE)
		{
			IS_CE_BOX = TRUE;

			// Use AC Option bit to test for "EP option" to control line sync
			if (calset[i].options & AC_OPT_BIT)
				CE_EP_OPTION = TRUE;
			else
				CE_EP_OPTION = FALSE;
		}
	} // for

	if (coupler_count>1) 		SURGE_COUPLER = 99;
	if (eft_coupler_count>1)	EFT_COUPLER   = 99;
	if (eft_module_count>1)		EFT_MODULE    = 99;
}


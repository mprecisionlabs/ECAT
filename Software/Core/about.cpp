#include "stdafx.h"
#include "calinfo.h"
#include "dlgctl.h"
#include "kt_comm.h"
#include "globals.h"

static HFONT hFixedFont = NULL;

static void AddLine2EditBox(HWND hEdit, LPCTSTR lpszLine)
{
	if (!hEdit) return;
	if ((lpszLine == NULL) || (*lpszLine == '\0'))
		return;

	int iLastLine = SendMessage(hEdit, EM_GETLINECOUNT, 0, 0) - 1;
	int iLineIndx = SendMessage(hEdit, EM_LINEINDEX, (WPARAM) iLastLine, 0);
	int iLastChar = SendMessage(hEdit, EM_LINELENGTH, (WPARAM) iLastLine, 0) + ((iLineIndx == -1) ? 0 : iLineIndx);
	SendMessage(hEdit, EM_SETSEL, (WPARAM) iLastChar, (LPARAM) iLastChar);
	SendMessage(hEdit, EM_REPLACESEL, (WPARAM) FALSE, (LPARAM) lpszLine);
}

BOOL CALLBACK AboutProc(HWND hMainDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	char BayName[128];
	int i;
	HWND hEdit = NULL;
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
		SendDlgItemMessage(hMainDlg,IDD_ABT_LIST,WM_SETFONT,(WPARAM) hFixedFont,0L);
		SetDlgItemText(hMainDlg,IDD_ABT_LABEL,(LPSTR)lParam);
		if (!SIMULATION)
		{  
			kt_device_info dvcInfo;
			if (SendMessage(hWndComm, KT_DEVICE_INFO, 0, (LPARAM) &dvcInfo))
			{
				PrintfDlg(hMainDlg, IDD_ABT_NAME, "KeyTek Device: %Fs", dvcInfo.machine_type);
				PrintfDlg(hMainDlg, IDD_ABT_SN,	"Serial Number: %lu", dvcInfo.serial_number);
				PrintfDlg(hMainDlg, IDD_ABT_REV, "Firmware Rev.:  %Fs", dvcInfo.firmware);
			}
		}
		else 
		{
			// fake the serial number etc.
			PrintfDlg(hMainDlg, IDD_ABT_SN,	" ");
			PrintfDlg(hMainDlg, IDD_ABT_REV, " ");
			PrintfDlg(hMainDlg, IDD_ABT_NAME, "Simulation Mode");
		}//SIMULATION
		
		hEdit = GetDlgItem(hMainDlg, IDD_ABT_LIST);
		SendDlgItemMessage(hMainDlg, IDD_ABT_LIST, LB_RESETCONTENT,	0, 0L);
		for (i=0; i<MAX_CAL_SETS; i++)
		{
			if (NOT_GHOST(i))
			{
				Get_Calinfo_Name(i,BayName);
				AddLine2EditBox(hEdit, BayName);
				if (BayName[0] != '\0')
					AddLine2EditBox(hEdit, "\r\n");
			}
		}//FOR
		return (TRUE);

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK)
		{
			if (hFixedFont != NULL)
			{
				DeleteObject(hFixedFont);
				hFixedFont = NULL;
			}

			EndDialog(hMainDlg, FALSE);
			return (TRUE);
		}

	default:
	   break;
	}
	
	return (FALSE);
}	

void show_about(HINSTANCE ghinst, HWND hMainDlg,LPSTR name)
{
	DialogBoxParam(ghinst, MAKEINTRESOURCE(IDD_ABOUT), hMainDlg, AboutProc,(LPARAM)(LPSTR)name);
}

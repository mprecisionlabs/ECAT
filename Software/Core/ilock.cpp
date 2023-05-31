/*********************************************************************

ILOCK.CPP -- Interlock handling functions, including interlock dialog
				and interlock messages.

HISTORY:
			4.00a version used as basis. 
*********************************************************************/
#include "stdafx.h"
#include "comm.h"
#include "dlgctl.h"
#include "globals.h"
#include "kt_comm.h"
#include "message.h"

ILOCK::ILOCK()
{
	lpfnIlock=NULL;
	IlockWnd=NULL;
	Interlock_Showing=FALSE;
	Created=FALSE;
}

ILOCK::~ILOCK()
{ 
	if (IlockWnd)
	{
		DestroyWindow(IlockWnd);
		IlockWnd=NULL;
	}
	
	lpfnIlock=NULL;
}
	
void ILOCK::Init(HWND Parent)
{
	hParrent=Parent;
}

BOOL CALLBACK EnumThreadWndProc(
				HWND hwnd,      // handle to a window
				LPARAM lParam   // application-defined value (*BOOL)
)
{
	BOOL bClosed = FALSE;
	if (SendMessage(hwnd, KT_ILOCK_ABORT_RUN, 0, (LPARAM) &bClosed) && bClosed)
		// our window, return FALSSE to stop enumeration
		return FALSE;

	return TRUE;	//continue enumeration
}


BOOL __declspec(dllexport) CALLBACK Ilock_Proc(HWND	hMainDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
	{
	case WM_INITDIALOG:
		{
			// Set the icon
			HICON hIcon = LoadIcon(NULL, IDI_ERROR);
			SendDlgItemMessage(hMainDlg, IDC_ILOCK_ICON, STM_SETICON, (WPARAM) hIcon, 0);
		}
		break;

	case WM_COMMAND	:
	    switch (LOWORD(wParam))
		{
		case IDABORT:
			{
				HWND hParent = GetParent(hMainDlg);

				/* If the Run Pause dialog is active (this is the case if the user
				pushes STOP and gets the Run Pause dialog, and then an interlock
				error occurs), KT_ILOCK_ABORT_RUN will cause that dialog to close. */
				EnumThreadWindows(GetCurrentThreadId(), EnumThreadWndProc, NULL);
				
				SendMessage(hParent, WM_COMMAND, CMD_ABORT_ILOCK, 0L);
				Log.logprintf("Operator Aborts from Interlock\r\n");
			}
		    break;

		case IDRETRY :
			ShowWindow(hMainDlg, SW_HIDE);
			Log.logprintf("Operator Retries from Interlock\r\n");
			Ilock.Show(FALSE);
			SendMessage(GetParent(hMainDlg), WM_COMMAND, CMD_PAUSE, 0L);
		    break;
		}
	    break;
	}
    return FALSE;
}

void ILOCK::Show(BOOL on)
{
	if (on)
	{
		//Show
		if (!Created)
		{	
			// prepare/create the interlock dialog box but do not show it.
			lpfnIlock =	(FARPROC) Ilock_Proc;
			IlockWnd = CreateDialog(ghinst, MAKEINTRESOURCE(IDD_INTERLOCK), hParrent, (DLGPROC)lpfnIlock);
			hParrent = GetParent(IlockWnd);
			Created=TRUE;
		}
		ShowWindow(IlockWnd, SW_SHOWNORMAL);
		EnableWindow(hParrent, FALSE);
	}
	else
	{
		//Hide 
		ShowWindow(IlockWnd, SW_HIDE);
		EnableWindow(hParrent, TRUE);
		SetForegroundWindow(hParrent);
		UpdateWindow(hParrent);
	}
}

void ILOCK::Check_Interlocks( long status)
{
	const int INFO_BUF_LEN = 2048;
	char lpszInfo[INFO_BUF_LEN];
	memset(lpszInfo, 0, sizeof(lpszInfo));
	BOOL Allow_retry;

	if (status)
	{
		Allow_retry=FALSE;

		switch (status)
		{
		case ILOCK_NOCOMM:
		{
			strcpy(lpszInfo,
					"The ECAT system is not communicating.\r\n"
					"If this condition persists, cycle the power to the ECAT unit.\r\n"
					"Please check communications connections or press Abort button.");
			RESET_REQUIRED=TRUE;
			Allow_retry=1;
		}
		break;

		case ILOCK_NO_EUT_POW:
		{
			Show(1);
			strcpy(lpszInfo,
					"ECAT EUT power is not present and a SURGE based on line phase has been requested.\r\n"
					"Please restore AC power or press Abort button.");
			Allow_retry=1;
		}
		break;
		
		case ILOCK_NO_DC_POW:
		{
			Show(1);
			strcpy(lpszInfo,
					"ECAT EUT DC power is not present and a DC Source SURGE has been requested.\r\n"
					"Please restore DC power or press Abort button.");
			Allow_retry=1;
		}
		break;

		case ILOCK_NO_HV_POW:
		{
			Show(1);
			strcpy(lpszInfo,
					"The E522 HV supply is not operating correctly.\r\n"
					"Please check the E522 AC power.");
			Allow_retry=0;
		}
		break;


		case ILOCK_NO_EUT_BLUE:
		{
			Show(1);
			strcpy(lpszInfo,
					"ECAT EUT power is not enabled.\r\n"
					"Please Cycle the EUT power button and try again, or press Abort button.");
			Allow_retry=1;
		}
		break;

		case ILOCK_ANALOG_FAILED:
		{
			Show(1);
			strcpy(lpszInfo,
					"Analog Readback Failed.\r\n"
					"This is an internal failure, please cycle system power and contact KeyTek.");
			Allow_retry=1;
		}
		break;
		
		case ILOCK_CHG_NREADY:   
		{
			Show(1);
			strcpy(lpszInfo,
				"Not Ready Error.\r\n"
				"The Unit was not ready to cycle again, please consult your manual for the minimum cycle time.");
			RESET_REQUIRED=TRUE;
			Allow_retry=1;
		}
		break;

		case ILOCK_INTERNAL:
		case ILOCK_SRG_ILOCK:    
		case ILOCK_SRG_BAD_NET:    
		case ILOCK_NO_SRG_IDLE:    
		{
			Show(1);
			strcpy(lpszInfo,
				"Internal Error.\r\n"
				"This is an internal failure, please cycle system power and contact KeyTek.");
			RESET_REQUIRED=TRUE;
			Allow_retry=1;
		}
		break;
		
		case ILOCK_EFT_NOT_BLUE:
		{
			strcpy(lpszInfo,
				"E400 EUT power is not enabled and a BURST with EUT power enabled has been requested.\r\n"
				"Please cycle the EUT power, or press Abort button.");
			Allow_retry=1;
		}
		break;

		case ILOCK_EFT_AC_ERROR:
		{
			strcpy(lpszInfo,
				"E400 EUT power is not present and a BURST based on line phase has been requested.\r\n"
				"Please restore AC power or press Abort button.");
			Allow_retry=1;
		}
		break;

		case ILOCK_EFT_NOT_IDLE:
		{ 
			strcpy(lpszInfo, "EFT Not Idle Error.\r\nUnknown Reason.\r\nConsult Factory.");
			Allow_retry=1;
			RESET_REQUIRED=TRUE;
		}
		break;

		case ILOCK_EFT_NOT_SET:
		{ 
			strcpy(lpszInfo, "EFT Not Set Error.\r\nUnknown Reason.\r\nConsult Factory.");
			Allow_retry=1;
			RESET_REQUIRED=TRUE;
		}
		break;

		case ILOCK_EFT_BAD_MODULE :
		{
			strcpy(lpszInfo, "E400 Bad Module Error.\r\nUnknown Reason.\r\nConsult Factory.");
			Allow_retry=1;
			RESET_REQUIRED=TRUE;
		}
		break;

		case ILOCK_EFT_INTERLOCK:
		case ILOCK_ECAT:
		{
			char tmpBuf[255] = "";
			QueryECAT(0,"SYS:ITEXT?", tmpBuf, sizeof(tmpBuf));
			char *lpTmp = tmpBuf;
			char *lpInfo = lpszInfo;
			BOOL bDiscardVoid = TRUE;
			while (*lpTmp)
			{
				switch (*lpTmp)
				{
				case '\n':
					*lpInfo++ = '\r';
					*lpInfo++ = '\n';
					bDiscardVoid = TRUE;
					break;
				
				case '\r':
					bDiscardVoid = TRUE;
					break;

				case ' ':
					if (!bDiscardVoid)
						*lpInfo++ = *lpTmp;
					break;
				
				default:
					bDiscardVoid = FALSE;
					*lpInfo++ = *lpTmp;
					break;
				}
				lpTmp++;
			}
			
			if (strlen(lpszInfo) > 0)
			{
				Allow_retry=0;
				RESET_REQUIRED=TRUE;
				break;
			}
			// else, fall down, since we don't have any info
		}

		default:
			strcpy(lpszInfo, "Unknown Error.\r\nThis is an unknown failure.\r\nUnknown remedy.");
			RESET_REQUIRED=TRUE;
			Allow_retry=0;
		break;

		}//Case

		Log.logprintf("Interlock:%s\r\n", lpszInfo);
		if (SIMULATION) return;

		Show(1);
		SetDlgItemText(IlockWnd, IDC_EDIT_INFO, lpszInfo);
		
		ShowWindow(IlockWnd, SW_SHOWNORMAL);
		SendMessage(GetParent(IlockWnd), WM_COMMAND, CMD_PAUSE, 1L);
		
		EnableWindow(GetDlgItem(IlockWnd, IDRETRY), Allow_retry);
		Interlock_Showing = TRUE;

	}
	else if (Interlock_Showing)	       
	{
		// window already displayed.
		EnableWindow(GetDlgItem(IlockWnd, IDRETRY), 1); 
		Interlock_Showing = FALSE;
	}
}

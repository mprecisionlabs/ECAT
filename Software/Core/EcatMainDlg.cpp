// EcatMainDlg.cpp: implementation of the CEcatMainDlg class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "EcatApp.h"
#include "EcatMainDlg.h"
#include "globals.h"
#include "kt_comm.h"
#include "comm.h"
#include "message.h"
#include "filesdlg.h"
#include "module.h"

#define MIN_ECAT_FW_REV (5.00)
#define MIN_CE_FW_REV (1.00)
#ifndef REVSTR
#define REVSTR ""
#endif

#define STR_TEST_MOD_TEXT		"Test setup has changed since last save.\nLoading new file will cause you to lose those changes."
#define STR_TEST_MOD_TITLE		"Test Not Saved."

//////////////////////////////////////////////////////////////////////
// Global ECat dialog procedure
//////////////////////////////////////////////////////////////////////
static LRESULT CALLBACK EcatDlgProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	// Set the pointer to the calling CEcatMainDlg if it's WM_INITDIALOG, 
	// otherwise extract the pointer from GWL_USERDATA
	CEcatMainDlg* lpWnd = NULL;
	if (msg == WM_INITDIALOG)
	{
		if (lParam != NULL)
		{
			lpWnd = (CEcatMainDlg*) lParam; 
			SetWindowLong(hWnd, GWL_USERDATA, (LONG) lpWnd);
		}
	}
	else
		lpWnd = (CEcatMainDlg*) GetWindowLong(hWnd, GWL_USERDATA);

	if (lpWnd != NULL) 
		return (lpWnd->LocalDlgProc(hWnd, msg, wParam, lParam));
	else
		return DefWindowProc(hWnd, msg, wParam, lParam);
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CEcatMainDlg::CEcatMainDlg()
{
	m_hWnd = NULL;
	changed_since_save = FALSE;
	EUT_POWER = 0;
	nRunMode = IDLE_MODE;
	change_shown = 0;
	m_uiTimerID = NULL;
}

//////////////////////////////////////////////////////////////////////
CEcatMainDlg::~CEcatMainDlg()
{
}

//////////////////////////////////////////////////////////////////////
LRESULT CEcatMainDlg::SendItemMsg(UINT uiItemId, UINT uiMsg, WPARAM wParam, LPARAM lParam)
{
	return SendDlgItemMessage(m_hWnd, uiItemId, uiMsg, wParam, lParam);
}

//////////////////////////////////////////////////////////////////////
BOOL CEcatMainDlg::Create(UINT uiResId, HWND hParent)
{
	if ((uiResId == NULL) || (hParent == NULL)) return FALSE;

	// Create the dialog and adjust the parent's size to match that of newly created dialog.
	// Then center the window within the working area of the desktop.
	m_hWnd = CreateDialogParam(GetInst(), MAKEINTRESOURCE(uiResId), hParent, (DLGPROC) EcatDlgProc, (LPARAM) this);
	if (m_hWnd == NULL)
		return FALSE;

	RECT rcDlg;
	GetWindowRect(m_hWnd, &rcDlg);
	MoveWindow(m_hWnd, 0, 0, rcDlg.right - rcDlg.left, rcDlg.bottom - rcDlg.top, FALSE);
	DWORD dwStyle = GetWindowLong(hParent, GWL_STYLE);
	AdjustWindowRect(&rcDlg, dwStyle, TRUE);
	MoveWindow(hParent, 0, 0, rcDlg.right - rcDlg.left, rcDlg.bottom - rcDlg.top, FALSE);

	return TRUE;
}

//////////////////////////////////////////////////////////////////////
// NOTE: Return 0 if the message has been processed, 1 for further 
//		 processing by the system
//////////////////////////////////////////////////////////////////////
int CEcatMainDlg::OnCmdMsg(UINT uiNotify, UINT uiCtrlId, HWND hwndCtl)
{
	switch (uiCtrlId)
	{
	case IDD_ECAT_EXIT:
	case IDCANCEL:
		SendMessage(GetParent(), WM_COMMAND, MAKEWPARAM(uiCtrlId, uiNotify), (LPARAM) hwndCtl);
		return 0;
	
	case CMD_ABORT_ILOCK:
		SendMessage(m_hWnd, WM_COMMAND, IDD_STOP, 0L);
		Ilock.Show(FALSE);
		return 0;

	default:
		return 1;
	}
}

//////////////////////////////////////////////////////////////////////
// NOTE: Return FALSE if focus is set to a control other then the one 
//		 sent in lParam
//////////////////////////////////////////////////////////////////////
BOOL CEcatMainDlg::OnInitDialog(HWND hWnd)
{
	// Create yellow brush
	yBrush = CreateSolidBrush(RGB(255,255,0));
	m_hWnd = hWnd;
	g_hMainDlg = hWnd;		
	HWND hParent = GetParent();

	// Initialize the interlock and hiddent window
	Ilock.Init(hParent);
	Last_State = (LONG) -1;
	hWndComm = init_kt_comm(hWnd, GetInst());
	if (hWndComm == NULL)
	{
		MessageBox(hWnd,
					"ECAT Communications Error",
					"Unable to create the Keytek Communications Window.",
					MB_OK | MB_ICONEXCLAMATION);
		DestroyWindow(hParent);
		return FALSE;
	}
	SendMessage(hWndComm, KT_SYS_STATUS,0, 0L);
	POWER_UP = FALSE;
	Last_State = ILOCK_OK;
	This_State = ECAT_Ilock();

	// Make sure we've got the communication window open and the firmware is OK
	// Check the revision number
	if (!SIMULATION)
	{
		kt_device_info dvcInfo;
		SendMessage(hWndComm, KT_DEVICE_INFO, 0, (LPARAM) &dvcInfo);

		// Detect if running on CExx box
		if (!_fstrnicmp((LPCSTR)&(dvcInfo.machine_type),"CECAT", 5) )
			IS_CE_BOX = TRUE;

		// Test against min. firmware rev. for the product
		if ( (IS_CE_BOX && (dvcInfo.firm_rev<MIN_CE_FW_REV)) ||
			(!IS_CE_BOX && (dvcInfo.firm_rev<MIN_ECAT_FW_REV)) )
		{
			float min_rev = (float)(IS_CE_BOX ? MIN_CE_FW_REV : MIN_ECAT_FW_REV);
			Message("ECAT Firmware Revision Error",
				"The Firmware Rev %7.4f is too low. It must be >= %7.4f.\nUnable to continue.",dvcInfo.firm_rev, min_rev);
			DestroyWindow(hParent);
			return FALSE;
		}
	}

	// Init log file
	Log.log_init(hWnd, (char*) CEcatApp::GetAppName(), get_revision_string());
	PostMessage(hParent, WM_COMMAND, (Log.log_on() ? IDM_LON : IDM_LOFF), 0L);

	return TRUE;
}

//////////////////////////////////////////////////////////////////////
// Empty virtual placeholder for the derived classes' overrides
//////////////////////////////////////////////////////////////////////
void CEcatMainDlg::OnTimer(UINT uiTimerId)
{
}

//////////////////////////////////////////////////////////////////////
LRESULT CALLBACK CEcatMainDlg::LocalDlgProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	// Local window procedure
	switch (msg) 
	{
		case WM_INITDIALOG:
			return OnInitDialog(hWnd);

		case WM_COMMAND:
			if (!OnCmdMsg((UINT) HIWORD(wParam), (UINT) LOWORD(wParam), (HWND) lParam))
				break;
			return DefWindowProc(hWnd, msg, wParam, lParam);

		case WM_DESTROY:
			OnDestroy();
			break;

		case WM_TIMER:
			OnTimer((UINT) wParam);
			break;

		default:
			return DefWindowProc(hWnd, msg, wParam, lParam);
   }

   return 0;
}

//////////////////////////////////////////////////////////////////////
void CEcatMainDlg::RedrawWindow(HWND hWnd)
{
	::RedrawWindow(hWnd, NULL, NULL, RDW_INVALIDATE | RDW_UPDATENOW | RDW_ERASE);
}

//////////////////////////////////////////////////////////////////////
LPCTSTR CEcatMainDlg::GetItemText(UINT uiItemId)
{
	static char st_lpszBuf[MAX_PATH + 1] = "";
	if (GetDlgItemText(m_hWnd, uiItemId, st_lpszBuf, MAX_PATH) == 0)
		st_lpszBuf[0] = '\0';

	return st_lpszBuf;
}

//////////////////////////////////////////////////////////////////////
void CEcatMainDlg::Convert_Seconds(LONG secs,	char *buffer)
{
	LONG s, h, m, d, y;
	char lbuff[10];
	buffer[0] =	0;
	s =	secs % 60;
	m =	secs / 60;
	h =	m / 60;
	d =	h / 24;
	h =	h % 24;
	m =	m % 60;
	y =	d / 365;
	d =	d % 365;
 
    if (y)
	{
		_ltoa(y,	lbuff, 10);
		strcat(buffer, lbuff);
		strcat(buffer, ":Years ");
	}
 
    if (d)
	{
		_ltoa(d,	lbuff, 10);
		strcat(buffer, lbuff);
		strcat(buffer, ":Days ");
	}
 
 
	_ltoa(h, lbuff, 10);
	strcat(buffer, lbuff);
	strcat(buffer, ":");

	_ltoa(m, lbuff, 10);
	if (m < 10)	strcat(buffer, "0");
	strcat(buffer, lbuff);
	strcat(buffer, ".");


	_ltoa(s, lbuff, 10);
	if (s < 10)	strcat(buffer, "0");
	strcat(buffer, lbuff);
}

//////////////////////////////////////////////////////////////////////
void CEcatMainDlg::OnChangeName()
{
	HWND hParent = GetParent();
	Tname.Execute(GetInst(), hParent, MAKEINTRESOURCE(IDD_TEST_NAME));
	DrawMenuBar(hParent);
}

//////////////////////////////////////////////////////////////////////
void CEcatMainDlg::OnDestroy() 
{
	// Stop the timer
	StopTimer();

	// Shutdown polling ecat window
	if ((hWndComm != NULL) && IsWindow(hWndComm))
	{
		SendMessage(hWndComm, KT_ECAT_EXIT, 0, 0L);

		// on a Exiting app or on a GPIB comm error.
		// if the hidden comm window was created, then on exit of app:
		DestroyWindow(hWndComm);		// destroy the keytek hidden comm window.
		hWndComm = NULL;				// clear the handle.
	}

	// delete yellow brush
	if (yBrush != NULL)
	{
		DeleteObject(yBrush);
		yBrush = NULL;
	}

	// set the handle to NULL - not a window anymore
	m_hWnd = NULL;
}

//////////////////////////////////////////////////////////////////////
BOOL CEcatMainDlg::DoExit()
{
	if (nRunMode == IDLE_MODE)
	{
		if (changed_since_save && 
			(MessageBox(NULL,
					"The setup has changed since the last save.\nExiting will cause you to lose those changes.\nContinue?",
					"File Not Saved", MB_YESNO|MB_ICONQUESTION) == IDNO))
					return FALSE;

        if ((EUT_POWER == 2) && (calset[SURGE_COUPLER].id != E505B_BOXT))  //eut power on when exiting check added by Ken Merrithew 5/96
		{
			if (MessageBox(NULL,"\nThe EUT Power is ON.\n\nLeave EUT Power On?","WARNING", MB_YESNO|MB_ICONWARNING) == IDNO)
			{
				EUT_POWER = 0;
				SendMessage(hWndComm, KT_EUT_OFF,0, 0L);
			}
		}  // end of eut power on when exiting check.

		nRunMode = WAIT_MODE;		//jak what else did I miss turning off?
		return TRUE;
	}
	else
		return FALSE;
}

//////////////////////////////////////////////////////////////////////
void CEcatMainDlg::UpdateTitle(LPSTR lpszTitle/* = NULL*/)
{
	// if the string is empty or equal to untitled, then draw it and exit
	HWND hParent = GetParent(); 
	if ((lpszTitle == NULL) || (*lpszTitle == '\0') || (_stricmp(lpszTitle, "untitled") == 0))
	{
		PrintfDlg(hParent, -1, "%s: Untitled", CEcatApp::GetAppName());
		return;
	}

	// Otherwise, make sure the path will fit into the title bar of the main window. If
	// not draw it with ellipses to shorten the path - we'll be using DrawText function here
	RECT rc;
	GetWindowRect(hParent, &rc);

	// Get NONCLIENTMETRICS for the main window - caption height, logical font, etc
	NONCLIENTMETRICS ncMetrics;
	memset(&ncMetrics, 0, sizeof(NONCLIENTMETRICS));
	ncMetrics.cbSize = sizeof(NONCLIENTMETRICS);
	SystemParametersInfo(SPI_GETNONCLIENTMETRICS, ncMetrics.cbSize, &ncMetrics, 0);

	rc.top = 0;
	rc.bottom = ncMetrics.iCaptionHeight;
	int iWidth = rc.right - rc.left;
	rc.left = 0;
	rc.right =	iWidth -								// total window width
				GetSystemMetrics(SM_CXSMICON) -			// icon
				(ncMetrics.iCaptionWidth * 3  + 4) -	// min. max, close buttons	
				(GetSystemMetrics(SM_CXEDGE) * 2 + 4);	// border
	char lpszFullTitle[MAX_PATH + 1] = "";
	
	// Before we create the title, make sure there are no '%' signs
	char strCleanTitle[MAX_PATH * 2];
	memset(strCleanTitle, 0, sizeof(strCleanTitle));
	int i = 0;
	LPSTR lpszTemp = lpszTitle;
	while (*lpszTemp)
	{
		strCleanTitle[i] = *lpszTemp++;
		if (strCleanTitle[i] == '%')
		{
			strCleanTitle[i + 1] = '%';
			i++;
		}
		i++;
	}
	wsprintf(lpszFullTitle, "%s: %s", CEcatApp::GetAppName(), strCleanTitle);

	// Let's see how it will be drawn by the DrawCaption
	HDC hDC = GetDC(hParent);
	HFONT hFont = CreateFontIndirect(&(ncMetrics.lfCaptionFont));
	HFONT hOldFont = (HFONT) SelectObject(hDC, hFont);
	DrawText(hDC, lpszFullTitle, strlen(lpszFullTitle), &rc, 
		DT_PATH_ELLIPSIS | DT_MODIFYSTRING | DT_CALCRECT);
	SelectObject(hDC, hOldFont);
	DeleteObject(hFont);
	ReleaseDC(hParent, hDC);

	// Ok, set the updated caption
	PrintfDlg(hParent, -1, lpszFullTitle);
}

//////////////////////////////////////////////////////////////////////
void CEcatMainDlg::DoFileNew()
{
	if (changed_since_save && MessageBox(NULL, STR_TEST_MOD_TEXT, STR_TEST_MOD_TITLE, 
							MB_OKCANCEL | MB_ICONINFORMATION) != IDOK)
		return;

	fnamebuf[0] = 0;
	DeleteContext();
	UpdateTitle();
	change_shown = -3;
	changed_since_save = FALSE;

	// Give derived classes a chance to do some special processing
	OnFileNew();
}

//////////////////////////////////////////////////////////////////////
void CEcatMainDlg::OpenFile()
{
	HANDLE hFile = CreateFile(
					fnamebuf, 
					GENERIC_READ, 
					0, 
					NULL, 
					OPEN_EXISTING,
					FILE_ATTRIBUTE_NORMAL,
					NULL
					);
	if (hFile == INVALID_HANDLE_VALUE)
	{
		Message("File Error", "Unable to open file : %s", fnamebuf);
		changed_since_save = FALSE;
		DoFileNew();
		return;
	}

	// E500 has some stuff right before it parses the file
	BeforeFileParse();
	DeleteContext();
	BOOL bLoadOk = Load(hFile);
	CloseHandle(hFile);

	if (bLoadOk)
	{
		UpdateTitle((LPSTR) fnamebuf);
	
		// Give derived classes a chance to do some special processing
		OnFileOpen();
	}
	else 
	{
		// File parsing error, we may end up with some garbage data - 
		// clean it up in DeleteContext(). Error message has already 
		// been given to the user at this point.
		fnamebuf[0] = 0;
		DeleteContext();
		UpdateTitle();
	}
	
	changed_since_save = FALSE;
	change_shown = -3;		
}

//////////////////////////////////////////////////////////////////////
void CEcatMainDlg::DoFileOpen()
{
	if (changed_since_save && MessageBox(NULL, STR_TEST_MOD_TEXT, STR_TEST_MOD_TITLE, 
							MB_OKCANCEL | MB_TASKMODAL | MB_ICONINFORMATION) != IDOK)
		return;

	if (CommonDlgOperation(GetHWND(), ghinst, fnamebuf, OPEN))
		OpenFile();
}

//////////////////////////////////////////////////////////////////////
void CEcatMainDlg::DoFileSave()
{
	if (fnamebuf[0] != 0)
	{
		HANDLE hFile = CreateFile(
						fnamebuf, 
						GENERIC_WRITE, 
						0, 
						NULL, 
						CREATE_ALWAYS,
						FILE_ATTRIBUTE_NORMAL,
						NULL
						);
		if (hFile == INVALID_HANDLE_VALUE)
		{
			Message("Save File Error", "Unable to create file : %s", fnamebuf);
			return;
		}

		Save(hFile);
		FlushFileBuffers(hFile);
		CloseHandle(hFile);
		UpdateTitle((LPSTR) fnamebuf);
		changed_since_save = FALSE;
		change_shown = -3;

		// Give derived classes a chance to do some special processing
		OnFileSave(FALSE);
	}
	else
		DoFileSaveAs();
}

//////////////////////////////////////////////////////////////////////
void CEcatMainDlg::DoFileSaveAs()
{
	fnamebuf[0] = 0;
	if (CommonDlgOperation(GetHWND(), ghinst, fnamebuf, SAVE))
	{
		HANDLE hFile = CreateFile(
						fnamebuf, 
						GENERIC_WRITE, 
						0, 
						NULL, 
						CREATE_ALWAYS,
						FILE_ATTRIBUTE_NORMAL,
						NULL
						);
		if (hFile == INVALID_HANDLE_VALUE)
		{
			Message("Save File Error", "Unable to create file : %s", fnamebuf);
			fnamebuf[0] = 0;
			return;
		}

		Save(hFile);
		FlushFileBuffers(hFile);
		CloseHandle(hFile);
		UpdateTitle((LPSTR) fnamebuf);
		changed_since_save = FALSE;
		change_shown = -3;

		// Give derived classes a chance to do some special processing
		OnFileSave(TRUE);
	}
}

//////////////////////////////////////////////////////////////////////
BOOL CEcatMainDlg::StartTimer(UINT uiTimerId, UINT uiDelay)
{
	if (m_uiTimerID != NULL)
		return FALSE;

	m_uiTimerID = SetTimer(m_hWnd, uiTimerId, uiDelay, NULL);
	return (m_uiTimerID != NULL);
}

//////////////////////////////////////////////////////////////////////
void CEcatMainDlg::StopTimer()
{
	if (m_uiTimerID != NULL)
	{
		KillTimer(m_hWnd, m_uiTimerID);
		m_uiTimerID = NULL;
	}
}



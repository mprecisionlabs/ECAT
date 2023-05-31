//////////////////////////////////////////////////////////////////////
// EcatMainWnd.cpp: implementation of the CEcatMainWnd class.
//
//////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "EcatApp.h"
#include "EcatMainWnd.h"
#include "EcatMainDlg.h"
#include "globals.h"
#include "calinfo.h"

#define LOG_MENU_POS			1

//////////////////////////////////////////////////////////////////////
// Global ECat window procedure
//////////////////////////////////////////////////////////////////////
static LRESULT CALLBACK EcatWndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	// Set the pointer to the calling CEcatMainWnd if it's WM_NCCREATE (comes in CREATESTRUCT), 
	// otherwise extract the pointer from GWL_USERDATA
	CEcatMainWnd* lpWnd = NULL;
	if (msg == WM_NCCREATE)
	{
		if (lParam != NULL)
		{
			lpWnd = (CEcatMainWnd*) (((LPCREATESTRUCT) lParam)->lpCreateParams); 
			SetWindowLong(hWnd, GWL_USERDATA, (LONG) lpWnd);
		}
	}
	else
		lpWnd = (CEcatMainWnd*) GetWindowLong(hWnd, GWL_USERDATA);

	if (lpWnd != NULL) 
		return (lpWnd->LocalWndProc(hWnd, msg, wParam, lParam));
	else
		return DefWindowProc(hWnd, msg, wParam, lParam);
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CEcatMainWnd::CEcatMainWnd()
{
	m_hWnd = NULL;
	m_pDlg = NULL;
	m_strHelpFilePath[0] = '\0';
}

//////////////////////////////////////////////////////////////////////
CEcatMainWnd::~CEcatMainWnd()
{
	if (m_pDlg != NULL)
	{
		delete m_pDlg;
		m_pDlg = NULL;
	}
}

//////////////////////////////////////////////////////////////////////
HWND CEcatMainWnd::GetDlgHWND() 
{
	return (m_pDlg) ? m_pDlg->GetHWND(): NULL;
}

//////////////////////////////////////////////////////////////////////
BOOL CEcatMainWnd::Create(HINSTANCE hInstance, LPCSTR lpszAppName)
{
	if (!Register(hInstance, lpszAppName))
		return FALSE;

	m_hWnd = CreateWindow(
					lpszAppName, 
					"Title",
					WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_BORDER | WS_MINIMIZEBOX,
					CW_USEDEFAULT, 0, CW_USEDEFAULT, 0,
					NULL, 
					NULL,
					hInstance, 
					(LPVOID) this);
	if (m_hWnd == NULL) return FALSE;
	g_hMainWnd = m_hWnd;

	// Change the "Calibration" menu
	if (CALIBRATION) 
	{
		HMENU hMenu = GetMenu(m_hWnd);
		HMENU hSubMenu = GetSubMenu(hMenu, 2);
		InsertMenu(hSubMenu, 1, MF_BYPOSITION | MF_SEPARATOR, -1, NULL);
		InsertMenu(hSubMenu, 2, MF_BYPOSITION | MF_STRING, IDM_CALIBRATION, "&Calibrate...");
	}

	CenterWindow(m_hWnd); 
	ShowWindow(m_hWnd, SW_SHOWNORMAL);
	UpdateWindow(m_hWnd);
	return TRUE;
}

//////////////////////////////////////////////////////////////////////
int CEcatMainWnd::OnCreate(HWND hWnd, LPCREATESTRUCT lpCS)
{
	/*
	if (!m_dlgMain.Create(ResId, hWnd))
		return -1;
	*/
	return 0;
}

//////////////////////////////////////////////////////////////////////
void CEcatMainWnd::OnDestroy() 
{
	// Shutdown help
	WinHelp(m_hWnd, GetHelpFileName(), HELP_QUIT, 0 );

	// Disconnect us from the window proc
	if (m_hWnd != NULL)
	{
		LONG l = SetWindowLong(m_hWnd, GWL_WNDPROC, (LONG) DefWindowProc);
		m_hWnd = NULL;
	}

	// Make the app quit (if the window is created with new, it will be 
	// deleted in ExitInstance())
	PostQuitMessage(0); 
}

//////////////////////////////////////////////////////////////////////
BOOL CEcatMainWnd::DoExit() 
{
	// Make sure the dialog, if present, will allow us to quit
	if (m_pDlg && m_pDlg->DoExit())
	{
		if (m_hWnd) 
			DestroyWindow(m_hWnd); 
		return TRUE;
	}

	return FALSE;
}

//////////////////////////////////////////////////////////////////////
LPCSTR CEcatMainWnd::GetHelpFilePath()
{
	if (m_strHelpFilePath[0] == '\0')
		wsprintf(m_strHelpFilePath, "%sHelp\\%s", CEcatApp::GetAppDir(), GetHelpFileName());
	
	return 	m_strHelpFilePath;
}

//////////////////////////////////////////////////////////////////////
// NOTE: Return 0 if the message has been processed, 1 for further 
//		 processing by the system
//////////////////////////////////////////////////////////////////////
int CEcatMainWnd::OnCmdMsg(UINT uiNotify, UINT uiCtrlId, HWND hwndCtl)
{
	HMENU hMenu, hSubMenu;
	switch (uiCtrlId)
	{
	case IDD_ECAT_EXIT:
	case IDCANCEL:
		return !DoExit();
	
	case CMD_ABORT_ILOCK:
		// Relay abort interlock command to the dialog
		if (m_pDlg)
			SendMessage(m_pDlg->GetHWND(), WM_COMMAND, CMD_ABORT_ILOCK, 0);
		break;

	case CMD_PAUSE:
		// Relay pause command to the dialog
		if (m_pDlg)
			SendMessage(m_pDlg->GetHWND(), WM_COMMAND, CMD_PAUSE, (LPARAM) hwndCtl);
		break;

	// Dafult menu commands
	case IDM_NEW:
		if (m_pDlg)
			m_pDlg->DoFileNew();
		break;

	case IDM_OPEN:
		if (m_pDlg)
			m_pDlg->DoFileOpen();
		break;

	case IDM_SAVE:
		if (m_pDlg)
			m_pDlg->DoFileSave();
		break;

	case IDM_SAVE_AS:
		if (m_pDlg)
			m_pDlg->DoFileSaveAs();
		break;

	case IDM_LON:
		hMenu = GetMenu(m_hWnd);
		hSubMenu = GetSubMenu(hMenu, LOG_MENU_POS);
		ModifyMenu(hMenu, LOG_MENU_POS, MF_BYPOSITION|MF_POPUP, (UINT)hSubMenu, "&Log [ON]");
		DrawMenuBar(m_hWnd);
		Log.set_log_on(TRUE);
		break;

	case IDM_LOFF:
		hMenu = GetMenu(m_hWnd);
		hSubMenu = GetSubMenu(hMenu, LOG_MENU_POS);
		ModifyMenu(hMenu, LOG_MENU_POS, MF_BYPOSITION|MF_POPUP, (UINT)hSubMenu, "&Log [OFF]");
		DrawMenuBar(m_hWnd);
		Log.set_log_on(FALSE);
		break;

	case IDM_LSET:
		Log.setup_log();
		DrawMenuBar(m_hWnd);
		break;

	case IDM_NAME:
		if (m_pDlg)
		{
			m_pDlg->OnChangeName();
			DrawMenuBar(m_hWnd);
		}
		break;

	case IDM_CALIBRATION:
		show_cal(GetInst(), m_hWnd);
		UpdateWindow(m_hWnd);
		if (m_pDlg)
			m_pDlg->PostCalibrate();
		break;

	case IDM_HELP_CONTENTS:
		WinHelp(m_hWnd, GetHelpFilePath(), HELP_CONTENTS, 0 );
		return TRUE;

	default:
		return 1;
	}

	return 0;
}

//////////////////////////////////////////////////////////////////////
LRESULT CALLBACK CEcatMainWnd::LocalWndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	// Local window procedure
	switch (msg) 
	{
		case WM_CREATE:
			return OnCreate(hWnd, (LPCREATESTRUCT) lParam);

		case WM_SYSCOMMAND:
			// See if the user is trying to close us through system 'x' button
			if (wParam == SC_CLOSE)
				return !DoExit();
			return DefWindowProc(hWnd, msg, wParam, lParam);

		case WM_COMMAND:
			if (!OnCmdMsg((UINT) HIWORD(wParam), (UINT) LOWORD(wParam), (HWND) lParam))
				break;
			return DefWindowProc(hWnd, msg, wParam, lParam);

		case WM_DESTROY:
			OnDestroy();
			break;

		case WM_CLOSE:
			DoExit();
			break;

		case WM_CHANGE_TITLE:
			SetWindowText(hWnd, (LPCSTR) lParam);
			break;

		// Enable / disable main menu
		case WM_ENABLE_RUN:
			{
				UINT uiFlag = MF_BYPOSITION | (((BOOL) lParam) ? MF_GRAYED : MF_ENABLED);
				HMENU hMenu	= GetMenu(hWnd);
				for (int i = 0; i < GetMenuItemCount(hMenu); i++)
					EnableMenuItem(hMenu, i, uiFlag);
				DrawMenuBar(hWnd);
			}
			break;

		default:
			// Check our registered message
			if (msg == CEcatApp::WM_REG_ECAT)
				return (LRESULT) TRUE;
			return DefWindowProc(hWnd, msg, wParam, lParam);
	}

	return 0;
}

//////////////////////////////////////////////////////////////////////
BOOL CEcatMainWnd::Register(HINSTANCE hInstance, LPCSTR lpszAppName)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX); 

	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= (WNDPROC) EcatWndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= LoadIcon(hInstance, MAKEINTRESOURCE(IDI_APP));
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= HBRUSH(COLOR_BTNFACE + 1);
	wcex.lpszMenuName	= MAKEINTRESOURCE(IDR_MAIN_MENU);
	wcex.lpszClassName	= lpszAppName;
	wcex.hIconSm		= (HICON) LoadImage(hInstance, MAKEINTRESOURCE(IDI_APP), 
							IMAGE_ICON, 16, 16, LR_DEFAULTCOLOR);

	return RegisterClassEx(&wcex);
}

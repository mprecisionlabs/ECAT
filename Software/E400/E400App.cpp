//////////////////////////////////////////////////////////////////////
// E400App.cpp: implementation of the CE400App class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "e400.h"
#include "E400App.h"
#include "E400MainWnd.h"
#include "..\core\globals.h"
#include "..\core\calinfo.h"

HICON CE400App::pericon = NULL;
HICON CE400App::freqicon = NULL; 
HICON CE400App::duricon = NULL; 
HICON CE400App::vicon = NULL; 
HICON CE400App::swicon = NULL; 
HICON CE400App::vpicon = NULL; 
HICON CE400App::pvicon = NULL; 
HICON CE400App::mvicon = NULL; 
HICON CE400App::mswicon = NULL; 
HICON CE400App::repicon = NULL; 
HICON CE400App::phaseicon = NULL;

HCURSOR CE400App::percursor = NULL; 
HCURSOR CE400App::freqcursor = NULL; 
HCURSOR CE400App::durcursor = NULL; 
HCURSOR CE400App::phcursor = NULL; 
HCURSOR CE400App::vcursor = NULL; 
HCURSOR CE400App::swcursor = NULL; 
HCURSOR CE400App::vpcursor = NULL; 
HCURSOR CE400App::pvcursor = NULL; 
HCURSOR CE400App::mvcursor = NULL; 
HCURSOR CE400App::repcursor = NULL; 
HCURSOR CE400App::phasecursor = NULL;

void PqfShowUploadDialog(BOOL bShow) {};

// Forward declaration
long __declspec(dllexport) CALLBACK	BlockWndProc(HWND, UINT, WPARAM, LPARAM);
void RegisterClasses(HANDLE hInstance);

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CE400App::CE400App(LPCSTR szAppName) : CEcatApp(szAppName)
{
	RESET_REQUIRED = FALSE;
}

//////////////////////////////////////////////////////////////////////
CE400App::~CE400App()
{
}

//////////////////////////////////////////////////////////////////////
BOOL CE400App::CreateMainWnd()
{
	m_pMainWnd = new CE400MainWnd();
	if (m_pMainWnd == NULL) return FALSE;

	if (!m_pMainWnd->Create(GetInst(), GetAppName()))
	{
		delete m_pMainWnd;
		m_pMainWnd = NULL;
		return FALSE;
	}

	return TRUE;
}

//////////////////////////////////////////////////////////////////////
BOOL CE400App::InitInstance(HINSTANCE hInstance, UINT uiAccelId, LPCSTR lpszCmdLine)
{
	DEF_EXT = "eft";
	DEF_FILES = "EFT Test Files (*.eft)\0*.eft\0801-4 Standard (*.std)\0*.std\0All Files (*.*)\0*.*\0";
	INI_NAME = "E400.INI";
	COMMERROR = FALSE;
	ghinst = hInstance;

	// icons.
	pericon = LoadIcon(hInstance, IDI_PERIOD);
	freqicon = LoadIcon(hInstance, IDI_FREQUENCY);
	duricon = LoadIcon(hInstance, IDI_DURATION);
	vicon = LoadIcon(hInstance, IDI_VOLTAGE);
	pvicon = LoadIcon(hInstance, IDI_POS_V);
	mvicon = LoadIcon(hInstance, IDI_NEG_V);
	vpicon = LoadIcon(hInstance, IDI_POLARITY);
	swicon = LoadIcon(hInstance, IDI_SWITCH);
	repicon = LoadIcon(hInstance, IDI_REPEAT);
	phaseicon = LoadIcon(hInstance, IDI_PHASE);
	mswicon = NULL;

	// cursors.
	LoadCursor(hInstance, IDC_CUR_PERIOD, percursor);
	LoadCursor(hInstance, IDC_CUR_FREQUENCY, freqcursor);
	LoadCursor(hInstance, IDC_CUR_DURATION, durcursor);
	LoadCursor(hInstance, IDC_CUR_VOLTAGE, vcursor);
	LoadCursor(hInstance, IDC_CUR_POLARITY, vpcursor);
	LoadCursor(hInstance, IDC_CUR_SWITCH, swcursor);
	LoadCursor(hInstance, IDC_CUR_REPEAT, repcursor);
	LoadCursor(hInstance, IDC_CUR_POS_V, pvcursor);
	LoadCursor(hInstance, IDC_CUR_NEG_V, mvcursor);
	LoadCursor(hInstance, IDC_CUR_PHASE, phasecursor);

	RegisterClasses(hInstance);

	// Register block window class
	WNDCLASS wndClass;
	wndClass.style = CS_HREDRAW	| CS_VREDRAW;
	wndClass.lpfnWndProc = BlockWndProc;
	wndClass.cbClsExtra	= 0;
	wndClass.cbWndExtra	= 0;
	wndClass.hInstance = hInstance;
	wndClass.hIcon = NULL;
	wndClass.hCursor = ::LoadCursor(NULL,	IDC_ARROW);
	wndClass.hbrBackground = (HBRUSH)(COLOR_BTNFACE + 1);
	wndClass.lpszMenuName = NULL;
	wndClass.lpszClassName = "BlockClass";
	if (!RegisterClass(&wndClass))
		return FALSE;

	// Initialize and show our window (CreateMainWnd() will be called in InitInstance())
	return CEcatApp::InitInstance(hInstance, uiAccelId, lpszCmdLine);
}

//////////////////////////////////////////////////////////////////////
BOOL CE400App::ReadCalFile()
{
	Cal_Read();
	if (!EFT_EXIST && CALIBRATION)
	{
		DestroyCommLinkDlg();
		while (!EFT_EXIST)
		{
			if (MessageBox(NULL, 
					"No EFT modules exist in this system.\nWould you like to switch to the calibration mode?", 
					"No EFT Modules", MB_SYSTEMMODAL | MB_YESNO | MB_ICONEXCLAMATION) == IDNO)
				break;
			show_cal(GetInst(), NULL);
			Cal_Read();
		}
	}
	
	if (!EFT_EXIST)
	{
		DestroyCommLinkDlg();
		MessageBox(NULL, "No EFT modules exist in this system.\nIt is not possible to run this system.", "No EFT Modules", MB_SYSTEMMODAL | MB_OK | MB_ICONERROR);
		return FALSE;
	}

	return TRUE;
}


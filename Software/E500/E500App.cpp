//////////////////////////////////////////////////////////////////////
// E500App.cpp: implementation of the CE500App class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "e500.h"
#include "E500App.h"
#include "E500MainWnd.h"
#include "..\core\globals.h"
#include "..\core\calinfo.h"

HICON CE500App::pericon = NULL;
HICON CE500App::freqicon = NULL; 
HICON CE500App::duricon = NULL; 
HICON CE500App::vicon = NULL; 
HICON CE500App::swicon = NULL; 
HICON CE500App::vpicon = NULL; 
HICON CE500App::pvicon = NULL; 
HICON CE500App::mvicon = NULL; 
HICON CE500App::mswicon = NULL; 
HICON CE500App::repicon = NULL; 
HICON CE500App::phicon = NULL;
HICON CE500App::waicon = NULL;

HCURSOR CE500App::percursor = NULL; 
HCURSOR CE500App::freqcursor = NULL; 
HCURSOR CE500App::durcursor = NULL; 
HCURSOR CE500App::phcursor = NULL; 
HCURSOR CE500App::vcursor = NULL; 
HCURSOR CE500App::swcursor = NULL; 
HCURSOR CE500App::vpcursor = NULL; 
HCURSOR CE500App::pvcursor = NULL; 
HCURSOR CE500App::mvcursor = NULL; 
HCURSOR CE500App::repcursor = NULL; 
HCURSOR CE500App::phasecursor = NULL;
HCURSOR CE500App::wacursor = NULL;

void PqfShowUploadDialog(BOOL bShow) {};

// Forward declaration
long __declspec(dllexport) CALLBACK	BlockWndProc(HWND, UINT, WPARAM, LPARAM);
void RegisterClasses(HANDLE hInstance);

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CE500App::CE500App(LPCSTR szAppName) : CEcatApp(szAppName)
{
	RESET_REQUIRED = FALSE;
	POWER_USER = FALSE;
}

//////////////////////////////////////////////////////////////////////
CE500App::~CE500App()
{
}

//////////////////////////////////////////////////////////////////////
BOOL CE500App::CreateMainWnd()
{
	m_pMainWnd = new CE500MainWnd();
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
BOOL CE500App::InitInstance(HINSTANCE hInstance, UINT uiAccelId, LPCSTR lpszCmdLine)
{
	DEF_EXT = "srg";
	DEF_FILES = "Surge Test Files (*.srg)\0*.srg\0 801-5 Standard (*.std)\0*.std\0All Files (*.*)\0*.*\0";
	INI_NAME = "E500.INI";
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
	swicon = LoadIcon(hInstance, IDI_POS_SWITCH);
	mswicon = LoadIcon(hInstance, IDI_NEG_SWITCH);
	repicon = LoadIcon(hInstance, IDI_REPEAT);
	phicon = LoadIcon(hInstance, IDI_PHASE);
	waicon = LoadIcon(hInstance, IDI_WAVE);

	// cursors.
	LoadCursor(hInstance, IDC_CUR_WAVE, wacursor);
	LoadCursor(hInstance, IDC_CUR_PHASE, phcursor);
	LoadCursor(hInstance, IDC_CUR_VOLTAGE, vcursor);
	LoadCursor(hInstance, IDC_CUR_POLARITY, vpcursor);
	LoadCursor(hInstance, IDC_CUR_SWITCH, swcursor);
	LoadCursor(hInstance, IDC_CUR_REPEAT, repcursor);
	LoadCursor(hInstance, IDC_CUR_POS_V, pvcursor);
	LoadCursor(hInstance, IDC_CUR_NEG_V, mvcursor);

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
BOOL CE500App::ReadCalFile()
{
	Cal_Read();

	if (!SURGE_EXIST && CALIBRATION)
	{
		DestroyCommLinkDlg();
		while (!SURGE_EXIST)
		{
			if (MessageBox(NULL, 
					"No surge modules exist in this system.\nWould you like to switch to the calibration mode?", 
					"No surge Modules", MB_SYSTEMMODAL | MB_YESNO | MB_ICONEXCLAMATION) == IDNO)
				break;
			show_cal(GetInst(), NULL);
			Cal_Read();
		}
	}

	if (!SURGE_EXIST)
	{
		DestroyCommLinkDlg();
		MessageBox(NULL,"No surge modules exist in this system.\nIt is not possible to run this system.", "No Surge Modules", MB_OK | MB_TASKMODAL | MB_ICONERROR);
		return FALSE;
	}

	return TRUE;
}

//////////////////////////////////////////////////////////////////////
void CE500App::ProcessCmdLine(int argc, char** argv)
{
	// Call base class for genral processing
	CEcatApp::ProcessCmdLine(argc, argv);

	// Handle extra command line switches 
	for (int i = 0; i < argc; i++)
	{
		if (strlen(argv[i]) == 0)
			continue;

		if (_stricmp(argv[i], "POWER_USER") == 0)
		{
			POWER_USER = TRUE;
			break;
		}
	}
}
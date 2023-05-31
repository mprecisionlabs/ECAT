//////////////////////////////////////////////////////////////////////
// PqfApp.cpp: implementation of the CPqfApp class.
//
//////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "windows.h"
#include "PqfApp.h"
#include "PqfMainWnd.h"

#define DO_GLOBALS
#include "..\core\globals.h"
#undef DO_GLOBALS

#define IS_MAIN_PROGRAM_FILE
#include "common.h"
#undef IS_MAIN_PROGRAM_FILE

#include "..\core\kt_comm.h"
#include "..\core\kt_hide.h"
#include "..\core\calinfo.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CPqfApp::CPqfApp(LPCSTR szAppName) : CEcatApp(szAppName)
{

}

//////////////////////////////////////////////////////////////////////
CPqfApp::~CPqfApp()
{

}

//////////////////////////////////////////////////////////////////////
BOOL CPqfApp::CreateMainWnd()
{
	m_pMainWnd = new CPqfMainWnd();
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
BOOL CPqfApp::InitInstance(HINSTANCE hInstance, UINT uiAccelId, LPCSTR lpszCmdLine)
{
	DEF_EXT = "pqf";
	DEF_FILES = "PQF Test Files (*.pqf)\0*.pqf\0All Files (*.*)\0*.*\0";
	INI_NAME = "PQF.INI";
	COMMERROR = FALSE;
	ghinst = hInstance;

    // Load cursors and bitmaps that we'll be using
    obj_hCursor_arrow = ::LoadCursor(NULL, IDC_ARROW);
    LoadCursor(hInstance, IDC_CUR_CROSS, obj_hCursor_cross);

	// Register the graph classes first and then call the base class InitInstance()
	// to complete the initialization
	WNDCLASS wndClass;
	wndClass.style         = CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS;
	wndClass.lpfnWndProc   = (WNDPROC) PqfGraphWndProc;
	wndClass.cbClsExtra    = 0;
	wndClass.cbWndExtra    = sizeof(DWORD);
	wndClass.hInstance     = hInstance;
	wndClass.hIcon         = NULL;
	wndClass.hCursor       = obj_hCursor_arrow;
	wndClass.hbrBackground = HBRUSH(COLOR_BTNFACE + 1);
	wndClass.lpszMenuName  = NULL;
	wndClass.lpszClassName = "PqfGraph";
	if (!RegisterClass(&wndClass)) return FALSE;

	wndClass.style         = CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS;
	wndClass.lpfnWndProc   = (WNDPROC) GraphWndProc;
	wndClass.cbClsExtra    = 0;
	wndClass.cbWndExtra    = sizeof(DWORD);
	wndClass.hInstance     = hInstance;
	wndClass.hIcon         = NULL;
	wndClass.hCursor       = obj_hCursor_arrow;
	wndClass.hbrBackground = (HBRUSH) GetStockObject(WHITE_BRUSH);
	wndClass.lpszMenuName  = NULL;
	wndClass.lpszClassName = "Graph";
	if (!RegisterClass(&wndClass)) return FALSE;
	
	// Deafult settings
	bIsEP3Avail = FALSE;     

    // Global stuff
	ghinst = hInstance;

	// Initialize and show our window (CreateMainWnd() will be called in InitInstance())
	if (!CEcatApp::InitInstance(hInstance, uiAccelId, lpszCmdLine))
		return FALSE;

	return TRUE;
}

//////////////////////////////////////////////////////////////////////
BOOL CPqfApp::ReadCalFile()
{
	// Check calibration file
	Cal_Read();
	if (!PQF_EXIST && CALIBRATION)
	{
		DestroyCommLinkDlg();
		while (!PQF_EXIST)
		{
			if (MessageBox(NULL, 
					"No PQF modules exist in this system.\nWould you like to switch to the calibration mode?", 
					"No PQF Modules", MB_SYSTEMMODAL | MB_YESNO | MB_ICONEXCLAMATION) == IDNO)
				break;
			show_cal(GetInst(), NULL);
			Cal_Read();
		}
	}
	
	if (!PQF_EXIST)
	{
		DestroyCommLinkDlg();
		MessageBox(NULL, "No PQF modules exist in this system.\nIt is not possible to run this system.", "No PQF Modules", MB_SYSTEMMODAL | MB_OK | MB_ICONERROR);
		return FALSE;
	}

	return TRUE;
}


//////////////////////////////////////////////////////////////////////
void CPqfApp::ProcessCmdLine(int argc, char** argv)
{
	// Call base class for genral processing
	CEcatApp::ProcessCmdLine(argc, argv);

	// Handle extra command line switches 
	for (int i = 0; i < argc; i++)
	{
		if (strlen(argv[i]) == 0)
			continue;

		// Some additional stuff (duplication of the code taken from the old 
		// PQFWare in winmain.cpp) to make the app complaint with the legacy code
		if (!SIMULATION && (_stricmp(argv[i], "SIMULATE") == 0))
			SIMULATION = TRUE;

		if (_stricmp(argv[i], "DEBUG") == 0)
			testmgr_bDebug = TRUE;
	}

	if (!SIMULATION)
		SIMULATION = testmgr_bDebug;
}

//////////////////////////////////////////////////////////////////////
BOOL CPqfApp::CheckProfileStringInt(LPSTR app, LPSTR key, BOOL bDefault, LPSTR inifilespec)
{
    INTEGER len, val;
    len = GetPrivateProfileString (app, key, ")(", common_message, 6, inifilespec);
    if (!len OR (common_message[0] EQ ')'))
        return (bDefault);
    val = atoi (common_message);
    return (val);
} 

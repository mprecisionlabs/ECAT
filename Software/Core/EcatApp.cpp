//////////////////////////////////////////////////////////////////////
// EcatApp.cpp: implementation of the CEcatApp class.
//
//////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#define  DOGLOBALS
#include "globals.h"
#undef DOGLOBALS
#include "EcatApp.h"
#include "comm.h"
#include "EcatMainWnd.h"
#include "EcatMainDlg.h"

static HINSTANCE g_hInstance = NULL;
inline HINSTANCE GetInst() {return g_hInstance;}
char CEcatApp::m_szAppName[MAX_PATH] = "";
HCURSOR CEcatApp::st_prevCursor = NULL;

#define STR_MUTEX_NAME		"tkEcatSingleInstanceMutex"
static HANDLE st_hMutex = NULL;
UINT CEcatApp::WM_REG_ECAT = RegisterWindowMessage("tkEcatSingleInstanceMsg");

//////////////////////////////////////////////////////////////////////
// Static helpers
//////////////////////////////////////////////////////////////////////
void CEcatApp::BeginWaitCursor()
{
	if (st_prevCursor != NULL)
		st_prevCursor = GetCursor();
	SetCursor(::LoadCursor(NULL, IDC_WAIT));
}

//////////////////////////////////////////////////////////////////////
void CEcatApp::EndWaitCursor()
{
	if (st_prevCursor == NULL)
		st_prevCursor = ::LoadCursor(NULL, IDC_ARROW);
	SetCursor(st_prevCursor);
	st_prevCursor = NULL;
}

//////////////////////////////////////////////////////////////////////
HICON CEcatApp::LoadIcon(HINSTANCE hInst, UINT uiResID, BOOL bMapColors/* = TRUE*/)
{
	return (HICON) ::LoadImage(hInst, MAKEINTRESOURCE(uiResID), 
		IMAGE_ICON, 0, 0, (bMapColors ? LR_LOADMAP3DCOLORS : LR_DEFAULTCOLOR));
}

//////////////////////////////////////////////////////////////////////
void CEcatApp::LoadIcon(HINSTANCE hInst, UINT uiResID, HICON& hIcon)
{
	if (hIcon == NULL)
		hIcon = ::LoadIcon(hInst, MAKEINTRESOURCE(uiResID));
}

//////////////////////////////////////////////////////////////////////
void CEcatApp::LoadCursor(HINSTANCE hInst, UINT uiResID, HCURSOR& hCursor)
{
	if (hCursor == NULL)
		hCursor = ::LoadCursor(hInst, MAKEINTRESOURCE(uiResID));
}

//////////////////////////////////////////////////////////////////////
LPCSTR CEcatApp::GetAppDir()
{
	static char lpszAppPath[MAX_PATH + 1] = "";
	if (lpszAppPath[0] == 0)
	{
		memset(lpszAppPath, 0, sizeof(lpszAppPath));
		if (GetModuleFileName(NULL, lpszAppPath, MAX_PATH) == 0)
			return "\\";

		char drive[_MAX_DRIVE];
		char dir[_MAX_DIR];
		char fname[_MAX_FNAME];
		char ext[_MAX_EXT];
		_splitpath(lpszAppPath, drive, dir, fname, ext);
		_makepath(lpszAppPath, drive, dir, "", "");
	}

	return lpszAppPath;
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CEcatApp::CEcatApp(LPCSTR szAppName)
{
	m_pMainWnd = NULL;
	m_hAccelTable = NULL;
	strcpy(m_szAppName, szAppName);

	SIMULATION = FALSE;
	CALIBRATION = FALSE;
	m_iComPort = 1;				// default is COM1
	m_dwBaudRate = CBR_2400;	// default is 2400

	memset(m_strFileName, 0, sizeof(m_strFileName));
}

//////////////////////////////////////////////////////////////////////
CEcatApp::~CEcatApp()
{
}

//////////////////////////////////////////////////////////////////////
static BOOL CALLBACK EnumWindowsProc(
					HWND hwnd,      // handle to parent window
					LPARAM lParam   // application-defined value
)
{
	BOOL bFound = (BOOL) SendMessage(hwnd, CEcatApp::WM_REG_ECAT, 0, 0);
	if (bFound)
	{
		// does it have any popups?
		HWND hWndChild = GetLastActivePopup(hwnd);

		// if iconic, restore the main window
		if (IsIconic(hwnd))
			ShowWindow(hwnd, SW_RESTORE);

		// bring the main window or its popup to the foreground
		SetForegroundWindow(hwnd);
		*((BOOL*) lParam) = TRUE;

		return FALSE; // stop enumeration
	}

	return TRUE; // keep going
}
 
//////////////////////////////////////////////////////////////////////
BOOL CEcatApp::InitInstance(HINSTANCE hInstance, UINT uiAccelId, LPCSTR lpszCmdLine)
{
	// Sanity check
	if (hInstance == NULL) return FALSE;

	// Make sure there is no other ecat running. And if it is, bring it to the front
	st_hMutex = CreateMutex(NULL, TRUE, STR_MUTEX_NAME);
	if (st_hMutex == NULL)
		return FALSE;

	if (GetLastError() == ERROR_ALREADY_EXISTS)
	{
		// Find another ecat
		BOOL bFound = FALSE;
		EnumWindows(EnumWindowsProc, (LPARAM) &bFound);
		if (bFound)
			return FALSE;
	}

	g_hInstance = hInstance;
	if (uiAccelId != NULL)
		m_hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(uiAccelId));

	ProcessCmdLine(lpszCmdLine);

	// Put up a dialog box if not in simulation mode
	if (!SIMULATION)
		CreateCommLinkDlg();

	if (!StartComm())
		return FALSE;

	// Read/Create the calibration file if appropriate
	if (!ReadCalFile())
	{
		StopComm();
		return FALSE;
	}

	// Close the comm link box
	if (!SIMULATION)
		DestroyCommLinkDlg();

	// Create the main window
	if (!CreateMainWnd())
	{
		StopComm();
		return FALSE;
	}

	PostMessage(m_pMainWnd->GetHWND(), WM_COMMAND, IDM_ABOUT, 0);

	return TRUE;
}

//////////////////////////////////////////////////////////////////////
void CEcatApp::ExitInstance()
{
	if (m_pMainWnd != NULL)
	{
		delete m_pMainWnd;
		m_pMainWnd = NULL;
	}

	StopComm();

	if (st_hMutex != NULL)
		CloseHandle(st_hMutex);
}

//////////////////////////////////////////////////////////////////////
int CEcatApp::Run()
{
	// Main message loop:
	MSG msg;
	while (GetMessage(&msg, NULL, 0, 0)) 
	{
		if (!m_hAccelTable || !TranslateAccelerator(msg.hwnd, m_hAccelTable, &msg)) 
		{
			if ((g_hMainDlg != NULL) && IsDialogMessage(g_hMainDlg, &msg))
				continue;

			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	return msg.wParam;
}

//////////////////////////////////////////////////////////////////////
void CEcatApp::CreateCommLinkDlg()
{
	if (m_hCommLnkDlg != NULL)
		ShowCommLinkDlg(TRUE);
	else
		m_hCommLnkDlg = CreateDialog(g_hInstance, MAKEINTRESOURCE(IDD_COMM_LINK_MSG), NULL, (DLGPROC) DefWindowProc); 
}

//////////////////////////////////////////////////////////////////////
void CEcatApp::DestroyCommLinkDlg()
{
	if (m_hCommLnkDlg != NULL)
	{
		DestroyWindow(m_hCommLnkDlg);
		m_hCommLnkDlg = NULL;
	}
}

//////////////////////////////////////////////////////////////////////
void CEcatApp::ShowCommLinkDlg(BOOL bShow)
{
	if (m_hCommLnkDlg != NULL)
	{
		if (bShow)
		{
			EnableWindow(m_hCommLnkDlg, TRUE);
			ShowWindow(m_hCommLnkDlg, SW_SHOW);
		}
		else
		{
			EnableWindow(m_hCommLnkDlg, FALSE);
			ShowWindow(m_hCommLnkDlg, SW_HIDE);
		}
	}
}

//////////////////////////////////////////////////////////////////////
BOOL CEcatApp::StartComm()
{
	if (!SIMULATION)
	{
		// Open the COM port
		if (!InitComm(m_iComPort, m_dwBaudRate))
		{
			DestroyCommLinkDlg();
			if (MessageBox(NULL, "Unable to open communications port. Do you want to run in Simulation mode?",
										"Communications Error", MB_YESNO | MB_ICONQUESTION) == IDYES)
			{
				SIMULATION = 1;
				return TRUE;

			}
			else   
				return FALSE;
		}

		// OK, COM port is open, now find the ECAT
		BOOL bOpenOK = FALSE;
		do
		{  
			// OpenEcatComm() will return TRUE if ECAT is communicating
			bOpenOK = OpenEcatComm();
			if (!bOpenOK)
			{
				DestroyCommLinkDlg();
				switch (MessageBox(NULL, "Unable to communicate with the ECAT system:\n"
										" Abort:\tend Application\n"
										" Retry:\tretry connection\n"
										" Ignore:\trun in simulation mode\n",
										"Communications Error",
										MB_ABORTRETRYIGNORE | MB_ICONEXCLAMATION | MB_SETFOREGROUND | MB_TASKMODAL))
				{
				case IDABORT:
					StopComm();
					return FALSE;

				case IDIGNORE:
					StopComm();
					SIMULATION = 1;
					return TRUE;

				case IDRETRY:
					CreateCommLinkDlg();
					COMMERROR = FALSE;               // Force communications
					break;
				}
			}
		}
		while (!bOpenOK);
	}

	return TRUE;
}

//////////////////////////////////////////////////////////////////////
void CEcatApp::StopComm()
{
	CloseComm();
}

//////////////////////////////////////////////////////////////////////
// Basic command line processing common to all three apps. Can be 
// overwritten in derive classes to add more processing
//////////////////////////////////////////////////////////////////////
void CEcatApp::ProcessCmdLine(int argc, char** argv)
{
	for (int i = 0; i < argc; i++)
	{
		if (strlen(argv[i]) == 0)
			continue;

		_strupr(argv[i]);
		if (strcmp(argv[i], "SIMULATION") == 0)
			SIMULATION = TRUE;
		else if (strcmp(argv[i], "CALIBRATE") == 0)
			CALIBRATION = TRUE;
		else if (strcmp(argv[i], "COM2") == 0)
			m_iComPort = 2;
		else if (strcmp(argv[i], "4800") == 0)
			m_dwBaudRate = CBR_4800;
		else if (strcmp(argv[i], "9600") == 0)
			m_dwBaudRate = CBR_9600;
		else if (strcmp(argv[i], "19200") == 0)
			m_dwBaudRate = CBR_19200;
		else if (argv[i][0] == '/')
		{
			// Could be a file name switch
			LPSTR lpszFileName = strstr(argv[i], "/F:\"");
			if (lpszFileName == NULL)
				continue;

			strncpy(m_strFileName, lpszFileName + 4, MAX_PATH);
			// remove last quote
			int iLen = strlen(m_strFileName);
			if ((iLen > 0) && (m_strFileName[iLen - 1] == '\"'))
				m_strFileName[iLen - 1] = '\0';
		}
		else
			continue;
		
		// set it to zero-length for derived classes, so they can skip it all along 
		// instead checking it against a string
		argv[i][0] = '\0';
	}
}

//////////////////////////////////////////////////////////////////////
static LPSTR GetNextTok(LPSTR &argvBuf)
{
	while (((*argvBuf != '\0') && (*argvBuf == ' ')) || (*argvBuf == '\t'))
		argvBuf++;

	if (*argvBuf == '\0')
		return NULL;

	BOOL bQ = FALSE;
	LPSTR lpCurChar = argvBuf;
	while (*lpCurChar)
	{
		switch (*lpCurChar)
		{
		case '\"':
			if (bQ)
				// We've reached the second quote
				bQ = FALSE;
			else
				bQ = TRUE;
			lpCurChar++;
			break;

		case ' ':
		case '\t':
			if (bQ)
				// We are within the quoted string, continue search
				lpCurChar++;
			else
				goto Done;
			break;

		case '\0':
			// reached the end of the line
			goto Done;

		default:
			// inc current pointer
			lpCurChar++;
			break;
		}
	}

Done:
	LPSTR lpTok = argvBuf;
	if (*lpCurChar != '\0')
	{
		*lpCurChar = '\0';
		argvBuf = (lpCurChar + 1);
	}
	else
		argvBuf = lpCurChar;

	return lpTok;
}

//////////////////////////////////////////////////////////////////////
void CEcatApp::ProcessCmdLine(LPCSTR lpszCmdLine)
{
	// Establish string and get the first token
	char seps[] = " \t";
	int argc = 0;
	char *argv[MAX_PATH];
	memset(argv, 0, sizeof(argv));

	char argvBuf[MAX_PATH + 1];
	memset(argvBuf, 0, sizeof(argvBuf));
	strncpy(argvBuf, lpszCmdLine, MAX_PATH);
	LPSTR lpArgv = argvBuf;
	argv[argc] = GetNextTok(lpArgv);	// first token
	while (argv[argc])
	{
		argc++;
		if (argc == MAX_PATH)
			break;

		argv[argc] = GetNextTok(lpArgv);
	}

	ProcessCmdLine(argc, argv);
}

//////////////////////////////////////////////////////////////////////
BOOL CEcatApp::CreateMainWnd()
{
	m_pMainWnd = new CEcatMainWnd();
	if (m_pMainWnd == NULL) return FALSE;

	if (!m_pMainWnd->Create(g_hInstance, m_szAppName))
	{
		delete m_pMainWnd;
		m_pMainWnd = NULL;
		return FALSE;
	}

	return TRUE;
}

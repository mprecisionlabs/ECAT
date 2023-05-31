// Implementation of common functions/structures used by all three programs E400, E500, PQFWare.
#include "stdafx.h"

#include "shlobj.h"
#include "Shlwapi.h"
#pragma comment(lib, "Shlwapi.lib")  // pass lib to linker

// Global handle to the main dialog
HWND g_hMainDlg = NULL;
HWND g_hMainWnd = NULL;

// Debugging function for Windows. Outputs a debug string to the current debug dump context.
// For the release version the function is empty and will not do anything.
#ifdef _DEBUG
void DBGOUT(LPSTR lpszMsg, ...)
{
	const size_t MAX_DBG_OUT = 512;	 
	char dumpStr[MAX_DBG_OUT];

	va_list args;
	va_start(args, lpszMsg);

	int nBuf = _vsnprintf(dumpStr, MAX_DBG_OUT - 1, lpszMsg, args);
	if (nBuf >= 0) 
	{
		dumpStr[nBuf] = '\0';
		OutputDebugString(dumpStr);
	}
	va_end(args);
}
#else
void DBGOUT(LPSTR lpszMsg, ...){}
#endif

// C2S - client to screen conversion of RECT structure
void C2S(HWND hParent, LPRECT pRect)
{
	if ((hParent == NULL) || (pRect == NULL))
		return;

	ClientToScreen(hParent, (LPPOINT) pRect);
	ClientToScreen(hParent, (LPPOINT) ((LPPOINT)pRect + 1));
}

// S2C - screen to client conversion of RECT structure
void S2C(HWND hParent, LPRECT pRect)
{
	if ((hParent == NULL) || (pRect == NULL))
		return;

	ScreenToClient(hParent, (LPPOINT) pRect);
	ScreenToClient(hParent, (LPPOINT) ((LPPOINT)pRect + 1));
}

// Determines whether the specified address is valid
BOOL IsValidAddress(void* p, UINT uiCnt, BOOL bReadWrite/* = TRUE*/)
{
	return ((p != NULL) && !IsBadReadPtr(p, uiCnt) && (!bReadWrite || !IsBadWritePtr(p, uiCnt)));
}

// Centers main window within the desktop's working area
void CenterWindow(HWND hWnd)
{
	if (!IsWindow(hWnd)) return;

	RECT rcWA, rcWnd;
	SystemParametersInfo(SPI_GETWORKAREA, NULL, &rcWA, NULL);
	GetWindowRect(hWnd, &rcWnd);
	POINT ptCenter;
	ptCenter.x = (rcWA.right - rcWA.left) / 2;
	ptCenter.y = (rcWA.bottom - rcWA.top) / 2;
	LONG lW = (rcWnd.right - rcWnd.left) / 2;
	LONG lH = (rcWnd.bottom - rcWnd.top) / 2;
	SetWindowPos(hWnd, NULL, rcWA.left + ptCenter.x - lW, rcWA.top + ptCenter.y - lH, 0, 0, 
		SWP_NOACTIVATE | SWP_NOSIZE | SWP_NOZORDER);
}

// Returns version string
LPSTR get_revision_string()
{
    static char revision_string[200];
    char date[20];
    char year[5];
    strcpy(date, __DATE__);
    sprintf(year, &date[strlen(date)-4]);
    sprintf(revision_string, REVISION_FORMAT,
        PRODUCT_LINE, PRODUCT, SUBPRODUCT, SUBPRODUCT_VERSION, year, COMPANY);

    return revision_string;
}

LPTSTR get_app_data_file_path(LPSTR file_name)
{
    // INI and LOG files are located in "all users" app data
    //  directory (CSIDL_COMMON_APPDATA), in subdirectory "Thermo\ECAT"
    static TCHAR file_path[MAX_PATH];
    SHGetFolderPath(NULL, CSIDL_COMMON_APPDATA, NULL, SHGFP_TYPE_CURRENT, file_path);
    PathCombine(file_path, file_path,  COMPANY_PATH);
    PathCombine(file_path, file_path,  PRODUCT);
    PathCombine(file_path, file_path,  file_name);

    return file_path;
}
#ifndef __THERMO_KEYTEK_UTIL_2_H__
#define __THERMO_KEYTEK_UTIL_2_H__

VOID UtilInit (WORD wm_msgbox);
VOID UtilEndDialog (HWND hWnd, INTEGER nResult, LPHANDLE lphButtonList);
BOOL UtilDestroyDialog (HWND hWnd, LPHANDLE lphButtonList);
BOOL UtilSetMenuName (HINSTANCE hInst, HWND hWnd, LPSTR lpszMenu);
VOID UtilEnableCtlColor (BOOL bEnable);
HBRUSH UtilCtlColor (HWND hWnd, HDC hDC, DWORD text_color, DWORD textback_color, HBRUSH hBackBrush);
INTEGER UtilMessageBox (HWND hWnd, LPSTR message, LPSTR caption, WORD wMcode);
BOOL UtilMessageTab (HANDLE hInst, HWND hWnd, WORD wStrnum, LPSTR caption, WORD wMcode);
BOOL UtilMessageTab2 (HANDLE hInst, HWND hWnd, WORD wStrnum1, WORD wStrnum2, LPSTR caption, WORD wMcode);
LPSTR UtilLoadString (HANDLE hInst, HWND hWnd, WORD wStrnum, LPSTR buftab, INTEGER maxlen);
LPSTR UtilLoadStrings (HANDLE hInst, HWND hWnd, WORD wStrnum, INTEGER strcnt, LPSTR buftab, INTEGER maxlen);
INTEGER UtilStripName (LPSTR name);
INTEGER UtilCompressName (LPSTR name);
INTEGER UtilStripExp (LPSTR text);
INTEGER UtilGetInt (LPSTR buffer);
INTEGER UtilGetSignedInt (LPSTR buffer, BOOL FAR * lpbOK);
LONG UtilGetLong (LPSTR buffer);
LONG UtilGetLong100 (LPSTR buffer);
VOID UtilDumpValue100 (LONG value, LPSTR buffer, INTEGER fieldlen, BOOL bStrip, BOOL bFixed, INTEGER qdec);
VOID UtilRectScreenToClient (HWND hWnd, LPRECT lprect);
VOID UtilDrawBitmap (HDC hDC, short xStart, short yStart, HBITMAP hBitmap);
VOID UtilMakePathList (HWND hWnd, LPSTR pathlist, LPSTR path, LPSTR sub1, LPSTR sub2, LPSTR sub3, LPSTR ext);
INTEGER UtilDirDir2ListBox (HWND hWndListBox, LPSTR pathlist, LPSTR match);
BOOL UtilCreateDirectoryChain (HWND hWnd, CHARACTER drivech, LPSTR dirpath);
BOOL UtilCheckFilename (HWND hWnd, LPSTR filename);
BOOL UtilCheckFilespec (HWND hWnd, LPSTR filespec);

#endif // __THERMO_KEYTEK_UTIL_2_H__

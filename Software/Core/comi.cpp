#include "stdafx.h"
#pragma hdrstop

LONG __declspec(dllexport) CALLBACK KT_HiddenWndProc(HWND hWnd, UINT Message, WPARAM wParam, LPARAM lParam);

char szarCommAppName[] = "kt_hide";
HWND init_kt_comm(HWND phwnd, HANDLE pinst)
{
	WNDCLASS    wndclass ;
	HWND hwnd;

	wndclass.style         = CS_HREDRAW | CS_VREDRAW ;
	wndclass.lpfnWndProc   = KT_HiddenWndProc ;

	// Extra storage for Class and Window objects.
	wndclass.cbClsExtra    = 0;
	wndclass.cbWndExtra    = 4;
	wndclass.hInstance     = (HINSTANCE)pinst;
	wndclass.hIcon         = NULL;
	wndclass.hCursor       = NULL;
	wndclass.hbrBackground = (HBRUSH)GetStockObject (WHITE_BRUSH) ;
	wndclass.lpszMenuName  = szarCommAppName;
	wndclass.lpszClassName = szarCommAppName;
	RegisterClass (&wndclass) ;

	hwnd = CreateWindow (szarCommAppName,szarCommAppName,
	WS_POPUP | WS_MINIMIZE,
	CW_USEDEFAULT, CW_USEDEFAULT,
	CW_USEDEFAULT, CW_USEDEFAULT,
	NULL, NULL, (HINSTANCE)pinst, NULL) ;

	SetWindowLong(hwnd,0,(long)phwnd); /*Set the parent handle in the window word */
	return (hwnd);
}

void FreeComm(HANDLE hInstance)
{ 
	UnregisterClass(szarCommAppName, (HINSTANCE)hInstance);
}

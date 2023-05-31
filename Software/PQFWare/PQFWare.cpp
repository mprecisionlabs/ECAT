///////////////////////////////////////////////////////////////////////////////
// PQFWare.cpp : Defines the entry point for the application.
//
///////////////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "PqfApp.h"

static HWND dlgMain = NULL;
static CHAR szAppName[] = "PQFWare";

// The one and only application object
CPqfApp theApp(szAppName);

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
int APIENTRY WinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPSTR     lpCmdLine,
                     int       nCmdShow)
{
	if (!theApp.InitInstance(hInstance, NULL, lpCmdLine))
		return -1;

	int iRet = theApp.Run();
	
	theApp.ExitInstance();
	return iRet;
}



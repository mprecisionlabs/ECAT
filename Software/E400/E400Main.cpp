///////////////////////////////////////////////////////////////////////////////
// E400Main.cpp : Defines the entry point for the E400 application.
//
///////////////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "E400App.h"

static CHAR szAppName[] = "BurstWare";

// The one and only application object
CE400App theApp(szAppName);

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
int APIENTRY WinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPSTR     lpCmdLine,
                     int       nCmdShow)
{
	if (!theApp.InitInstance(hInstance, IDC_E400_ACL, lpCmdLine))
		return -1;

	int iRet = theApp.Run();
	
	theApp.ExitInstance();
	return iRet;
}



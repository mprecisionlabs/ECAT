///////////////////////////////////////////////////////////////////////////////
// E500Main.cpp : Defines the entry point for the E500 application.
//
///////////////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "E500App.h"

static CHAR szAppName[] = "SurgeWare";

// The one and only application object
CE500App theApp(szAppName);

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
int APIENTRY WinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPSTR     lpCmdLine,
                     int       nCmdShow)
{
	if (!theApp.InitInstance(hInstance, IDC_E500_ACL, lpCmdLine))
		return -1;

	int iRet = theApp.Run();
	
	theApp.ExitInstance();
	return iRet;
}



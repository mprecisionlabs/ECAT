//////////////////////////////////////////////////////////////////////
// PqfApp.h: interface for the CPqfApp class.
//
//////////////////////////////////////////////////////////////////////
#if !defined(AFX_PQFAPP_H__C7FB4CAA_672E_11D6_B321_00B0D0DE3525__INCLUDED_)
#define AFX_PQFAPP_H__C7FB4CAA_672E_11D6_B321_00B0D0DE3525__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "..\core\EcatApp.h"

class CPqfApp : public CEcatApp  
{
public:
	CPqfApp(LPCSTR szAppName);
	virtual ~CPqfApp();

public:
	virtual BOOL InitInstance(HINSTANCE hInstance, UINT uiAccelId, LPCSTR lpszCmdLine);
	virtual BOOL CreateMainWnd();
	virtual BOOL ReadCalFile();
	virtual void ProcessCmdLine(int argc, char** argv);

private:
	BOOL CheckProfileStringInt(LPSTR app, LPSTR key, BOOL bDefault, LPSTR inifilespec);

};

extern CPqfApp theApp;

#endif // !defined(AFX_PQFAPP_H__C7FB4CAA_672E_11D6_B321_00B0D0DE3525__INCLUDED_)

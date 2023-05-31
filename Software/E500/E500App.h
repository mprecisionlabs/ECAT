//////////////////////////////////////////////////////////////////////
// E500App.h: interface for the CE500App class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_E400APP_H__F9280402_76D1_11D6_B321_00B0D0DE3525__INCLUDED_)
#define AFX_E400APP_H__F9280402_76D1_11D6_B321_00B0D0DE3525__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "..\core\EcatApp.h"

class CE500App : public CEcatApp  
{
public:
	CE500App(LPCSTR szAppName);
	virtual ~CE500App();

public:
	virtual BOOL InitInstance(HINSTANCE hInstance, UINT uiAccelId, LPCSTR lpszCmdLine);
	virtual BOOL CreateMainWnd();
	virtual BOOL ReadCalFile();
	virtual void ProcessCmdLine(int argc, char** argv);

public:
	// Icons and cursors
	static HICON		pericon, freqicon, duricon, vicon, swicon, vpicon, 
						pvicon, mvicon, mswicon, repicon, phicon, waicon;


	static HCURSOR		percursor, freqcursor, durcursor, phcursor, vcursor, wacursor,
						swcursor, vpcursor, pvcursor, mvcursor, repcursor, phasecursor;

public:
	BOOL POWER_USER;

};

extern CE500App theApp;

#endif // !defined(AFX_E400APP_H__F9280402_76D1_11D6_B321_00B0D0DE3525__INCLUDED_)

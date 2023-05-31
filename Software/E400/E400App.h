//////////////////////////////////////////////////////////////////////
// E400App.h: interface for the CE400App class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_E400APP_H__F9280402_76D1_11D6_B321_00B0D0DE3525__INCLUDED_)
#define AFX_E400APP_H__F9280402_76D1_11D6_B321_00B0D0DE3525__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "..\core\EcatApp.h"

class CE400App : public CEcatApp  
{
public:
	CE400App(LPCSTR szAppName);
	virtual ~CE400App();

public:
	virtual BOOL InitInstance(HINSTANCE hInstance, UINT uiAccelId, LPCSTR lpszCmdLine);
	virtual BOOL CreateMainWnd();
	virtual BOOL ReadCalFile();

public:
	// Icons and cursors
	static HICON		pericon, freqicon, duricon, vicon, swicon, vpicon, pvicon, mvicon, mswicon, repicon, phaseicon;
	static HCURSOR		percursor, freqcursor, durcursor, phcursor, vcursor, swcursor, vpcursor, pvcursor, mvcursor, repcursor, phasecursor;
};

extern CE400App theApp;

#endif // !defined(AFX_E400APP_H__F9280402_76D1_11D6_B321_00B0D0DE3525__INCLUDED_)

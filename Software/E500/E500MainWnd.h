// E500MainWnd.h: interface for the CE500MainWnd class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_E500MAINWND_H__F9280403_76D1_11D6_B321_00B0D0DE3525__INCLUDED_)
#define AFX_E500MAINWND_H__F9280403_76D1_11D6_B321_00B0D0DE3525__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "..\core\EcatMainWnd.h"

class CE500MainWnd : public CEcatMainWnd  
{
public:
	CE500MainWnd();
	virtual ~CE500MainWnd();

public:
	virtual int OnCreate(HWND hWnd, LPCREATESTRUCT lpCS);
	virtual int OnCmdMsg(UINT uiNotify, UINT uiCtrlId, HWND hwndCtl);
	virtual LPCSTR GetHelpFileName() {return "surge.hlp";}

};

#endif // !defined(AFX_E500MAINWND_H__F9280403_76D1_11D6_B321_00B0D0DE3525__INCLUDED_)

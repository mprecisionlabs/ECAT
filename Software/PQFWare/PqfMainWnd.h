// PqfMainWnd.h: interface for the CPqfMainWnd class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PQFMAINWND_H__1D553741_6BCD_11D6_B321_00B0D0DE3525__INCLUDED_)
#define AFX_PQFMAINWND_H__1D553741_6BCD_11D6_B321_00B0D0DE3525__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "..\core\EcatMainWnd.h"

class CPqfMainWnd : public CEcatMainWnd  
{
public:
	CPqfMainWnd();
	virtual ~CPqfMainWnd();

public:
	virtual int OnCreate(HWND hWnd, LPCREATESTRUCT lpCS);
	virtual BOOL DoExit();
	virtual int OnCmdMsg(UINT uiNotify, UINT uiCtrlId, HWND hwndCtl);

	// Individual window procedure
	virtual LRESULT CALLBACK LocalWndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
	virtual LPCSTR GetHelpFileName() {return "PQF.hlp";}

private:
	inline HWND GetGraphHWND() {return GetDlgItem(GetDlgHWND(), IDD_PQF_GRAPH);}
};

#endif // !defined(AFX_PQFMAINWND_H__1D553741_6BCD_11D6_B321_00B0D0DE3525__INCLUDED_)

// EcatMainWnd.h: interface for the CEcatMainWnd class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ECATMAINWND_H__C7FB4CA9_672E_11D6_B321_00B0D0DE3525__INCLUDED_)
#define AFX_ECATMAINWND_H__C7FB4CA9_672E_11D6_B321_00B0D0DE3525__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CEcatMainDlg;
class CEcatMainWnd  
{
public:
	CEcatMainWnd();
	virtual ~CEcatMainWnd();

public:
	HWND GetHWND() {return m_hWnd;}
	HWND GetDlgHWND();
	CEcatMainDlg* GetDlgPtr() {return m_pDlg;}

	virtual BOOL Create(HINSTANCE hInstance, LPCSTR lpszAppName);
	virtual int OnCreate(HWND hWnd, LPCREATESTRUCT lpCS);
	virtual void OnDestroy();
	virtual int OnCmdMsg(UINT uiNotify, UINT uiCtrlId, HWND hwndCtl);
	virtual BOOL DoExit();
	virtual LPCSTR GetHelpFileName() {return "";}
	LPCSTR GetHelpFilePath();

	// Individual window procedure
	virtual LRESULT CALLBACK LocalWndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

protected:
	virtual BOOL Register(HINSTANCE hInstance, LPCSTR lpszAppName);

private:
	HWND m_hWnd;
	char m_strHelpFilePath[MAX_PATH];

protected:
	CEcatMainDlg* m_pDlg;

};

#endif // !defined(AFX_ECATMAINWND_H__C7FB4CA9_672E_11D6_B321_00B0D0DE3525__INCLUDED_)

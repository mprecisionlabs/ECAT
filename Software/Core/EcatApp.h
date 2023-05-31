//////////////////////////////////////////////////////////////////////
// EcatApp.h: interface for the CEcatApp class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ECATAPP_H__C7FB4CA8_672E_11D6_B321_00B0D0DE3525__INCLUDED_)
#define AFX_ECATAPP_H__C7FB4CA8_672E_11D6_B321_00B0D0DE3525__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define WM_ENABLE_RUN		(WM_USER + 1)
#define WM_CHANGE_TITLE		(WM_USER + 2)
#define WM_ADD_CAL			(WM_USER + 3)

//////////////////////////////////////////////////////////////////////
// This is the base application class for all three ECAT apps. Since they
// all have the same interface (dialog in the center of the screen) and they
// get initialized similarily, we'll use the common ancestor for all three 
// apps with some virtual functions to override in descendands. 
//
// The structure of the apps is as follows:
// CEcatApp (and descendands)
//		|
//		--- CEcatMainWnd (and descendands)
//				|
//				--- CEcatMainDlg (and descendands)
//
// To tailor the classes, derive from the base classes and override virtual 
// functions. For example, PQFWare has three main derivatives: CPqfApp, 
// CPqfMainWnd, CPqfMainDlg with customized functionality.
//////////////////////////////////////////////////////////////////////
class CEcatMainWnd;
class CEcatApp  
{
public:
	CEcatApp(LPCSTR szAppName);
	virtual ~CEcatApp();

public:
	virtual BOOL InitInstance(HINSTANCE hInstance, UINT uiAccelId, LPCSTR lpszCmdLine);
	virtual void ExitInstance();
	virtual int Run();

	virtual void ProcessCmdLine(int argc, char** argv);

	// Placeholder, should be overwritten in the derived classes
	virtual BOOL ReadCalFile() = NULL;
	virtual BOOL CreateMainWnd();

	BOOL StartComm();
	void StopComm();

	static LPCSTR GetAppName() {return m_szAppName;}
	static LPCSTR GetAppDir();
	static UINT CEcatApp::WM_REG_ECAT;

	// Static helpers for working with ICONS/CURSORS
	HICON LoadIcon(HINSTANCE hInst, UINT uiResID, BOOL bMapColors = TRUE);
	static void LoadIcon(HINSTANCE hInst, UINT uiResID, HICON& hIcon);
	static void LoadCursor(HINSTANCE hInst, UINT uiResID, HCURSOR& hCursor);

	static void BeginWaitCursor();
	static void EndWaitCursor();

	void CreateCommLinkDlg();
	void DestroyCommLinkDlg();
	void ShowCommLinkDlg(BOOL bShow);

	BOOL IsDdeFile() {return (m_strFileName[0] != '\0');}
	LPCSTR GetDdeFileName() {return (LPCSTR) m_strFileName;}

private:
	void ProcessCmdLine(LPCSTR lpszCmdLine);

private:
	HACCEL m_hAccelTable;
	int m_iComPort;
	DWORD m_dwBaudRate;
	HWND m_hCommLnkDlg;
	static char m_szAppName[MAX_PATH];
	static HCURSOR st_prevCursor;

protected:
	CEcatMainWnd* m_pMainWnd;
	char m_strFileName[MAX_PATH + 1];

};

extern HINSTANCE GetInst();

#endif // !defined(AFX_ECATAPP_H__C7FB4CA8_672E_11D6_B321_00B0D0DE3525__INCLUDED_)

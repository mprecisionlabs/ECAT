//////////////////////////////////////////////////////////////////////
// EcatMainDlg.h: interface for the CEcatMainDlg class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ECATMAINDLG_H__FC43FEA2_6BDC_11D6_B321_00B0D0DE3525__INCLUDED_)
#define AFX_ECATMAINDLG_H__FC43FEA2_6BDC_11D6_B321_00B0D0DE3525__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define	OFF						0
#define	ON						1
#define	ENABLE					1
#define	DISABLE					0
#define	TIMER1					1
#define	TIMER2					2

#define IDLE_MODE				0
#define RUN_MODE				1
#define STEP_MODE				2
#define WAIT_MODE				3
#define PAUSE_MODE				4
#define DELAY_MODE				5

#include "tname.h"

//////////////////////////////////////////////////////////////////////
// Base class for the ECAT application's main dialog
//////////////////////////////////////////////////////////////////////
class CEcatMainDlg  
{
public:
	CEcatMainDlg();
	virtual ~CEcatMainDlg();

public:
	inline HWND GetHWND() {return m_hWnd;}
	inline HWND GetParent() {return (m_hWnd == NULL) ? NULL : ::GetParent(m_hWnd);}
	BOOL SetItemText(UINT uiItemId, LPCTSTR lpText) {return SetDlgItemText(m_hWnd, uiItemId, lpText);}
	LPCTSTR GetItemText(UINT uiItemId);
	LRESULT SendItemMsg(UINT uiItemId, UINT uiMsg, WPARAM wParam, LPARAM lParam);
	HWND GetDlgItem(UINT uiItemId) {return ::GetDlgItem(m_hWnd, uiItemId);}	
	virtual BOOL Create(UINT uiResId, HWND hParent);

	void RedrawWindow(HWND hWnd);

	// Message handlers
	virtual int OnCmdMsg(UINT uiNotify, UINT uiCtrlId, HWND hwndCtl);
	virtual BOOL OnInitDialog(HWND hWnd);
	virtual void OnTimer(UINT uiTimerId);
	virtual void OnDestroy();
	virtual BOOL DoExit();

	// Individual dialog procedure
	virtual LRESULT CALLBACK LocalDlgProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

	void ShowOutOfMemoryDlg() {MessageBox(m_hWnd, "Not enough memory", "Fatal error", MB_OK | MB_ICONERROR);}
	void Convert_Seconds(LONG secs,	char *buffer);

	void DoFileNew();
	void DoFileOpen();
	void OpenFile();
	void DoFileSave();
	void DoFileSaveAs();

	// Virtual overrides for the derived classes
	virtual void UpdateTitle(LPSTR lpszTitle = NULL);
	virtual BOOL Load(HANDLE hFile) {return !dlgCtl::load_objects(hFile);};
	virtual void Save(HANDLE hFile) {};
	virtual void DeleteContext() {};
	virtual void BeforeFileParse() {};
	virtual void OnFileNew() {};
	virtual void OnFileOpen() {};
	virtual void OnFileSave(BOOL bSaveAs) {};
	virtual void OnChangeName();

	virtual void PostCalibrate() {};

	BOOL StartTimer(UINT uiTimerId, UINT uiDelay);
	void StopTimer();

protected:
	HBRUSH yBrush;
	UINT m_uiTimerID;

private:
	HWND m_hWnd;

public:
	TESTNAME	Tname;
	BOOL		changed_since_save;
	int			change_shown;
	LONG		EUT_POWER;
	int			nRunMode;
	long		Last_State;
	long		This_State;
};

#endif // !defined(AFX_ECATMAINDLG_H__FC43FEA2_6BDC_11D6_B321_00B0D0DE3525__INCLUDED_)

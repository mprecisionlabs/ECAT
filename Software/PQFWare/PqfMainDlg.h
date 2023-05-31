//////////////////////////////////////////////////////////////////////
// PqfMainDlg.h: interface for the CPqfMainDlg class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PQFMAINDLG_H__FC43FEA3_6BDC_11D6_B321_00B0D0DE3525__INCLUDED_)
#define AFX_PQFMAINDLG_H__FC43FEA3_6BDC_11D6_B321_00B0D0DE3525__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "..\core\EcatMainDlg.h"
#include "..\core\MultiStateBtn.h"
#include "common.h"

// Save/Load exception class
class CSaveLoadEx
{
public:
    CSaveLoadEx() {};
    virtual ~CSaveLoadEx() {};
};

class CPqfMainDlg : public CEcatMainDlg  
{
public:
	CPqfMainDlg();
	virtual ~CPqfMainDlg();

public:
	virtual int OnCmdMsg(UINT uiNotify, UINT uiCtrlId, HWND hwndCtl);
	virtual BOOL OnInitDialog(HWND hWnd);
	virtual void OnTimer(UINT uiTimerId);
	virtual void OnDestroy();

	virtual LRESULT CALLBACK LocalDlgProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

	// Overrides to work with PQF test plan files
	virtual BOOL Load(HANDLE hFile);
	virtual void Save(HANDLE hFile);
	virtual void DeleteContext();

private:
	// List veiw handling helpers
	void InsertColumn(int nCol, LPTSTR lpszHeading, int nWidth, int nFormat = LVCFMT_LEFT);
	int GetSelItem();
	int GetListItemCnt();
	int GetNextListItem(int iItem, int iFlags);
	LPPQFSTEP GetListItemData(int iItem);
	void SetListItemData(int iItem, LPPQFSTEP pStep);
	void SwapListItems(BOOL bMoveUp);
	void SelectListItem(int iItem);
	
	// helpers - begin
	BOOL Text2Int(UINT uiCtrlId, int& iVal);
	BOOL Text2Int(LPSTR lpszVal, int& iVal);
	void Int2Text(UINT uiCtrlId, int iVal);
	BOOL Text2Dbl(UINT uiCtrlId, double& dVal);
	void Dbl2Text(UINT uiCtrlId, const double& dVal);
	BOOL Text2Dur();
	void Dur2Text();

	void UpdateDbl(UINT uiWndId, int& iHost, int iMin, int iMax, BOOL bInc);
	void UpdateInt(UINT uiWndId, int& iHost, int iMin, int iMax, BOOL bInc);
	// helpers - end

	void UpdateStart(LPNMUPDOWN lpNMH);
	void UpdateDuration(LPNMUPDOWN lpNMH);
	void UpdateDurUnits(LPNMUPDOWN lpNMH);
	void UpdateRmsI(LPNMUPDOWN lpNMH);
	void UpdatePeakI(LPNMUPDOWN lpNMH);
	void UpdateTestCtrls();

	BOOL IsTestTooLong(LPPQFSTEP pPqfStep = NULL);
	void InsertStep();
	void ReplaceStep();
	void CopyStep();
	void DeleteStep();
	void DeleteAllSteps(BOOL bAsk = FALSE);

	void UpdateData(BOOL bSaveAndValidate);
	void FillTestStep(PQFSTEP& pqfStep);
	void TestStep2ListItem(int iListItem, const PQFSTEP& pqfStep);

	void SendPqfIdleCmnd();
	void SetStandbyLevel(short shLevel);
	short GetStandbyLevel();

	void SetPhase(BYTE byPhase);
	BYTE GetPhase();

	BOOL GetRepeatFlag();
	void SetRepeatFlag(BOOL bRepeat);

	void PqfUpdateTestStatus(BOOL bStart);
	void PqfSetTotalTime(DWORD dwTotDegrees);
	void PqfCalcTimeLeft();
	void PqfDlgTestCompleted();
	DWORD PqfUtilStep2Degrees(LPPQFSTEP lpStep);
	DWORD PqfUtilStep2DurationDegrees(const PQFSTEP& pPqfStep);
	void PqfSetTimeLeft(DWORD dwTotDegrees, BOOL bClear = FALSE);
	void PqfTestNextStep();
	BOOL PqfValidateTest();

	BOOL Testmgr___ReadStatusExec (HWND hDlg, INTEGER caller, INTEGER idd_eutpower, INTEGER idd_eutcheat);
	WORD TestmgrCreateTest();
	void TestmgrDestroyTest();
	BOOL TestmgrReadStatus(HWND hDlg, INTEGER caller);
	void TestmgrUpdateEUTPower(HWND hDlg, INTEGER idd_eutpower, INTEGER idd_eutcheat);
	void TestmgrKludge();
	void TestmgrCheat();

	void RunTest();
	void StopTest();
	void Qualify();

	void EnableControls(BOOL bTesting);
	void ForceClosure();
	
	void SimulateStatusUpdate(BOOL bStart);
	void LogStatusUpdate();

private:
	CMultiStateBtn m_btnExit;
	CMultiStateBtn m_btnStop;
	CMultiStateBtn m_btnRun;
	CMultiStateBtn m_btnEut;

	// Test values
	int m_iStartPhase;
	int m_arrDur[PQF_NUM_DURATION_UNITS];	// to keep track of duration-unit pair for the situation
											// when the user is changing units
	int m_iDurUnits;
	int m_iRmsI;
	int m_iPeakI;
	BYTE m_byPhase;
	BOOL m_bRepeatTest;
	BOOL m_bQualifying;
};

#endif // !defined(AFX_PQFMAINDLG_H__FC43FEA3_6BDC_11D6_B321_00B0D0DE3525__INCLUDED_)

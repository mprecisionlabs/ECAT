//////////////////////////////////////////////////////////////////////
// E500MainDlg.h: interface for the CE500MainDlg class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_E500MAINDLG_H__F9280404_76D1_11D6_B321_00B0D0DE3525__INCLUDED_)
#define AFX_E500MAINDLG_H__F9280404_76D1_11D6_B321_00B0D0DE3525__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "..\core\EcatMainDlg.h"
#include "..\core\MultiStateBtn.h"
#include "..\core\dlgctl.h"
#include "..\core\scroll.h"
#include "..\core\msec.h"
#include "..\core\block.h"
#include "..\core\message.h"
#include "..\core\duals.h"
#include "..\core\phase.h"
#include "..\core\waves.h"
#include "..\core\meas.h"
#include "..\core\cpls.h"
#include "..\core\REPEAT.H"
#include "..\core\POLARITY.H"
#include "..\core\acmeas.h"

class CE500MainDlg : public CEcatMainDlg, public dlgCtl   
{
public:
	CE500MainDlg();
	virtual ~CE500MainDlg();

public:
	virtual int OnCmdMsg(UINT uiNotify, UINT uiCtrlId, HWND hwndCtl);
	virtual BOOL OnInitDialog(HWND hWnd);
	virtual void OnTimer(UINT uiTimerId);

	virtual LRESULT CALLBACK LocalDlgProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

	virtual void DeleteContext();
	virtual void Save(HANDLE hFile) {SaveText(hFile);}
	virtual void BeforeFileParse();
	virtual void PostCalibrate();

private:
	// Buttons
	CMultiStateBtn m_btnExit;
	CMultiStateBtn m_btnStop;
	CMultiStateBtn m_btnRun;
	CMultiStateBtn m_btnStep;
	CMultiStateBtn m_btnEut;

private:
	char		Levels[10];
	BOOL		chirp_hide;
	long		Last_State;
	long		This_State;
	int			charge_failed;
	LONG		steps_taken;
	LONG		LastTime;
	LONG		total_steps;
	LONG		LastTest;
	BOOL		changed_while_running;
	DWORD 		LastTick;
	long		Run_time;
	long		Wait_time;
	BOOL		PAUSE;
	BOOL		IL_PAUSE;
	BOOL		vfix;
	BOOL		dfix;
	BOOL		ffix;
	BOOL		pfix;
	BOOL		swfix;
	BOOL		wafix;
	LONG		mv;
	int			polcnt;
	int			repcnt;
	int			pol_type;
	LONG		dcnt;
	LONG		swcnt;
	LONG		wacnt;
	LONG		vcnt;
	LONG		Desired_Charge_Mode;
	LONG		Actual_Charge_Mode;
	LONG		Wait_mul;
	DWORD		Time_elapsed;
	int			Old_Min_Time;
	LONG		PauseWait;

	HWND		Block_Window;
	Scroll_Pair Wait_Pair;
	DUALS		VoltageSet;
	WAVE		WaveMode;
	PHASE		PhaseSet;
	REPEAT		Repeat;
	POLARITY	PolaritySet;
	MEAS		MeasSet;
	BOOL		SET_READY;
	ACMEAS		AcmeasSet;
	COUPLE		CoupleMode;

	BOOL IS_E52x;					// Change from IS_E522;  JFL 3/4/96

	// Overrides
	void Clear(void);
	virtual int SaveText(HANDLE hFile);
	int Process_Line(LPSTR);
	
	// Some garbage for backward compatibility
	BOOL fixed()            {return 1;}
	LONG ReportSteps()      {return total_steps;}
	BOOL Set_Next_Step()    {return 0;}
	void Set_Update(BOOL b) {}
	void Show_Run(BOOL b)   {}
	void Set_Start_Values() {}

	// To make dlgCtl happy
	virtual BOOL DlgProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam) {return 0;};

	long GetAllowCouple();

};

#endif // !defined(AFX_E500MAINDLG_H__F9280404_76D1_11D6_B321_00B0D0DE3525__INCLUDED_)

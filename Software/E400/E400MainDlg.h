//////////////////////////////////////////////////////////////////////
// E400MainDlg.h: interface for the CE400MainDlg class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_E400MAINDLG_H__F9280404_76D1_11D6_B321_00B0D0DE3525__INCLUDED_)
#define AFX_E400MAINDLG_H__F9280404_76D1_11D6_B321_00B0D0DE3525__INCLUDED_

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
#include "..\core\cpls.h"
#include "..\core\REPEAT.H"
#include "..\core\POLARITY.H"
#include "..\core\acmeas.h"

class CE400MainDlg : public CEcatMainDlg, public dlgCtl   
{
public:
	CE400MainDlg();
	virtual ~CE400MainDlg();

public:
	virtual int OnCmdMsg(UINT uiNotify, UINT uiCtrlId, HWND hwndCtl);
	virtual BOOL OnInitDialog(HWND hWnd);
	virtual void OnTimer(UINT uiTimerId);

	virtual LRESULT CALLBACK LocalDlgProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

	virtual void DeleteContext();
	virtual void PostCalibrate();
	virtual void Save(HANDLE hFile) {SaveText(hFile);}

private:
	// Buttons
	CMultiStateBtn m_btnExit;
	CMultiStateBtn m_btnStop;
	CMultiStateBtn m_btnRun;
	CMultiStateBtn m_btnStep;
	CMultiStateBtn m_btnEut;

private:
	char		Levels[10];
	HBRUSH		yBrush;
	BOOL		chirp_hide;
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
	BOOL		DC_MODE;
	BOOL		vfix;
	BOOL		dfix;
	BOOL		ffix;
	BOOL		pfix;
	BOOL		swfix;
	BOOL		wafix;
	BOOL		phfix;

	BOOL		Phase_Mode;
	int			polcnt;
	int			repcnt;
	int			pol_type;
	LONG		dcnt;
	LONG		fcnt;
	LONG		phcnt;
	LONG		pcnt;
	LONG		swcnt;
	LONG		vcnt;
	LONG		Wait_mul;
	LONG		Dur_mul;
	LONG		Repcnt;
	DWORD		Time_elapsed;

	HWND		Block_Window;
	Scroll_Pair Wait_Pair;
	Scroll_Pair Dur_Pair;
	DUALS		VoltageSet;
	DUALS		DurSet;
	DUALS		FreqSet;
	DUALS		PerSet;
	DUALS		PhaseSet;
	REPEAT		Repeat;
	POLARITY	PolaritySet;
	MSEC		msecSet;
	BOOL		SET_READY;
	ACMEAS		AcmeasSet;
	COUPLE		CoupleMode;

	// Some garbage for backward compatibility
	BOOL fixed()            {return 1;}
	LONG ReportSteps()      {return total_steps;}
	BOOL Set_Next_Step()    {return 0;}
	void Set_Update(BOOL b) {}
	void Show_Run(BOOL b)   {}
	void Set_Start_Values() {}

	// Overrides
	void Clear(void);
	virtual int SaveText(HANDLE hFile);
	int Process_Line(LPSTR);

	// To make dlgCtl happy
	virtual BOOL DlgProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam) {return 0;};

};

#endif // !defined(AFX_E400MAINDLG_H__F9280404_76D1_11D6_B321_00B0D0DE3525__INCLUDED_)

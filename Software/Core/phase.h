#ifndef PHASE_CLASS
#define PHASE_CLASS

#include "duals.h"

class PHASE : public DUALS
{
private:
	BOOL Allow_DC;
	BOOL allow_lineref;
	BOOL Three_Phase;
	BOOL curr_mode;
	BOOL allow_phase;
	UINT Update_Mode;
	void set_mode(int nmode);
	BOOL DlgProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);

public:
	PHASE(void) {allow_lineref=0; Allow_DC=FALSE; Three_Phase=FALSE; curr_mode=0; allow_phase=99;}
	virtual ~PHASE(){}
	virtual void  Refresh_Values();
	void redo_list();
	void Init(HANDLE hInstance,HWND hwnd, RECT& rect, HICON hicon,UINT start,UINT end,UINT init,int exp,int step,LPSTR text,char * unit_txt);
	void LineRef(BOOL dv) {if (dv==allow_lineref) return; allow_lineref=dv; redo_list(); set_mode(0);}
	void dc_possible(BOOL dv) {Allow_DC=dv; redo_list();}
	void three_phase(BOOL tp){Three_Phase=tp; redo_list();}
	void  Set_Message(UINT msg1,UINT msg2,HWND wnd){Update_Mode=msg2; DUALS::Set_Message(msg1,wnd);};
	void  Show_Run(BOOL state);
	void Set_Start_Values();
	void Clear(void);
	void Random_only(BOOL);
	BOOL  Set_Next_Step();
	LONG ReportSteps(void);
	BOOL fixed();
	virtual int  SaveText(HANDLE);
	int Process_Line(LPSTR);
};
#endif


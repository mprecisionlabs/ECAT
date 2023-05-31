#ifndef REPEAT_CLASS
#define REPEAT_CLASS

#include "file_ver.h"
#include "dlgctl.h"
#include "scroll.h"
#include "dlgctl.h"

class REPEAT :public dlgCtl
{
private:
	int count_spin;
	int count_edit;
	int count;
	int active_count;
	HWND edit_wnd;
	BOOL init;
	BOOL suspend_edit;
	BOOL suspend_spin;
	BOOL run_shown;
	BOOL DlgProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);
	void Value_Changed();

public:
	REPEAT(){init=FALSE;};
	void Init(HANDLE hInstance,HWND hwnd, RECT& rect,LPSTR text);
	BOOL fixed(){return (count==1);};
	LONG ReportSteps(void){return count;};
	BOOL  Set_Next_Step();
	void  Show_Run(BOOL state);
	void Set_Start_Values();
	void Clear(void);
	void Hide(BOOL);
	virtual int  SaveText(HANDLE);
	int Process_Line(LPSTR);
};
#endif

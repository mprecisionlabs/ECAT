#ifndef POLARITY_CLASS
#define POLARITY_CLASS

#include "file_ver.h"
#include "dlgctl.h"
#include "block.h"
#include "duals.h"
#include "scroll.h"
#include "dlgctl.h"

class POLARITY :public dlgCtl
{
private:
	DUALS *dual_ptr;
	int poltype;
	int steps;
	BOOL current_pol;
	int active_count;
	BOOL run_shown;
	float voltage_value;
	BOOL init;
	HICON pi,mi,vi;
	HWND Update_Window;
	WORD Update_Message;
	BOOL DlgProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);
	void Value_Changed();

public:
	POLARITY(){init=FALSE;};
	void Init(HANDLE hInstance,HWND hwnd, DUALS &dual,HICON picon,HICON micon, HICON vicon,LPSTR text);
	BOOL fixed(){return (poltype==0);};
	LONG ReportSteps(void){return steps;};
	BOOL  Set_Next_Step();
	void  Show_Run(BOOL state);
	void Set_Start_Values();
	void  Set_Message(UINT msg,HWND wnd){Update_Window=wnd; Update_Message=msg;};
	int Value(){return poltype;};
	void Clear(void);
	void Hide(BOOL);
	void  Refresh_Values();
	virtual int  SaveText(HANDLE);
	int Process_Line(LPSTR);
};
#endif

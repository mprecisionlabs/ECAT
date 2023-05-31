#ifndef MEAS_OBJ
#define MEAS_OBJ

#include "dlgctl.h"
#include "file_ver.h"

class far MEAS: public dlgCtl
{
private:
	HBRUSH yBrush;
	HBRUSH rBrush;
	HWND Update_Window;
	LONG Update_Message;
	BOOL Update_Values;
	BOOL run_shown;
	char GTS[4];
	LONG cp_value;
	UINT out_of_limits;
	LONG last_meas;
	LONG curr_meas_value;
	int  active_bay;
	int  last_bay;

	void Update_Measurments();
	BOOL DlgProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);
	BOOL IsAuto(int bay, int lines);

public:
	MEAS(){}
	void  Init(HANDLE ,HWND ,RECT&, LPSTR);
	LONG  ReportSteps(){return 0;}
	BOOL  fixed(){return 1;};
	void  Setup_Values(LONG);
	void  Show_Run(BOOL state);
	void  Set_Message(UINT msg,HWND wnd){Update_Window=wnd; Update_Message=msg;};
	void  Set_Update(BOOL updt){Update_Values=updt; };
	int   New_Numbers(int far *);
	void  Refresh_Values();
	void  Clear(void);
	virtual int   SaveText(HANDLE);
	int   Process_Line(LPSTR);
};
#endif

#ifndef MSEC_CLASS
#define MSEC_CLASS

#include "file_ver.h"
#include "dlgctl.h"
#include "block.h"
#include "duals.h"
#include "scroll.h"

class MSEC : public dlgCtl
{
private:
	DUALS *freq_ptr;
	DUALS *dur_ptr;
	float freq_value;
	float dur_value;
	BOOL init;
	BOOL run_shown;
	BOOL mSec_fixed;
	long is_pulses;
	HWND Update_Window;
	BOOL Update_Values;
	WORD Update_fMessage;
	WORD Update_dMessage;
	BOOL DlgProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);
	void Value_Changed();

public:
	MSEC(){init=FALSE;};
	void Init(HANDLE hInstance,HWND hwnd, DUALS &duald,DUALS &dualf,LPSTR text);
	BOOL fixed(){return 1;};
	LONG ReportSteps(void){return 1;};
	BOOL  Set_Next_Step(){return 1;};
	void  Show_Run(BOOL state){run_shown=state;};
	void Set_Start_Values(){};
	void  Set_Message(UINT msgd,UINT msgf,HWND wnd){Update_Window=wnd; Update_fMessage=msgf; Update_dMessage=msgd;};
	int Value(){return is_pulses; };
	void  Set_Update(BOOL updt){Update_Values=updt; };
	void mSec_Only(BOOL msonly);
	void Clear(void);
	void Hide(BOOL){};
	virtual int  SaveText(HANDLE);
	int Process_Line(LPSTR);
};
#endif


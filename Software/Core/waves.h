#ifndef WAVES
#define WAVES

#include "dlgctl.h"
#include "file_ver.h"

class WAVE: public dlgCtl
{
private:
	HICON icon;
	LONG number_of_steps;
	LONG mode;
	LONG Current_wave_Pos;
	LONG Last_Active_Wave;
	LONG This_Active_Wave;
	BOOL show;
	int  tmH;
	int  TlimitAC;
	int  VlimitAC;
	int  TlimitIO;
	int  VlimitIO;

	BOOL five_chan_io;
	BOOL allow_front;
	long allow_couple;

	HWND Update_Window;
	LONG Update_Message;
	BOOL Update_Values;
	LPFN newlpfn;
	LPFN oldlistlpfn;
	BOOL Change_Ok;
	int  cur_wave;
	int  cur_plugin;
	BOOL DlgProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);
	void Update_State();
	void Setup_Plugin(HWND hDlg,int IDD_WAVEU,int cur_plugin);
	void Scan_list();

public:

	WAVE(){}

	long AllowCouple() {return allow_couple; }
	BOOL AllowFront()  {return allow_front;  }
	BOOL FiveChanIo()  {return five_chan_io;}

	int  MaxVoltage(long cplmode);
	int  MinTime(long cplmode);   

	void Init(HANDLE ,HWND ,RECT&,HICON,LPSTR);
	LONG ReportSteps(void);
	BOOL fixed(){return (mode==0);};
	long process(HWND hwnd,UINT message, WPARAM wParam, LPARAM lParam);
	void Set_Start_Values();
	BOOL Set_Next_Step();
	LONG Get_Current_Values();
	void Show_Run(BOOL state);

	void Set_Message(UINT msg,HWND wnd){Update_Window=wnd; Update_Message=msg;};
	void Set_Update(BOOL updt){Update_Values=updt; };
	int  Copy_Net_Name(LPSTR copyto, int len);

	void Refresh_Values();
	void Clear(void);
	virtual int  SaveText(HANDLE);
	int  Process_Line(LPSTR);
	void Set_wave(int);
	int  Get_waven(LPSTR);
};
#endif

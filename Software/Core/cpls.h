#ifndef CPLS
#define CPLS
#include "dlgctl.h"
#include "file_ver.h"
#include "btn.h"
#include "MultiStateBtn.h"

const unsigned int DATA_OFF = 0;
const unsigned int DATA_ON  = 1;

const unsigned int CMODE_ASYMM = 0;
const unsigned int CMODE_SYMM  = 1;

const unsigned int CLAMP_20V  = 0;
const unsigned int CLAMP_220V = 1;
const unsigned int CLAMP_EXT  = 2;

int ValidCoupleMode(WORD set_cpl,BOOL Five_Chan_Io = TRUE,HWND hDlg=NULL);

class far COUPLE: public dlgCtl
{
private:
	LONG Last_Couple;
	LONG Last_Data_Couple;
	BOOL surge_app;
	LONG Current_Couple_Pos;
	int tmH;
	int tmW;
	HICON icon;
	LONG number_of_steps;
	LONG mode;
	BOOL show;
	HWND Update_Window;
	LONG Update_Message;
	BOOL Update_Values;

	BOOL Front_Only;
	long allow_couple;
	long selected_coupler;
	int  io_net;
	int  io_wave;

	TOGLE_BTN clamp_btn;
	TOGLE_BTN data_btn;
	TOGLE_BTN cmode_btn;

	char front_name[10];

	int local_1_x;
	int local_2_x;
	int local_3_x;
	int local_4_x;
	int local_5_x;
	int local_s_x;
	int local_W_x;

	int  HitRow;
	int  HitButton;
	int  Hit_x;
	BOOL Three_Phase;
	BOOL Five_Chan_Io;
	LPFN newlpfn;
	LPFN oldlistlpfn;
	BOOL Change_Ok;
	BOOL Captured;
	BOOL DlgProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);
	void Set_Button_State(HWND hDlg,LONG IDD_MASK,LONG BIT_MASK);

	void Next_State(LONG Bit_MASK,LONG &cpl);
	LONG show_cpl_display(LONG cp,LONG mask,LONG lParam, char *cl);
	int  valid_mode(LONG set_cpl);
	void Update_State();
	void Show_Valid();
	BOOL Set_Mode(int nmode,BOOL reset = TRUE);
	void InitModeComboBox();
	void InitCouplerComboBox();
	void InitCoupleControls();
	void CouplerChanged();
	void ShowIoControls();
	void SetModeFront();
	void SetModeList(HWND hDlg);
	void SelectCoupler(unsigned long type);
	long SelCouplerType();
	long SelectedCouplerType(){return selected_coupler;}
	void PrintfEcatOutAtIO();
	void MesureListItem(HWND hDlg, LPMEASUREITEMSTRUCT lpMIS);

public:
	COUPLE();
	~COUPLE() {};
	void  Init(HANDLE ,HWND ,RECT&,HICON,LPSTR,BOOL);
	BOOL  FrontPanel(void) {return (mode==0); };
	LONG  ReportSteps(void);
	BOOL  fixed() {return (mode<=1);};
	BOOL  front() {return (mode==0);};

	void  Set5IOLines(BOOL state);

	void  Clear(void);
	long  process(HWND hwnd,UINT message, UINT wParam, LONG lParam);
	void  Set_Start_Values();
	BOOL  Set_Next_Step();
	LONG  Get_Current_Values();
	void  Show_Run(BOOL state);
	void  Set_Message(UINT msg,HWND wnd){Update_Window=wnd; Update_Message=msg;};
	void  Set_Update(BOOL updt){Update_Values=updt; };
	void  Set_Both_Modes(BOOL DOit){surge_app=DOit;};
	void  Change_Icon(HICON hicon);
	void  SetFrontName(LPSTR name);

	void  SetFrontOnly(long only);
	void  SetAllowCouple(long state);

	long  CoupleType();

	void  Refresh_Values();
	BOOL  setvalid();
	virtual int   SaveText(HANDLE);
	int   Process_Line(LPSTR);

private:
	// Buttons
	// E4551
	CMultiStateBtn btnPE;
	CMultiStateBtn btnN;
	CMultiStateBtn btnL1;
	CMultiStateBtn btnL2;
	CMultiStateBtn btnL3;
	// E571
	CMultiStateBtn btnG;
	CMultiStateBtn btnS1;
	CMultiStateBtn btnS2;
	CMultiStateBtn btnS3;
	CMultiStateBtn btnS4;

};
#endif


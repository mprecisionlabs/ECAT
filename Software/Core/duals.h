
#ifndef DUAL_CLASS
#define DUAL_CLASS
#include "file_ver.h"
#include "dlgctl.h"

#include "scroll.h"
#include "dlgctl.h"

#define MODE_FIXED    0
#define MODE_LINEAR   1
#define MODE_LOG      2
#define MODE_LIST     3
#define MODE_NEW_LIST 4
#define MODE_F8014    5
#define MODE_CHIRP    6

class DUALS :public dlgCtl
{
protected:
	int expv;
	LONG startset;
	LONG endset;
	UINT initset;
	UINT expset;
	UINT stepset;

	BOOL step_type;
	BOOL linear_step_type;
	Scroll_Pair enterv;
	Scroll_Pair startv;
	Scroll_Pair stopv;
	Scroll_Pair singlev;
	HWND Update_Window;
	UINT Update_Message;
	int Current_List_Pos;
	int tmH;
	BOOL Update_Values;
	HICON icon;
	BOOL VList_Shown;
	BOOL List_Shown;
	BOOL Special_F8014;
	LONG Old_sel;
	int  Step_Delta;
	LONG Steps_Done;
	float  LogInc;
	float  Percent_Change;
	float  Display_Value;
	float  Present_Value;
	LONG number_of_steps;
	LONG mode;
	HBRUSH rBrush;
	HBRUSH gBrush;
	HBRUSH yBrush;
	BOOL hidden;
	char units[5];
	BOOL DlgProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);
	void Value_Changed();
	void load_limitcheck(long&);
	void setup_mode(int modev);

public:
	DUALS();
	virtual ~DUALS(){}
	void Init(HANDLE hInstance,HWND hwnd, RECT& rect, HICON hicon,UINT start,UINT end,UINT init,int exp,int step,LPSTR text,char * unit_txt);
	void AddToList(LPSTR);
	void ResetList(LPSTR);
	LONG ReportList(void){return(Old_sel);}
	void SetListStr(LPSTR);
	void SetListN(int);
	LONG ReportSteps(void);
	BOOL fixed();
	void Change_Icon(HICON hicon);
	void Set_Start_Values();
	BOOL  Set_Next_Step();
	float Get_Current_Values(){return Present_Value;};
	float Get_Max_Value();//Set while running not max allowed
	float Get_Min_Value();//Set min while running
	long  Get_Upper_Limit(){return endset;}		  // set with Set_Max_Value
	void  Show_Run(BOOL state);
	void  Set_Special_F8014(BOOL set, BOOL chirp);
	BOOL  Get_Special_F8014(){return (Special_F8014 && (mode==MODE_F8014));};
	void  Set_Value(float value);
	void  Set_Max_Value(float value); 
	void  Set_Min_Value(float value);
	BOOL  Get_Chirp_F8014(){return (Special_F8014 && (mode==MODE_CHIRP));};
	void  Change_State(UINT start,UINT end,UINT init,int exp,int step,LPSTR text,char * unit_txt);
	void  Set_Message(UINT msg,HWND wnd){Update_Window=wnd; Update_Message=msg;};
	void  Set_Update(BOOL updt){Update_Values=updt; };
	virtual void  Refresh_Values();
	void Clear(void);
	void Hide(BOOL);
	void Set_Units(LPSTR);
	virtual int  SaveText(HANDLE);
	int Process_Line(LPSTR);
};



#endif



#ifndef ACMEAS_OBJ
#define ACMEAS_OBJ

#include "dlgctl.h"
#include "scroll.h"

const int   AC_MAX            = 1000;
const int   CAL_RMS_LIMIT_OFF = 48; 	// index of default RMS lim in CALINFO.cal[]


// class for IDD_AC_LIMIT - limit edit dialog box
//
//	Execute() passes values for peak & RMS limits to data members
//           and executes dialog box, for which Scroll Pairs are 
//				 initialized with data member values.
// 			 After return from Execute() data member have new limit values

class far ACLIM
{
private:

	int  rms_min;
	int  rms_max;
	int  peak_max;
	int  def_lim;

	Scroll_Pair rmin_pair;
	Scroll_Pair rmax_pair;
	Scroll_Pair pmax_pair;

	static ACLIM *dlg;
	static BOOL __declspec(dllexport) CALLBACK  AcLimProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);

public:

	ACLIM(){dlg=this;}
	~ACLIM(){}

	void Execute(HANDLE hInstance,HWND hwnd,LPSTR name,int,int,int,int);
	int  GetRmsMin() const {return rms_min;}
	int  GetRmsMax() const {return rms_max;}
	int  GetPeakMax()const {return peak_max;}
};


// class for "AC_Measurements" dialog window
//


class far ACMEAS: public dlgCtl
{
private:

	HBRUSH yBrush;
	HBRUSH rBrush;
	BOOL   hidden;

	UINT   out_of_limits;			    // bit field, 1 bit for peak/RMS control

	int    rms_min;
	int    rms_max;						 // present RMS  limits
	int    peak_max;						 // present peak limit
	int    default_lim;					 // default RMS max limit, set in CALINFO
	int 	  module;						 // EUT Mains source module

	ACLIM  lim_dlg;						 // edit limits dialog

	BOOL DlgProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);

	void ShowLimits();					 // display limits
	void ShowNumbers(int *);			 // process & display new numbers
	void ShowError();					 // display error message box
	void PrintfLimits() const;		 // send limits to ECAT
	void UpdateSettings();				 // update AC_OPTION & check validity of limits
											 // display limits & send to ECAT
public:
	ACMEAS();

	void Hide(BOOL b);    				 // updates AC_OPTION, hide or show wnd

	void UpdateNumbers();				    // process new numbers received
	void UpdateEutSource();	          // after LOCAL, set ECAT to screen settings

	// Init() as dlgCtl, init module with coupler & UpdateSettings()
	void Init(HANDLE ,HWND ,RECT&, LPSTR,int coupler); 

	// update module & UpdateSettings(), called after calibration dialog
	void Update(int coupler){module = coupler;UpdateSettings();}

	void Clear(void);											   // clear for new test
	virtual int  SaveText(HANDLE);	   									// save settings to file
	int  Process_Line(LPSTR);									// restore after SaveText
};


#endif

#ifndef TNAME     
#define TNAME     
#include <string.h>
#include "dlgctl.h"

class far TESTNAME
{
public:

	TESTNAME(){dlg=this;name[0] = 0;}
	~TESTNAME(){}

	int  Execute(HINSTANCE hinst,HWND hwnd,LPSTR name);
	void GetText(LPSTR text) const {strcpy(text,name);}	   		
	void SetText(const LPSTR text) {name[0] = 0;if (text)_fstrncpy(name,text,79);}	   		
	void Clear(void){name[0] = 0;}						

private:
	char name[80];

	static TESTNAME *dlg;
	static BOOL __declspec(dllexport) CALLBACK TestNameProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);
};

#endif

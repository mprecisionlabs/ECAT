#include "stdafx.h"
#include "tname.h"

TESTNAME *TESTNAME::dlg;

int TESTNAME::Execute(HINSTANCE hinst,HWND hwnd,LPSTR name)
{
	return  DialogBox(hinst,name,hwnd,(DLGPROC)(TESTNAME::TestNameProc));
}

BOOL __declspec(dllexport) CALLBACK TESTNAME::TestNameProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{ 
	case WM_INITDIALOG:
		SetDlgItemText(hDlg,IDD_NAME_TEXT,dlg->name);
		SetWindowText(hDlg,"Test Name");    	// window title
		return TRUE;

	case WM_COMMAND:                                                     
		switch (LOWORD(wParam))                                                     
		{                                                                  
		case IDCANCEL:			       			// Cancel button pressed
			EndDialog(hDlg,TRUE);
			return TRUE;

		case IDOK:								// Ok button pressed
			if (!GetDlgItemText(hDlg,IDD_NAME_TEXT,dlg->name,79))
			  dlg->name[0] = 0;
			EndDialog(hDlg, TRUE);			    // close dialog
			return TRUE;

		default:
			break;
		}
	
	default:
		break;
	}

	return FALSE;
}


#include "stdafx.h"
#include "globals.h"
#include "kt_comm.h"
#include "dlgctl.h"

BOOL __declspec(dllexport) CALLBACK PauseProc(HWND hMainDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	BOOL bStop = FALSE;
	BOOL bContTest = FALSE;
	switch (message)
	{
	case WM_INITDIALOG:
		EnableWindow(GetDlgItem(hMainDlg, IDC_EDIT_LOG), Log.log_on());
		return TRUE;

	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case IDC_BUT_RUN:
			// If RUN button is pressed then continue running the test
			bStop = TRUE;
			bContTest = TRUE;
			break;

		case IDC_BUT_STOP:
		   // If STOP button is pressed then abort the test
			bStop = TRUE;
			break;
		}
		break;

	case KT_ILOCK_ABORT_RUN:
		// if ABORT message from interlock error dialog then abort the test
		bStop = TRUE;
		*((BOOL*) lParam) = TRUE;
		break;

	default:
		break;
	}

	if (bStop)
	{
		Log.LogItem( GetDlgItem( hMainDlg, IDC_EDIT_LOG ));
		EndDialog( hMainDlg, bContTest );
		return TRUE;
	}

	return FALSE;
}

BOOL Do_Pause(HWND hMainDlg)
{
	if (Log.log_on()) Log.loglpstr("Operator Paused Test\n");
	EnableWindow(hMainDlg, FALSE);
	BOOL rv = DialogBox(ghinst, MAKEINTRESOURCE(IDD_PAUSE), hMainDlg, (DLGPROC) PauseProc);
	EnableWindow(hMainDlg, TRUE);
	if (Log.log_on())
		Log.loglpstr((rv) ? "Operator Continues Test\n" : "Operator Aborted Test\n");

	return rv;
}


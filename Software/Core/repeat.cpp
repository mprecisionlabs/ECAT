#include "stdafx.h"
#include "repeat.h"
#include "dlgctl.h"
#include "message.h"

void REPEAT::Init(HANDLE hInstance,HWND hwnd, RECT& rect, LPSTR text)
{
	suspend_spin=FALSE;
	suspend_edit=FALSE;
	edit_wnd = NULL;
	create(hInstance,hwnd,rect,MAKEINTRESOURCE(IDD_REPEATDLG));
	ShowWindow(hDlgChild,SW_SHOWNORMAL);
	strcpy(Obj_Name,text);
	created=TRUE;
}

void REPEAT::Clear(void)
{ 
	SendDlgItemMessage(hDlgChild,IDD_SPIN,UDM_SETPOS,0,MAKELONG((short) 1, 0));
	SetDlgItemInt(hDlgChild,IDD_EDIT,1,0);
	ichanged=TRUE;
}

BOOL REPEAT::DlgProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{ 
	int ok;
	switch (message)
	{
	case WM_INITDIALOG:
		count_spin=count_edit=count=1;
		edit_wnd=GetDlgItem(hDlg,IDD_EDIT);
		SendDlgItemMessage(hDlg,IDD_SPIN,UDM_SETRANGE32,1,9999);
		SetDlgItemInt(hDlg,IDD_EDIT,1,0);
		SendDlgItemMessage(hDlg,IDD_SPIN,UDM_SETPOS,0,MAKELONG((short) 1, 0));
		init=TRUE;
		return TRUE;

	// See, if our spin control is communicating
	case WM_NOTIFY:
		if (!init) return TRUE;
		if (wParam == IDD_SPIN)
		{
			LPNMUPDOWN pnmh = (LPNMUPDOWN) lParam;
			if (pnmh->hdr.code == UDN_DELTAPOS)
			{
				// Spin control just got clicked. Disallow the change if 
				// the spin is suspended
				if (suspend_spin) return TRUE;

				count = pnmh->iPos + pnmh->iDelta;
				if (count < 1) count = 1;
				suspend_edit = TRUE;
				ichanged = TRUE;
				SetDlgItemInt(hDlg, IDD_EDIT, count, 0);
				suspend_edit = FALSE;
				return TRUE;
			}
		}
		break;

	case WM_COMMAND:
		switch (LOWORD(wParam))
		{ 
		case IDD_EDIT:
			// Check for notification messages from the edit box
			switch (HIWORD(wParam))
			{
			case EN_KILLFOCUS:
				return TRUE;

			case EN_CHANGE:
				if (suspend_edit) 
					return TRUE;
				
				count_edit=GetDlgItemInt(hDlg,IDD_EDIT,&ok,0);
				if (!ok) 
					SetDlgItemInt(hDlg,IDD_EDIT,count,0);
				else
				{
					count=count_edit;
					suspend_spin=TRUE;
					ichanged=TRUE;
					SendDlgItemMessage(hDlg,IDD_SPIN,UDM_SETPOS,0,MAKELONG((short) count, 0));
					suspend_spin=FALSE;
				}

				return TRUE;
			}//Edit lParam
			break;//IDD_EDIT

		}//Switch wParam for WM_COMMAND
		break;
	}

	return FALSE;
}

void REPEAT::Show_Run(BOOL state)
{
	run_shown=state;
	if (!state) 
		PrintfDlg(hDlgChild,IDD_REPEAT_LAB,"Repeat");
	else 
		PrintfDlg(hDlgChild,IDD_REPEAT_LAB,"Repeat: %d",active_count);
}

void REPEAT::Set_Start_Values()
{
	active_count=1;
	PrintfDlg(hDlgChild,IDD_REPEAT_LAB,"Repeat: %d",active_count);
}

BOOL REPEAT::Set_Next_Step()
{
	active_count++;
	PrintfDlg(hDlgChild,IDD_REPEAT_LAB,"Repeat: %d",active_count);
	return (active_count>count);
}

void REPEAT::Hide(BOOL b)
{
	if (!b)
		ShowWindow(hDlgChild,SW_SHOWNORMAL);
	else
		ShowWindow(hDlgChild,SW_HIDE);
}

int REPEAT::SaveText(HANDLE hfile)
{
	fhprintf(hfile,"%Fs:%d\r\n",(LPSTR)Obj_Name,(int)count);
	return 1;
}

int REPEAT::Process_Line(LPSTR buffer)
{
	long temp;
	char lbuff[30];
	_fstrncpy((LPSTR)lbuff,buffer,20);
	if (sscanf(lbuff,"%d",&temp)!=1)
	{
		Message("Unknow String to Parse!","Unknown:\n%Fs",buffer);
		return 1;
	}
	SendDlgItemMessage(hDlgChild,IDD_SPIN,UDM_SETPOS,0,MAKELONG((short) temp, 0));
	SetDlgItemInt(hDlgChild,IDD_EDIT,temp,0);
	ichanged=TRUE;
	return 0;
}

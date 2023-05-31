#include "stdafx.h"
#include "dlgctl.h"
#include "phase.h"
#include "message.h"

#define PHASE_LIST_CHANGED		(WM_USER+10)
#define IDD_TEXT_LO				122
#define DC_MODE					(curr_mode==4)

void PHASE::redo_list()
{
	ResetList("Ref:");
	AddToList("Rnd");
	if (allow_lineref)
	{
		AddToList("L1");
		if (Three_Phase)
		{
			AddToList("L2");
			AddToList("L3");
		}
	}

	if (Allow_DC)
		AddToList("DC");
}

void PHASE::Init(HANDLE hInstance,HWND hwnd, RECT& rect, HICON hicon,UINT start,UINT end,UINT init,int exp,int step,LPSTR text,char * unit_txt)
{
	DUALS::Init(hInstance,hwnd,rect,hicon,start,end,init,exp,step,text,unit_txt);
	redo_list();
	Notify(PHASE_LIST_CHANGED,(long)hDlgChild);
	curr_mode=99;
	PostMessage(hDlgChild,PHASE_LIST_CHANGED,0,0L);
}

BOOL PHASE::DlgProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{  
	int nmode;
	if (message!=PHASE_LIST_CHANGED) 
		return (DUALS::DlgProc(hDlg, message, wParam, lParam));

	nmode=lParam;
	if (Allow_DC) 
	{ 
		if (((!allow_lineref) && (lParam==1)) || (!Three_Phase && (lParam==2))) 
			nmode=4;
		else 
			nmode=lParam;
	}

	if (nmode!=curr_mode) 
		set_mode(nmode);

	return TRUE;
}

LONG PHASE::ReportSteps(void)
{
	if ((curr_mode==0) || (curr_mode==4))
		return 1;
	else
		return DUALS::ReportSteps();
}

BOOL PHASE::fixed()
{
	if ((curr_mode==0) || (curr_mode==4))
		return 1;
	else
		return DUALS::fixed();
}

void PHASE::Set_Start_Values()
{
	if (!allow_phase) return;

	SendMessage(Update_Window,Update_Mode,0,curr_mode);
	if ((curr_mode==0) || (curr_mode==4)) return;
	DUALS::Set_Start_Values();
}

BOOL  PHASE::Set_Next_Step()
{
	if ((curr_mode==0) || (curr_mode==4)) 
		return TRUE;
	return 
		DUALS::Set_Next_Step();
}
 
void  PHASE::Show_Run(BOOL state)
{
	DUALS::Show_Run(state);
	if ((curr_mode!=0) && (curr_mode!=4)) return;
	if (state)
	{
		ShowWindow( GetDlgItem( hDlgChild,IDD_RUN_TEXT    ), SW_HIDE);
		ShowWindow( GetDlgItem( hDlgChild,IDD_RUN_EDIT    ), SW_HIDE);
		ShowWindow( GetDlgItem( hDlgChild,IDD_INSERT      ), SW_HIDE );
		ShowWindow( GetDlgItem( hDlgChild,IDD_DELETE      ), SW_HIDE );
		ShowWindow( GetDlgItem( hDlgChild,IDD_VAL_SCROLL  ), SW_HIDE );
		ShowWindow( GetDlgItem( hDlgChild,IDD_VAL_EDIT    ), SW_HIDE );
		ShowWindow( GetDlgItem( hDlgChild,IDD_LIST  ), SW_HIDE );
	}
	else
	{
		ShowWindow(GetDlgItem(hDlgChild,IDD_STEP_BUTTON),SW_HIDE);
		ShowWindow(GetDlgItem(hDlgChild,IDD_MODE1_CB),SW_HIDE);
		ShowWindow( GetDlgItem( hDlgChild,IDD_INSERT      ), SW_HIDE );
		ShowWindow( GetDlgItem( hDlgChild,IDD_DELETE      ), SW_HIDE );
		ShowWindow( GetDlgItem( hDlgChild,IDD_VAL_SCROLL  ), SW_HIDE );
		ShowWindow( GetDlgItem( hDlgChild,IDD_VAL_EDIT    ), SW_HIDE );
		ShowWindow( GetDlgItem( hDlgChild,IDD_LIST  ), SW_HIDE );
	}
}

void  PHASE::Refresh_Values()
{
	if (!allow_phase) return;

	SendMessage(Update_Window,Update_Mode,0,curr_mode);
	if ((curr_mode==0) || (curr_mode==4)) 
		return;
	else  
		DUALS::Refresh_Values();
}

int PHASE::SaveText(HANDLE hfile)
{
	switch (curr_mode)
	{ 
	case  0:
		fhprintf(hfile,"%Fs:REF: Random \r\n",(LPSTR)Obj_Name);
		break;

	case  1:
	case  2:
	case  3:
		DUALS::SaveText(hfile);
		fhprintf(hfile,"%Fs:REF: L%d \r\n",(LPSTR)Obj_Name,(int)curr_mode);
		break;

	case  4:
		fhprintf(hfile,"%Fs:REF: DC \r\n",(LPSTR)Obj_Name);
		break;
	}
	return 1;
}

int PHASE::Process_Line(LPSTR fbuffer)
{ 
	int n;
	char buffer[100];
	strcpy(buffer,fbuffer);

	if (_fstrstr(buffer,"REF:")==buffer)
	{ 
		allow_lineref=TRUE;
		allow_phase=TRUE;
		redo_list();
		if (_fstrstr(buffer,"L1"))
			n=1;
		else if (_fstrstr(buffer,"L2") && Three_Phase)
			n=2;
		else if (_fstrstr(buffer,"L3") && Three_Phase)
			n=3;
		else if (_fstrstr(buffer,"DC"))
			n=4;
		else if (_fstrstr(buffer,"Random"))
			n=0;
		else
		{
			Message("Unknow String to Parse!","Unknown:\n%Fs",fbuffer);
			return 1;
		}

		switch (n)
		{
		case 0:
		case 1:
		case 2:
		case 3:
			SetListN(n);
			break;

		case 4:
			if (Three_Phase)
				SetListN(n);
			else 
				SetListN(2);
			break;
		}

		set_mode(n);
		return 0;
	}
	else
	{  
		if (curr_mode!=1)
		{
			Random_only(FALSE);
			SetListN(1);
			set_mode(1);
		}
	}

	return DUALS::Process_Line(fbuffer);
}

void PHASE::set_mode(int nmode)
{ 
	if (nmode==curr_mode) return;

	if ((nmode==0) || (nmode==4))
	{
		/* We are entering DC mode */
		int old_mode=mode;
		curr_mode=nmode;
		setup_mode(MODE_F8014);
		SetDlgItemText(hDlgChild,IDD_TEXT_LO,"Phase");
		ShowWindow(GetDlgItem(hDlgChild,IDD_MODE1_CB),SW_HIDE);

		if (allow_phase) 
		{
			ShowWindow(GetDlgItem(hDlgChild,IDD_EXTRA_LAB),SW_HIDE);
			ShowWindow(GetDlgItem(hDlgChild,IDD_EXTRA_CB),SW_SHOWNORMAL);
		}

		mode=old_mode;
		ichanged=TRUE;
	}
	else
	{
		/*We Are Not entering DC Mode */
		curr_mode=nmode;
		setup_mode(mode);
		SetDlgItemText(hDlgChild,IDD_TEXT_LO,"Phase");
		ShowWindow(GetDlgItem(hDlgChild,IDD_MODE1_CB),SW_SHOWNORMAL);
		ShowWindow(GetDlgItem(hDlgChild,IDD_EXTRA_LAB),SW_HIDE);
		ShowWindow(GetDlgItem(hDlgChild,IDD_EXTRA_CB),SW_SHOWNORMAL);

		ichanged=TRUE;
	}

}
 
void PHASE::Clear(void)
{
	DUALS::Clear();
	redo_list();
	curr_mode=99;
	set_mode(0);
	SetListN(0);
}

void PHASE::Random_only(BOOL ro)
{
	if (allow_phase==(!ro)) return;
	int org_mode=curr_mode;
	curr_mode=99;
	allow_phase=(!ro);
	if (ro)
	{
		set_mode(0);
		ShowWindow(GetDlgItem(hDlgChild,IDD_EXTRA_CB),SW_HIDE);
		ShowWindow(GetDlgItem(hDlgChild,IDD_EXTRA_LAB),SW_SHOWNORMAL);
		PrintfDlg(hDlgChild,IDD_EXTRA_LAB,"Rnd");
	}
	else
	{
		ShowWindow(GetDlgItem(hDlgChild,IDD_EXTRA_LAB),SW_HIDE);
		ShowWindow(GetDlgItem(hDlgChild,IDD_EXTRA_CB),SW_SHOWNORMAL);
		set_mode(org_mode);
		redo_list();

		switch(curr_mode)
		{   
		case 0:
		case 1:
		case 2:
		case 3:
			SetListN(curr_mode);
			break;

		case 4:
			if (Three_Phase)
				SetListN(curr_mode);
			else 
				SetListN(2);
			break;
		}
	}
}

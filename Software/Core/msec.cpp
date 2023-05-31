#include "stdafx.h"
#include "dlgctl.h"
#include "MSEC.h"
#include "Message.h"
#include "globals.h"

#define  MSEC_LIST_CHANGED (WM_USER+10)
#define  MSEC_FVALUE_CHANGED (WM_USER+11)
#define  MSEC_DVALUE_CHANGED (WM_USER+12)

void MSEC::Init(HANDLE hInstance,HWND hwnd, DUALS &duald,DUALS &dualf, LPSTR text)
{
	RECT rect;
	rect.top=1;
	rect.left=1;
	rect.bottom=2;
	rect.right=2;
	create(hInstance,hwnd,rect,MAKEINTRESOURCE(IDD_POLL));
	ShowWindow(hDlgChild,SW_HIDE);
	dur_ptr=&duald;
	freq_ptr=&dualf;
	freq_value=1000.0;
	dur_value=10.0;
	Update_Window=0;
	Update_fMessage=0;
	Update_dMessage=0;
	run_shown=FALSE;
	dur_ptr->ResetList("Units");
	dur_ptr->AddToList("mSec");
	dur_ptr->AddToList("Pulse");
	dur_ptr->Notify(MSEC_LIST_CHANGED,(long)hDlgChild);
	dur_ptr->Set_Message(MSEC_DVALUE_CHANGED,hDlgChild);
	dur_value=dur_ptr->Get_Current_Values();
	mSec_fixed=FALSE;
	freq_ptr->Set_Message(MSEC_FVALUE_CHANGED,hDlgChild);
	freq_value=freq_ptr->Get_Current_Values();
	strcpy(Obj_Name,text);
	created=TRUE;
}


BOOL MSEC::DlgProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	    case WM_INITDIALOG:
		    init=TRUE;
		    return TRUE;
	    case MSEC_LIST_CHANGED: 
		    if (mSec_fixed)
		    {
			    is_pulses=FALSE;
		    }
		    else
		    {
			    if (lParam==is_pulses) 
				    return TRUE;
			    is_pulses=lParam;
		    }
		    ichanged=TRUE;

		    switch (is_pulses)
		    {
		        case 0 :       //Msec
			        freq_ptr->Set_Max_Value(10000.0);
			        freq_ptr->Set_Min_Value(1000.0);
			        dur_ptr->Set_Units("mS");
			        freq_ptr->Set_Units("Hz");
			        freq_value=freq_ptr->Get_Current_Values();
			        dur_ptr->Set_Max_Value(20);
			        dur_value=dur_ptr->Get_Current_Values();
			        break;

		        case 1 :       //Pulses
			        if (EFT_2MHZ)
				        freq_ptr->Set_Max_Value(2000.0);
			        else
				        freq_ptr->Set_Max_Value(1000.0);
			        freq_ptr->Set_Min_Value(1.0);
			        freq_ptr->Set_Units("KHz");
			        freq_value=freq_ptr->Get_Current_Values();
			        freq_value*=1000.0;
			        dur_ptr->Set_Units("pls");
			        dur_ptr->Set_Max_Value(200);
			        dur_value=dur_ptr->Get_Current_Values();
			        break;
		    }
		    if (run_shown) 
			    Value_Changed();
		    return TRUE;

	    case MSEC_FVALUE_CHANGED:
	    {
		    float fv;
		    long far *lv=(long far *)&fv;
		    *lv=lParam;
		    if (is_pulses) freq_value=fv*(float)1000.0;
		    else freq_value=fv;
		    fv=freq_value;
		    SendMessage(Update_Window,Update_fMessage,0,*lv);
		    Value_Changed();
		    return TRUE;
	    }

	    case MSEC_DVALUE_CHANGED:
	    {
		    float fv;
		    long far *lv=(long far *)&fv;
		    *lv=lParam;
		    dur_value=fv;
		    Value_Changed();
		    return TRUE;
	    }

	    case WM_COMMAND:
		    return TRUE;
	}
	return FALSE;
}


void MSEC::Value_Changed()
{
	float fv;
	long far *lv=(long far *)&fv;
	if (!Update_Values) return;
	if (!is_pulses) 
	{
		fv=(dur_value*freq_value/(float)1000.0);
		SendMessage(Update_Window,Update_dMessage,(WPARAM)dur_value,*lv);
	} 
	else 
	{
		fv=dur_value;
		if (fv<1.0) fv=1.0;
		SendMessage(Update_Window,Update_dMessage,0,*lv);
	}
}

void MSEC::mSec_Only(BOOL msonly)
{
	if (mSec_fixed==msonly) return;
	mSec_fixed=msonly;
	dur_ptr->ResetList("Units");
	dur_ptr->AddToList("mSec");
	if (!msonly)
		dur_ptr->AddToList("Pulse");
	DlgProc(hDlgChild,MSEC_LIST_CHANGED,0,0L);
} 

void MSEC::Clear(void)
{
	mSec_fixed=FALSE;
	dur_ptr->ResetList("Units");
	dur_ptr->AddToList("mSec");
	dur_ptr->AddToList("Pulse");
	DlgProc(hDlgChild,MSEC_LIST_CHANGED,0,0L);
}


int MSEC::SaveText(HANDLE hfile)
{
	if (is_pulses) 
		fhprintf(hfile,"%Fs:Pulses\r\n",(LPSTR)Obj_Name);
	else 
		fhprintf(hfile,"%Fs:mSec\r\n",(LPSTR)Obj_Name);

	return 1;
}

int MSEC::Process_Line(LPSTR buffer)
{
	BOOL sv;
	if (_fstrstr(buffer,"Pulses")==buffer) 
		sv=TRUE;
	else if (_fstrstr(buffer,"mSec")==buffer) 
		sv=FALSE;
	else
	{
		Message("Unknow String to Parse!","Unknown:\n%Fs",buffer);
		return 1;
	}

	dur_ptr->SetListN(sv);
	SendMessage(hDlgChild,MSEC_LIST_CHANGED,0,(long)sv);
	return 0;
}

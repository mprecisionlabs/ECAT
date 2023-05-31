#include "stdafx.h"
#include "dlgctl.h"
#include "polarity.h"
#include "message.h"

#define  POLARITY_LIST_CHANGED (WM_USER+10)
#define  POLARITY_VALUE_CHANGED (WM_USER+11)

void POLARITY::Init(HANDLE hInstance,HWND hwnd, DUALS &dual,HICON picon,HICON micon, HICON vicon, LPSTR text)
{
	RECT rect;
	rect.top=1;
	rect.left=1;
	rect.bottom=2;
	rect.right=2;
	create(hInstance,hwnd,rect,MAKEINTRESOURCE(IDD_POLL));
	ShowWindow(hDlgChild,SW_HIDE);
	dual_ptr=&dual;
	pi=picon;
	mi=micon;
	vi=vicon;
	Update_Window=0;
	Update_Message=0;
	run_shown=FALSE;
	dual_ptr->ResetList("Polarity:");
	dual_ptr->AddToList("Pos");
	dual_ptr->AddToList("Neg");
	dual_ptr->AddToList("Alt 1 ea");
	dual_ptr->AddToList("Alt 5 ea");
	dual_ptr->Notify(POLARITY_LIST_CHANGED,(long)hDlgChild);
	dual_ptr->Set_Message(POLARITY_VALUE_CHANGED,hDlgChild);
	voltage_value=dual_ptr->Get_Current_Values();
	poltype=0;
	steps=1;
	strcpy(Obj_Name,text);
	created=TRUE;
}

BOOL POLARITY::DlgProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_INITDIALOG:
		init=TRUE;
		return TRUE;

	case POLARITY_LIST_CHANGED: 
		if (lParam==poltype) return TRUE;
		poltype=lParam;
		ichanged=TRUE;
		switch (poltype)
		{
		case 0 :       //Positive polarity
			dual_ptr->Change_Icon(pi);
			current_pol=0;
			steps=1;
			break;

		case 1 :       //Negative Polarity
			dual_ptr->Change_Icon(mi);
			current_pol=1;
			steps=1;
			break;

		case 2 :       //Alternating Polarity
			dual_ptr->Change_Icon(vi);
			current_pol=0;
			active_count=0;
			steps=2;
			break;

		case 3 :       //Alternating Polarity 5 Ea
			dual_ptr->Change_Icon(vi);
			current_pol=0;
			active_count=0;
			steps=10;
			break;
		}

		if (run_shown) 
			Value_Changed();
		return TRUE;

	case POLARITY_VALUE_CHANGED:
	{
		float fv;
		long far *lv=(long far *)&fv;
		*lv=lParam;
		voltage_value=fv;
		if (run_shown) 
			Value_Changed();
		return TRUE;
	}

	case WM_COMMAND:
		return TRUE;
	}

	return FALSE;
}


void POLARITY::Show_Run(BOOL state)
{
	run_shown=state;
	if (state)
	{
		switch (poltype)
		{
		case 0: //Positive
		case 1: //Negative
			break;
	
		case 2: //alt 1
		case 3: //alt 5
			dual_ptr->Change_Icon(pi);
			break;
		}
		Value_Changed();
	}//Run
	else
	{
		switch (poltype)
		{
		case 0: //Positive
		case 1: //Negative
			break;
	
		case 2: //alt 1
		case 3: //alt 5
			dual_ptr->Change_Icon(vi);
			break;
		}
	}//Not Run
}



void POLARITY::Set_Start_Values()
{
	switch (poltype)
	{
	case 0: //Positive
		dual_ptr->Change_Icon(pi);
		current_pol=0;
		break;

	case 1: //Negative
		dual_ptr->Change_Icon(mi);
		current_pol=1;
		break;

	case 2: //alt 1
	case 3: //alt 5
		dual_ptr->Change_Icon(pi);
		current_pol=0;
		active_count=0;
		break;
	}
	Value_Changed();
}

BOOL POLARITY::Set_Next_Step()
{
	switch (poltype)
	{ 
	case 0: //Positive
	case 1: //Negative
		return TRUE;
	
	case 2: //alt 1
		if (current_pol) 
			return TRUE;
		else 
			current_pol=1; 
		break; 

	case 3: //alt 5
		if (current_pol)
		{
			active_count++;
			if (active_count==5) 
				return TRUE;
			current_pol=0;
		} 
		else 
			current_pol=1;
		break;
	}

	if (current_pol) 
		dual_ptr->Change_Icon(mi);
	else 
		dual_ptr->Change_Icon(pi);
	Value_Changed();
	return FALSE;
}


void POLARITY::Value_Changed()
{ 
	if (run_shown) 
		Refresh_Values();
}

void POLARITY::Refresh_Values()
{
	float fv;
	long far *lv=(long far *)&fv;
	fv=voltage_value;
	if (current_pol) 
		fv=-fv;
	SendMessage(Update_Window,Update_Message,0,*lv);
}

void POLARITY::Hide(BOOL b)
{
}

void POLARITY::Clear()
{
	voltage_value=dual_ptr->Get_Current_Values();
	poltype=0;
	steps=1;
	ichanged=TRUE;
}

int POLARITY::SaveText(HANDLE hfile)
{
	switch (poltype)
	{
	case 0 :       //Positive
		fhprintf(hfile,"%Fs:Positive\r\n",(LPSTR)Obj_Name);
		break;

	case 1 :       //Negative Polarity
		fhprintf(hfile,"%Fs:Negative\r\n",(LPSTR)Obj_Name);
		break;
	
	case 2 :       //Alternating Polarity
		fhprintf(hfile,"%Fs:Alternate 1 each\r\n",(LPSTR)Obj_Name);
		break;

	case 3 :       //Alternating Polarity 5 Ea
		fhprintf(hfile,"%Fs:Alternate 5 each\r\n",(LPSTR)Obj_Name);
		break;
	}
	return 1;
}

int POLARITY::Process_Line(LPSTR buffer)
{
	int pt;
	if (_fstrstr(buffer,"Positive")==buffer) 
		pt=0;
	else if (_fstrstr(buffer,"Negative")==buffer) 
		pt=1;
	else if (_fstrstr(buffer,"Alternate 1")==buffer) 
		pt=2;
	else if (_fstrstr(buffer,"Alternate 5")==buffer) 
		pt=3;
	else
	{
		Message("Unknow String to Parse!","Unknown:\n%Fs",buffer);
		return 1;
	}

	dual_ptr->SetListN(pt);
	poltype=pt;
	switch (poltype)
	{
	case 0 :       //Positive polarity
	case 1 :       //Negative Polarity
		steps=1;
		break;

	case 2 :       //Alternating Polarity
		steps=2;
		break;

	case 3 :       //Alternating Polarity 5 Ea
		steps=10;
		break;
	}
	ichanged=TRUE;
	return 0;
}



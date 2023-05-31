/*****************************************************************************

DUALS.CPP -- Defines class underlying several of the screen groups in
			BurstWare and SurgeWare.  See APPE400.H where many of the derived
			classes are declared.

HISTORY:
	8/9/95	JFL	Add >0 test for N_Steps, IDD_STEPS_EDIT (eliminate crashes).
   2/26/96	JFL	"Special_801-4" now displays 1000-4 in listbox.
*****************************************************************************/

#include "stdafx.h"
#pragma hdrstop
#include "DUALS.h"
#include <math.h>
#include "message.h"

DUALS::DUALS()
{
	Notify_Message=0;
	Notify_Window=0;
	Update_Window=0;
	Update_Message=0;
	hidden=FALSE;
}

void DUALS::Init(HANDLE hInstance,HWND hParent, RECT& rect, HICON hicon,UINT start,UINT end,UINT init,int exp,int step,LPSTR text,char * unit_txt)
{
	running=0;
	icon=hicon;
	Special_F8014=FALSE;
	Step_Delta=step;
	N_steps=10;
	tmH=0;
	Percent_Change=10.0;
	Update_Window=0;
	Update_Values=FALSE;
	_fstrncpy(units,unit_txt,4);
	if (unit_txt[1]==0)
	{ 
		units[1]=' '; 
		units[2]=0;
	}

	if (step>10) 
		step=step/10; 
	else 
		step=1;

	create(hInstance,hParent,rect,MAKEINTRESOURCE(IDD_DUAL));
	startset=start;
	endset=end;
	initset=init;
	expset=exp;
	stepset=step;
	linear_step_type=step_type=0;

	startv.init(hDlgChild,IDD_START_SB,IDD_START_EDIT,start,end,init,exp,step,RGB(0,255,0),units);
	stopv.init(hDlgChild,IDD_STOP_SB,IDD_STOP_EDIT,start,end,init,exp,step,RGB(255,0,0),units);
	enterv.init(hDlgChild,IDD_VAL_SCROLL,IDD_VAL_EDIT,start,end,init,exp,step,RGB(0,255,0),units);
	singlev.init(hDlgChild,IDD_SINGLE_SB,IDD_SINGLE_EDIT,start,end,init,exp,step,RGB(0,255,0),units);
	enterv.Hide(TRUE);
	startv.Hide(TRUE);
	stopv.Hide(TRUE);
	singlev.Hide(FALSE);
	Change_Icon(hicon);
	ShowWindow(hDlgChild,SW_SHOWNORMAL);
	SetDlgItemText(hDlgChild,IDD_TEXT_HI,text);
	SetDlgItemText(hDlgChild,IDD_TEXT_LO,text);
	strcpy(Obj_Name,text);
	created=TRUE;
}

void  DUALS::Change_State(UINT start,UINT end,UINT init,int exp,int step,LPSTR text,char * unit_txt)
{
	Step_Delta=step;
	_fstrncpy(units,unit_txt,4);
	if (unit_txt[1]==0){ units[1]=' '; units[2]=0;}
	startv.init(hDlgChild,IDD_START_SB,IDD_START_EDIT,start,end,init,exp,step,RGB(0,255,0),units);
	stopv.init(hDlgChild,IDD_STOP_SB,IDD_STOP_EDIT,start,end,init,exp,step,RGB(255,0,0),units);
	singlev.init(hDlgChild,IDD_SINGLE_SB,IDD_SINGLE_EDIT,start,end,init,exp,step,RGB(0,255,0),units);
	enterv.init(hDlgChild,IDD_VAL_SCROLL,IDD_VAL_EDIT,start,end,init,exp,step,RGB(0,255,0),units);
	startv.Hide(TRUE);
	stopv.Hide(TRUE);
	enterv.Hide(TRUE);
	singlev.Hide(FALSE);
	SetDlgItemText(hDlgChild,IDD_TEXT_HI,text);
	SetDlgItemText(hDlgChild,IDD_TEXT_LO,text);
}


void DUALS::Change_Icon(HICON hIcon)
{ 
	icon=hIcon;
	SendMessage(GetDlgItem(hDlgChild,IDD_ICON), STM_SETICON, (WPARAM)hIcon, 0);
}

void DUALS::AddToList(LPSTR text)
{
	SendDlgItemMessage(hDlgChild,IDD_EXTRA_CB,CB_ADDSTRING,0,(LONG)text);
	ShowWindow( GetDlgItem( hDlgChild,IDD_EXTRA_CB   ), SW_SHOWNORMAL);
	ShowWindow( GetDlgItem( hDlgChild,IDD_EXTRA_LAB  ), SW_HIDE);
	ShowWindow( GetDlgItem( hDlgChild,IDD_EXTRA_TEXT ), SW_SHOWNORMAL);
	if (!List_Shown) 
	{
		SendDlgItemMessage(hDlgChild,IDD_EXTRA_CB,CB_SETCURSEL,0,0L); 
		Old_sel=0; 
	}
	List_Shown=TRUE;
}


void DUALS::ResetList(LPSTR text)
{
	List_Shown=FALSE;
	SetDlgItemText(hDlgChild,IDD_EXTRA_TEXT,text);
	SendDlgItemMessage(hDlgChild,IDD_EXTRA_CB,CB_RESETCONTENT,0,0L);
	ShowWindow( GetDlgItem( hDlgChild,IDD_EXTRA_CB    ), SW_HIDE);
	ShowWindow( GetDlgItem( hDlgChild,IDD_EXTRA_TEXT    ), SW_HIDE);
	Old_sel=-1;
}

LONG DUALS::ReportSteps(void)
{
	LONG n;
	LONG diff;
	if (hidden) return 1;
	switch (mode)
	{
		case MODE_FIXED: /*Fixed */ return 1;
	
		case MODE_LINEAR: /*Linear */
			if(step_type) return N_steps+1;
			{
				diff=stopv.Value()-startv.Value();
				if (Step_Delta >0) 
				{
					n=(diff/Step_Delta);
					if ((diff % Step_Delta)!=0) n++;
				}
				if (n<0) return -n;
				if (n==0) n++;
				return n+1;
			}

		case MODE_LOG:/*Log*/ return N_steps+1;
		
		case MODE_LIST:/*List */
			n=( SendDlgItemMessage(hDlgChild,IDD_LIST,LB_GETCOUNT,0,0L)-2);
			if (n<=0) n=0;
		return n;
		
		case MODE_F8014:/*F8014 */return 1;

		case MODE_CHIRP:/*F8014 */return 1;
	}
	
	return(1);
}

#define DIS  (*((LPDRAWITEMSTRUCT)(lParam)))
#define MIS  (*((LPMEASUREITEMSTRUCT)(lParam)))

BOOL DUALS::DlgProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	POINT ptOld;
	LONG sel;
	LONG rv;
	LONG cp;
	float f;
	HBRUSH lhBrush;
	char gStr0[100];
	POINT point;
	HDC hDC;
	static BOOL Suspend_Steps;

	switch (message)
	{
	case WM_INITDIALOG:
		List_Shown=FALSE;
		VList_Shown=FALSE;

		rBrush=CreateSolidBrush(RGB(255,0,0));
		gBrush=CreateSolidBrush(RGB(0,255,0));
		yBrush=CreateSolidBrush(RGB(255,255,0));

		ShowWindow( GetDlgItem( hDlg,IDD_INSERT      ), SW_HIDE );
		ShowWindow( GetDlgItem( hDlg,IDD_DELETE      ), SW_HIDE );
		ShowWindow( GetDlgItem( hDlg,IDD_VAL_SCROLL  ), SW_HIDE );
		ShowWindow( GetDlgItem( hDlg,IDD_VAL_EDIT    ), SW_HIDE );
		ShowWindow( GetDlgItem( hDlg,IDD_LIST  ), SW_HIDE );

		ShowWindow( GetDlgItem( hDlg,IDD_START_TXT   ), SW_HIDE );
		ShowWindow( GetDlgItem( hDlg,IDD_STOP_TXT    ), SW_HIDE );
		ShowWindow( GetDlgItem( hDlg,IDD_STEPS_EDIT  ), SW_HIDE );
		ShowWindow( GetDlgItem( hDlg,IDD_STEPS_TXT   ), SW_HIDE );
		ShowWindow( GetDlgItem( hDlg,IDD_SINGLE_LB   ), SW_HIDE );
		ShowWindow( GetDlgItem( hDlg,IDD_EXTRA_CB    ), SW_HIDE);
		ShowWindow( GetDlgItem( hDlg,IDD_EXTRA_LAB   ), SW_HIDE);
		ShowWindow( GetDlgItem( hDlg,IDD_STEP_TXT2   ), SW_HIDE );
		ShowWindow( GetDlgItem( hDlg,IDD_STEP_BUTTON ), SW_HIDE);
		ShowWindow( GetDlgItem( hDlg,IDD_EXTRA_TEXT  ), SW_HIDE);
		ShowWindow( GetDlgItem( hDlg,IDD_TEXT_HI     ), SW_HIDE);
		ShowWindow( GetDlgItem( hDlg,IDD_RUN_TEXT    ), SW_HIDE);
		ShowWindow( GetDlgItem( hDlg,IDD_RUN_EDIT    ), SW_HIDE);
		ShowWindow( GetDlgItem( hDlg,IDD_MODE_LAB    ), SW_HIDE);


		SetDlgItemText(hDlg,IDD_STEPS_TXT,"Change");
		SetDlgItemText(hDlg,IDD_STEP_TXT2,"Step");
		N_steps=1;
		SendDlgItemMessage(hDlg,IDD_MODE1_CB,CB_ADDSTRING,0,(LONG)((LPSTR)"Fixed"));
		SendDlgItemMessage(hDlg,IDD_MODE1_CB,CB_ADDSTRING,0,(LONG)((LPSTR)"Linear"));

		SendDlgItemMessage(hDlg,IDD_MODE1_CB,CB_ADDSTRING,0,(LONG)((LPSTR)"Log"));
		SendDlgItemMessage(hDlg,IDD_MODE1_CB,CB_ADDSTRING,0,(LONG)((LPSTR)"List"));
		SendDlgItemMessage(hDlg,IDD_MODE1_CB,CB_ADDSTRING,0,(LONG)((LPSTR)"New List"));
		SendDlgItemMessage(hDlg,IDD_MODE1_CB,CB_SETCURSEL,0,(LONG)((LPSTR)"Fixed"));

		SendDlgItemMessage(hDlg,IDD_LIST,LB_ADDSTRING,0,0x40000000L);
		SendDlgItemMessage(hDlg,IDD_LIST,LB_ADDSTRING,0,0x40000001L);
		SendDlgItemMessage(hDlg,IDD_LIST,LB_SETCURSEL,0,0L);
		created = TRUE;
		return FALSE;

	case WM_CTLCOLORSTATIC:
	case WM_CTLCOLOREDIT:
		lhBrush = NULL;
		hDC=(HDC)wParam;
		switch(GetWindowLong((HWND)lParam,GWL_ID))
		{
		case IDD_SINGLE_EDIT:
		case IDD_START_EDIT:
			lhBrush=gBrush;
			SetBkColor(hDC,RGB(0,255,0));
			break;
		
		case IDD_STOP_EDIT:
			lhBrush=rBrush;
			SetBkColor(hDC,RGB(255,0,0));
			break;

		case IDD_RUN_EDIT:
		case IDD_VAL_EDIT:
			lhBrush=yBrush;
			SetBkColor(hDC,RGB(255,255,0));
			break;
		}
		
		if (lhBrush != NULL)
		{
			UnrealizeObject(lhBrush);
			point.x=point.y=0;
			ClientToScreen(hDlg,&point);
			SetBrushOrgEx(hDC,point.x,point.y,&ptOld);
			SelectObject(hDC,lhBrush);
			return ((DWORD)lhBrush);
		}
		break;
		
	case WM_DESTROY:
		DeleteObject(gBrush); 
		DeleteObject(rBrush); 
		DeleteObject(yBrush); 
		return FALSE;

	case WM_HSCROLL:
		if (created)
		{
			switch (GetWindowLong (HWND (lParam), GWL_ID))
			{
				case IDD_SINGLE_SB:
				{
					rv=(singlev.Scroll_Proc(message,wParam,lParam));
					if (singlev.changed())
					{
						ichanged=TRUE;
						if (running)  
						{
							Present_Value=(float)singlev.Value();
							Value_Changed(); //XXX
						}
					}
					return rv;
				}

				case IDD_START_SB:
					ichanged=TRUE;
					return (startv.Scroll_Proc(message,wParam,lParam));

				case IDD_STOP_SB:
					ichanged=TRUE;
					return(stopv.Scroll_Proc(message,wParam,lParam));

				case IDD_VAL_SCROLL:
					ichanged=TRUE;
					return(enterv.Scroll_Proc(message,wParam,lParam));

			} // switch WindowLong
		}
		else
			return TRUE;
		break;

	case WM_DRAWITEM:
		cp=DIS.itemData;
		//DBGOUT("WM_DRAWITEM - cp: 0x%08X.\n", cp);

		MoveToEx(DIS.hDC,DIS.rcItem.left,DIS.rcItem.top+1,&ptOld);
		SetTextAlign(DIS.hDC,TA_UPDATECP);

		if (cp == 0x40000000) 
			TextOut(DIS.hDC,0,0,"Start of List",13);
		else if (cp==0x40000001)
			TextOut(DIS.hDC,0,0,"End of List",11);
		else
		{
            int i;

			for (i=0; i<20; i++) 
				gStr0[i]=' ';

			_itoa(DIS.itemID,gStr0,10);
			i=strlen(gStr0);
			gStr0[i]=' ';
			TextOut(DIS.hDC,0,0,(LPSTR)gStr0,5);
			sprintf(gStr0,"%ld %Fs",DIS.itemData,(LPSTR)units);
			i=strlen(gStr0);
			TextOut(DIS.hDC,0,0,(LPSTR)gStr0,i);
		}

		if ((DIS.itemState &ODS_SELECTED) && running)
		{
			SetBkColor(DIS.hDC,RGB(255,255,0));
			TextOut(DIS.hDC,0,0,"<---",4);
			SetBkColor(DIS.hDC,GetSysColor(COLOR_WINDOW));
		}
		else 
			TextOut(DIS.hDC,0,0,"       ",7);
		return TRUE;

	case WM_MEASUREITEM: 
		if (!tmH)
		{ 
			TEXTMETRIC tm;
			HDC hdc=GetDC(hDlg);
			GetTextMetrics(hdc,&tm);
			tmH=tm.tmHeight;
			ReleaseDC(hDlg,hdc);
		}
		MIS.itemHeight=tmH+2; 
		return TRUE;

	case WM_COMMAND:
		// Look who's talking
		switch (LOWORD(wParam))
		{
		case IDD_EXTRA_CB:
			// Check notification messages sent in HIWORD(wParam)
			switch (HIWORD(wParam))
			{
			case CBN_KILLFOCUS:
			case CBN_SELCHANGE:
				sel=SendDlgItemMessage(hDlg,IDD_EXTRA_CB,CB_GETCURSEL,0,0L);
				if (sel!=Old_sel) 
				{
					Old_sel=sel;
					if (Notify_Message) 
					{
						if (Notify_Window==0) 
							SendMessage((HWND)parent,Notify_Message,0,sel);
						else 
							SendMessage((HWND)Notify_Window,Notify_Message,0,sel);
					}
					ichanged=TRUE;
				}
				break;
			}
			break;

		case IDD_MODE1_CB:
			// Check notification messages sent in HIWORD(wParam)
			switch (HIWORD(wParam))
			{
			case CBN_KILLFOCUS:
			case CBN_SELCHANGE:
				if (mode!=SendDlgItemMessage(hDlg,IDD_MODE1_CB,CB_GETCURSEL,0,0L))
				{
					ichanged=TRUE;
					mode=SendDlgItemMessage(hDlg,IDD_MODE1_CB,CB_GETCURSEL,0,0L);
					setup_mode(mode);
				}
				return TRUE;
				break;
			}
			break;

		case IDD_INSERT:
			cp=SendDlgItemMessage(hDlg,IDD_LIST,LB_GETTOPINDEX,0,0L);
			if (SendDlgItemMessage(hDlg,IDD_LIST,LB_GETITEMDATA,cp+1,0L)==0x40000001)
				SendDlgItemMessage(hDlg,IDD_LIST,LB_INSERTSTRING,cp+1,(LONG)enterv.Value());
			else if (SendDlgItemMessage(hDlg,IDD_LIST,LB_GETITEMDATA,cp+2,0L)==0x40000001)
				SendDlgItemMessage(hDlg,IDD_LIST,LB_INSERTSTRING,cp+2,(LONG)enterv.Value());
			else
			{ 
				SendDlgItemMessage(hDlg,IDD_LIST,LB_INSERTSTRING,cp+3,(LONG)enterv.Value());
				SendDlgItemMessage(hDlg,IDD_LIST,LB_SETTOPINDEX,cp+2,0L);
			}

			ichanged=TRUE;
			return TRUE;

		case IDD_DELETE:
			cp=SendDlgItemMessage(hDlg,IDD_LIST,LB_GETTOPINDEX,0,0L);
			if (0x40000001!=SendDlgItemMessage(hDlg,IDD_LIST,LB_GETITEMDATA,cp+1,0L))
				SendDlgItemMessage(hDlg,IDD_LIST,LB_DELETESTRING,cp+1,0L);
			SendDlgItemMessage(hDlg,IDD_LIST,LB_SETTOPINDEX,cp,0L);
			ichanged=TRUE;
			return TRUE;

		case IDD_VAL_SCROLL:
		case IDD_VAL_EDIT:
			if (created)
				return(enterv.Scroll_Proc(message,wParam,lParam));
			else 
				break;

		case IDD_START_SB:
		case IDD_START_EDIT:
			ichanged=TRUE;
			if (created)
				return(startv.Scroll_Proc(message,wParam,lParam));
			else 
				break;

		case IDD_STOP_SB:
		case IDD_STOP_EDIT:
			ichanged=TRUE;
			if (created)
				return(stopv.Scroll_Proc(message,wParam,lParam));
			else 
				break;

		case IDD_SINGLE_SB:
		case IDD_SINGLE_EDIT:
			ichanged=TRUE;
			if (created)
			{
				rv=(singlev.Scroll_Proc(message,wParam,lParam));
				if ((running) && (singlev.changed()))
				{
					Present_Value=(float)singlev.Value();
					Value_Changed(); //XXX
				}
				return rv;
			}
			else 
				break;

		case IDD_STEP_BUTTON:
			step_type ^= 1;
			if (mode == MODE_LINEAR)
				linear_step_type = step_type;
			ichanged = TRUE;
			PostMessage(hDlg,WM_COMMAND,UPDATE_STEP,0L);
			return TRUE;

		case IDD_STEPS_EDIT:
			// Check notification messages sent in HIWORD(wParam)
			switch (HIWORD(wParam))
			{
			case EN_CHANGE:
				if (!Suspend_Steps)
				{   
					ichanged = TRUE;
					GetDlgItemText(hDlg, IDD_STEPS_EDIT, (LPSTR)gStr0, 100);
					if (sscanf(gStr0, "%f", &f) && f > 0)
					{
						if (step_type) 
							N_steps = (long)f;
						else if (mode == MODE_LINEAR) 
							Step_Delta = (int)f;
						else 
							Percent_Change = f;
					}
				}
				break;

			case EN_KILLFOCUS:
				if (!Suspend_Steps)
				{
					ichanged = TRUE;
					GetDlgItemText(hDlg, IDD_STEPS_EDIT, (LPSTR)gStr0, 100);
					if(sscanf(gStr0,"%f",&f) && f > 0)
					{
						if (step_type) 
							N_steps = (long)f;
						else if (mode == MODE_LINEAR) 
							Step_Delta= (int)f;
						else 
							Percent_Change = f;
					}

					PostMessage(hDlg, WM_COMMAND, UPDATE_STEP, 0L);
				}
				break;
			}
			break;

		case UPDATE_STEP:
			Suspend_Steps=TRUE;
			if (!step_type)
			{
				SetDlgItemText(hDlg,IDD_STEPS_TXT,"Size");
				if (mode==MODE_LOG) 
				{ 
					PrintfDlg(hDlg,IDD_STEPS_EDIT,"%2.1f%%",Percent_Change);
					SetDlgItemText(hDlg,IDD_STEP_TXT2,"Percent");
				}
				else 
				{
					PrintfDlg(hDlg,IDD_STEPS_EDIT,"%d",Step_Delta);
					PrintfDlg(hDlg,IDD_STEP_TXT2,"Step (%s)",units);
				}
			}
			else
			{
				SetDlgItemText(hDlg,IDD_STEPS_TXT,"N Steps");
				SetDlgItemText(hDlg,IDD_STEP_TXT2,"");
				PrintfDlg(hDlg,IDD_STEPS_EDIT,"%d",N_steps);
			}
			Suspend_Steps=FALSE;
			return TRUE;

		}//Switch wParam
	}//Switch message

	return FALSE;
}


void DUALS::Set_Start_Values()
{
	switch (mode)
	{
	case MODE_FIXED: //Fixed
		VList_Shown=FALSE;
		Present_Value=(float)singlev.Value();
		Value_Changed();
		Steps_Done=0;
		break;

	case MODE_LOG: //LOG
		LogInc=(float)(log((float)stopv.Value())-log((float)startv.Value()))/(N_steps);

	case MODE_LINEAR: //Linear
		VList_Shown=FALSE;
		Present_Value=(float)startv.Value();
		Value_Changed();
		if (!step_type)
		{
			if ((startv.Value()>stopv.Value()) && (Step_Delta>0))
			{
				Step_Delta=-Step_Delta;
				PrintfDlg(hDlgChild,IDD_STEPS_EDIT,"%d",Step_Delta);
			}
		}
		break;

	case MODE_LIST: //LIST
		VList_Shown=TRUE;
		Current_List_Pos=1;
		SendDlgItemMessage(hDlgChild,IDD_LIST,LB_SETCURSEL,Current_List_Pos,0L);
		SendDlgItemMessage(hDlgChild,IDD_LIST,LB_SETTOPINDEX,Current_List_Pos-1,0L);
		if (SendDlgItemMessage(hDlgChild,IDD_LIST,LB_GETCOUNT,0,0L)-2)
			Present_Value=(float)SendDlgItemMessage(hDlgChild,IDD_LIST,LB_GETITEMDATA,Current_List_Pos,0L);
		else 
			Present_Value=(float)enterv.Value();
		Value_Changed();
		break;
	}

	PrintfDlg(hDlgChild,IDD_RUN_EDIT,"%2.0f %Fs",Present_Value,(LPSTR)units);
}

void DUALS::Show_Run(BOOL state)
{
	running=state;
	LONG csel;
	char buff[30];
	InvalidateRect(GetDlgItem(hDlgChild,IDD_LIST),NULL,0);

	if (state)
	{ 
		ShowWindow( GetDlgItem( hDlgChild,IDD_RUN_TEXT    ), SW_SHOWNORMAL);
		ShowWindow( GetDlgItem( hDlgChild,IDD_RUN_EDIT    ), SW_SHOWNORMAL);
		ShowWindow(GetDlgItem(hDlgChild,IDD_EXTRA_CB),SW_HIDE);
		ShowWindow(GetDlgItem(hDlgChild,IDD_MODE1_CB),SW_HIDE);
		ShowWindow(GetDlgItem(hDlgChild,IDD_MODE_LAB),SW_SHOWNORMAL);
		csel=SendDlgItemMessage(hDlgChild,IDD_MODE1_CB,CB_GETCURSEL,0,0L);
		SendDlgItemMessage(hDlgChild,IDD_MODE1_CB,CB_GETLBTEXT,(UINT)csel,(LONG)((LPSTR)buff));
		PrintfDlg(hDlgChild,IDD_MODE_LAB,"%s",buff);
		if (List_Shown)
		{
			ShowWindow(GetDlgItem(hDlgChild,IDD_EXTRA_CB),SW_HIDE);
			ShowWindow(GetDlgItem(hDlgChild,IDD_MODE_LAB),SW_SHOWNORMAL);
			csel=SendDlgItemMessage(hDlgChild,IDD_EXTRA_CB,CB_GETCURSEL,0,0L);
			SendDlgItemMessage(hDlgChild,IDD_EXTRA_CB,CB_GETLBTEXT,(UINT)csel,(LONG)((LPSTR)buff));
			PrintfDlg(hDlgChild,IDD_EXTRA_LAB,"%s",buff);
			ShowWindow(GetDlgItem(hDlgChild,IDD_EXTRA_LAB),SW_SHOWNORMAL);
		}

		if (VList_Shown)
		{                                          
			ShowWindow(GetDlgItem(hDlgChild,IDD_INSERT),SW_HIDE);
			ShowWindow(GetDlgItem(hDlgChild,IDD_DELETE),SW_HIDE);
			enterv.Hide(TRUE);
		}
		if (mode==MODE_LINEAR) 
			ShowWindow(GetDlgItem(hDlgChild,IDD_STEP_BUTTON),SW_HIDE);
	}
	else
	{
		ShowWindow( GetDlgItem( hDlgChild,IDD_RUN_TEXT    ), SW_HIDE);
		ShowWindow( GetDlgItem( hDlgChild,IDD_RUN_EDIT    ), SW_HIDE);
		ShowWindow(GetDlgItem(  hDlgChild,IDD_MODE1_CB),SW_SHOWNORMAL);
		ShowWindow(GetDlgItem(  hDlgChild,IDD_MODE_LAB),SW_HIDE);
		if (List_Shown)
		{ 
			ShowWindow(GetDlgItem(hDlgChild,IDD_EXTRA_LAB),SW_HIDE);
			ShowWindow(GetDlgItem(hDlgChild,IDD_EXTRA_CB),SW_SHOWNORMAL);
		}

		if (VList_Shown)
		{    
			ShowWindow(GetDlgItem(hDlgChild,IDD_INSERT),SW_SHOWNORMAL);
			ShowWindow(GetDlgItem(hDlgChild,IDD_DELETE),SW_SHOWNORMAL);
			enterv.Hide(FALSE);
		}

		if (mode==MODE_LINEAR) 
			ShowWindow(GetDlgItem(hDlgChild,IDD_STEP_BUTTON),SW_SHOWNORMAL);
	}
}



BOOL DUALS::Set_Next_Step()
{
	if (hidden) return TRUE;
	switch (mode)
	{
	case MODE_FIXED: //Fixed
		Present_Value=(float)singlev.Value();
		Value_Changed();
		return TRUE;

	case MODE_LINEAR: //Linear
		if (step_type)
		{ 
			//N Steps
			if (Present_Value==stopv.Value()) 
				return TRUE;
			Present_Value+=((float)(stopv.Value()-startv.Value()))/((float)(N_steps));
		}
		else
		{ 
			//Step Size
			if (Present_Value==stopv.Value()) 
				return TRUE;
			Present_Value+=Step_Delta;
		}
		break;

	case MODE_LOG: //LOG
		if (Present_Value==stopv.Value()) 
			return TRUE;
		Present_Value=(float)exp(log(Present_Value)+LogInc);
		break;

	case MODE_LIST: //LIST
		long next=SendDlgItemMessage(hDlgChild,IDD_LIST,LB_GETITEMDATA,Current_List_Pos+1,0L);
		if (next==0x40000001) return TRUE;
		Current_List_Pos++;
		SendDlgItemMessage(hDlgChild,IDD_LIST,LB_SETCURSEL,Current_List_Pos,0L);
		SendDlgItemMessage(hDlgChild,IDD_LIST,LB_SETTOPINDEX,Current_List_Pos-1,0L);

		Present_Value=(float)next;
		PrintfDlg(hDlgChild,IDD_RUN_EDIT,"%2.0f %Fs",Present_Value,(LPSTR)units);
		Value_Changed();
		return FALSE;
	}

	if (startv.Value()<stopv.Value())
	{
		if (Present_Value >= stopv.Value())
			Present_Value=(float)stopv.Value();
	}
	else 
	{
		if (Present_Value<=stopv.Value()) 
		{
			Present_Value=(float)stopv.Value();
		}
	}

	PrintfDlg(hDlgChild,IDD_RUN_EDIT,"%2.0f %Fs",Present_Value,(LPSTR)units);
	Value_Changed();
	return FALSE;

}

void DUALS::Set_Special_F8014(BOOL set, BOOL chirp)
{
	if (set==Special_F8014) return;
	Special_F8014=set;
	if (set)
	{
		SendDlgItemMessage(hDlgChild,IDD_MODE1_CB,CB_ADDSTRING,0,(LONG)((LPSTR)"1000-4"));
		if (chirp) 
			SendDlgItemMessage(hDlgChild,IDD_MODE1_CB,CB_ADDSTRING,0,(LONG)((LPSTR)"CHIRP"));
	}
	else
	{
		SendDlgItemMessage(hDlgChild,IDD_MODE1_CB,CB_DELETESTRING,4,0L);
		SendDlgItemMessage(hDlgChild,IDD_MODE1_CB,CB_SETCURSEL,0,(LONG)((LPSTR)"Fixed"));
	}
}

void  DUALS::Set_Value(float value)
{
	singlev.Set_Value((LONG) value);
	Present_Value=(float)singlev.Value();
	Value_Changed();
}

void DUALS::Set_Max_Value(float value)
{
	if (Present_Value>value) 
		Present_Value=value;
	singlev.Set_Max_Value((LONG) value);
	startv.Set_Max_Value((LONG) value);
	stopv.Set_Max_Value((LONG) value);
	enterv.Set_Max_Value((LONG) value);
	endset=(long)value;
	Value_Changed();
}

void DUALS::Set_Min_Value(float value)
{
	if (Present_Value<value) 
		Present_Value=value;
	singlev.Set_Min_Value((LONG) value);
	startv.Set_Min_Value((LONG) value);
	stopv.Set_Min_Value((LONG) value);
	enterv.Set_Min_Value((LONG) value);
	startset=(long)value;
	Value_Changed();
}

void DUALS::Set_Units(LPSTR new_units)
{
	_fstrncpy(units,new_units,4);
	if (new_units[1]==0)
	{ 
		units[1]=' '; 
		units[2]=0;
	}

	PrintfDlg(hDlgChild,IDD_RUN_EDIT,"%2.0f %Fs",Present_Value,(LPSTR)units);
	switch (mode)
	{ 
	case MODE_LOG:
		PrintfDlg(hDlgChild,IDD_STEPS_EDIT,"%2.1f%%",Percent_Change);
		break;

	case MODE_LINEAR:
		PrintfDlg(hDlgChild,IDD_STEP_TXT2,"Step (%s)",units);
		break;

	case MODE_LIST: 
		PrintfDlg(hDlgChild,IDD_RUN_EDIT,"%2.0f %Fs",Present_Value,(LPSTR)units);
		break;
	}	

	enterv.Set_Units(new_units);
	stopv.Set_Units(new_units);
	singlev.Set_Units(new_units);
	startv.Set_Units(new_units);
}


void DUALS::Value_Changed()
{
	if ((Update_Values) && (Update_Window))
	{
		Refresh_Values();
		PrintfDlg(hDlgChild,IDD_RUN_EDIT,"%2.0f %Fs",Present_Value,(LPSTR)units);
	}
}

void DUALS::Refresh_Values()
{
	float fv;
	long far *lv=(long far *)&fv;
	if (hidden)  fv=-1.0; else
	fv=(float)Present_Value;
	SendMessage(Update_Window,Update_Message,0,*lv);
}


float DUALS::Get_Max_Value()
{
	switch (mode)
	{
	case MODE_FIXED://Fixed
	case MODE_F8014:
	case MODE_CHIRP:
		return (float)singlev.Value();

	case MODE_LINEAR://Linear
	case MODE_LOG://Log
		if (stopv.Value() >startv.Value()) 
			return (float)stopv.Value();
		else 
			return (float)startv.Value();

	case MODE_LIST://List
	{
		long maxval = startset;
		for (int i = 1;;i++)
		{
			long val=SendDlgItemMessage(hDlgChild,IDD_LIST,LB_GETITEMDATA,i,0L);
			if (val==0x40000001) break;
			if (val > maxval) maxval = val;
		}
		return (float)maxval;
	}

	default: 
		return 0.0;
	}
}

float DUALS::Get_Min_Value()
{
	switch (mode)
	{
	case MODE_FIXED://Fixed
	case MODE_F8014:
	case MODE_CHIRP:
		return (float)singlev.Value();
	
	case MODE_LINEAR://Linear
	case MODE_LOG://Log
		if (stopv.Value() <startv.Value()) return (float)stopv.Value();
		else return (float)startv.Value();
	
	case MODE_LIST://List
	{
		long minval = endset;
        int i;
		for (i = 1;;i++)
		{
			long val=SendDlgItemMessage(hDlgChild,IDD_LIST,LB_GETITEMDATA,i,0L);
			if (val==0x40000001) break;
			if (val < minval) 
				minval = val;
		}
		if (i == 1) return (float)startset;       // empty list
		else        return (float)minval;
		}
		default: 
		return 0.0;
	}
}

void DUALS::Hide(BOOL b)
{
	if (b==hidden) return;
	if (!b)
		ShowWindow(hDlgChild,SW_SHOWNORMAL);
	else
		ShowWindow(hDlgChild,SW_HIDE);
	hidden=b;
	ichanged=TRUE;
}

BOOL DUALS::fixed()
{
	if ((mode==MODE_FIXED) || (mode==MODE_F8014) ||  (mode==MODE_CHIRP) || (hidden)) 
		return 1;
	else 
		return 0;
}

void DUALS::Clear(void)
{
	startv.init(hDlgChild,IDD_START_SB,IDD_START_EDIT,startset,endset,initset,expset,stepset,RGB(0,255,0),units);
	stopv.init(hDlgChild,IDD_STOP_SB,IDD_STOP_EDIT,startset,endset,initset,expset,stepset,RGB(255,0,0),units);
	enterv.init(hDlgChild,IDD_VAL_SCROLL,IDD_VAL_EDIT,startset,endset,initset,expset,stepset,RGB(0,255,0),units);
	singlev.init(hDlgChild,IDD_SINGLE_SB,IDD_SINGLE_EDIT,startset,endset,initset,expset,stepset,RGB(0,255,0),units);
	enterv.Hide(TRUE);
	startv.Hide(TRUE);
	stopv.Hide(TRUE);
	singlev.Hide(FALSE);

	ShowWindow( GetDlgItem( hDlgChild,IDD_INSERT      ), SW_HIDE );
	ShowWindow( GetDlgItem( hDlgChild,IDD_DELETE      ), SW_HIDE );
	ShowWindow( GetDlgItem( hDlgChild,IDD_VAL_SCROLL  ), SW_HIDE );
	ShowWindow( GetDlgItem( hDlgChild,IDD_VAL_EDIT    ), SW_HIDE );
	ShowWindow( GetDlgItem( hDlgChild,IDD_LIST  ), SW_HIDE );

	ShowWindow( GetDlgItem( hDlgChild,IDD_START_TXT   ), SW_HIDE );
	ShowWindow( GetDlgItem( hDlgChild,IDD_STOP_TXT    ), SW_HIDE );
	ShowWindow( GetDlgItem( hDlgChild,IDD_STEPS_EDIT  ), SW_HIDE );
	ShowWindow( GetDlgItem( hDlgChild,IDD_STEPS_TXT   ), SW_HIDE );
	ShowWindow( GetDlgItem( hDlgChild,IDD_SINGLE_LB   ), SW_HIDE );
	ShowWindow( GetDlgItem( hDlgChild,IDD_STEP_TXT2   ), SW_HIDE );
	ShowWindow( GetDlgItem( hDlgChild,IDD_STEP_BUTTON ), SW_HIDE);
	ShowWindow( GetDlgItem( hDlgChild,IDD_EXTRA_CB    ), SW_HIDE);
	ShowWindow( GetDlgItem( hDlgChild,IDD_EXTRA_LAB   ), SW_HIDE);
	ShowWindow( GetDlgItem( hDlgChild,IDD_EXTRA_TEXT  ), SW_HIDE);
	ShowWindow( GetDlgItem( hDlgChild,IDD_TEXT_HI     ), SW_HIDE);
	ShowWindow( GetDlgItem( hDlgChild,IDD_RUN_TEXT    ), SW_HIDE);
	ShowWindow( GetDlgItem( hDlgChild,IDD_RUN_EDIT    ), SW_HIDE);
	ShowWindow( GetDlgItem( hDlgChild,IDD_MODE_LAB    ), SW_HIDE);


	SetDlgItemText(hDlgChild,IDD_STEPS_TXT,"Change");
	SetDlgItemText(hDlgChild,IDD_STEP_TXT2,"Step");
	N_steps=1;
	SendDlgItemMessage(hDlgChild,IDD_MODE1_CB,CB_SETCURSEL,0,(LONG)((LPSTR)"Fixed"));

	SendDlgItemMessage(hDlgChild,IDD_LIST,LB_RESETCONTENT,0,0L);
	SendDlgItemMessage(hDlgChild,IDD_LIST,LB_ADDSTRING,0,0x40000000L);
	SendDlgItemMessage(hDlgChild,IDD_LIST,LB_ADDSTRING,0,0x40000001L);
	SendDlgItemMessage(hDlgChild,IDD_LIST,LB_SETCURSEL,0,0L);
	if (List_Shown) 
	{ 
		ShowWindow( GetDlgItem( hDlgChild,IDD_EXTRA_CB    ), SW_SHOWNORMAL);
		ShowWindow( GetDlgItem( hDlgChild,IDD_EXTRA_TEXT    ), SW_SHOWNORMAL);
		SendDlgItemMessage(hDlgChild,IDD_EXTRA_CB,CB_SETCURSEL,0,0L); 
		Old_sel=0; 
	}

	mode=0;
	ichanged=TRUE;
}

int DUALS::SaveText(HANDLE hfile)
{
	int n;
	int i;
	long Present_Value;
	switch (mode)
	{ 
	case  MODE_FIXED:
		fhprintf(hfile,"%Fs:Fixed %d %Fs\r\n",(LPSTR)Obj_Name,(int)singlev.Value(),(LPSTR)units);
		break;

	case  MODE_LINEAR:
		fhprintf(hfile,"%Fs:Linear %d to %d %Fs",(LPSTR)Obj_Name,(int)startv.Value(),(int)stopv.Value(),(LPSTR)units);
		if (step_type) 
			fhprintf(hfile," in  %d steps \r\n",N_steps);
		else 
			fhprintf(hfile," stepping %d %Fs\r\n",Step_Delta,(LPSTR)units);
		break;

	case  MODE_LOG:
		fhprintf(hfile,"%Fs:Log %d to %d %Fs in %d steps\r\n",(LPSTR)Obj_Name,(int)startv.Value(),(int)stopv.Value(),(LPSTR)units,N_steps);
		break;
	
	case  MODE_LIST:
		fhprintf(hfile,"%Fs:List of %Fs\r\n",(LPSTR)Obj_Name,(LPSTR)units);
		n=( SendDlgItemMessage(hDlgChild,IDD_LIST,LB_GETCOUNT,0,0L)-2);
		for (i=0; i<n; i++)
		{
			Present_Value=SendDlgItemMessage(hDlgChild,IDD_LIST,LB_GETITEMDATA,i+1,0L);
			fhprintf(hfile,"%Fs:Step:%d\r\n",(LPSTR)Obj_Name,(int)Present_Value);
		}
		break;
	
	case  MODE_F8014:
		fhprintf(hfile,"%s:801-4\r\n",Obj_Name);
		break;
	
	case  MODE_CHIRP:
		fhprintf(hfile,"%s:Chirp\r\n",Obj_Name);
		break;
	}
	return 1;
}                    

int DUALS::Process_Line(LPSTR fbuffer)
{
	long temp1;
	long temp2;
	long temp3;
	long cp;
	char buffer[100];
	char tbuf[100];
	strcpy(buffer,fbuffer);

	if (_fstrstr(buffer,"Fixed")==buffer)
	{  
		sscanf(buffer,"Fixed %ld",&temp1);
		load_limitcheck(temp1);
		mode=99;
		SendDlgItemMessage(hDlgChild,IDD_MODE1_CB,CB_SETCURSEL,MODE_FIXED,0L);
		SendMessage(hDlgChild,WM_COMMAND,MAKELONG(IDD_MODE1_CB,CBN_SELCHANGE),(LPARAM) GetDlgItem(hDlgChild,IDD_MODE1_CB));
		singlev.Set_Value(temp1);
	}
	else if ((_fstrstr(buffer,"Linear")==buffer) && (_fstrstr(buffer," in ")))
	{  
		sscanf(buffer,"Linear %ld to %ld %s in %ld",&temp1,&temp2,tbuf,&temp3);
		load_limitcheck(temp1);
		load_limitcheck(temp2);
		startv.Set_Value(temp1);
		stopv.Set_Value(temp2);
		mode=99;
		SendDlgItemMessage(hDlgChild,IDD_MODE1_CB,CB_SETCURSEL,MODE_LINEAR,0L);
		SendMessage(hDlgChild,WM_COMMAND,MAKELONG(IDD_MODE1_CB,CBN_SELCHANGE),(LPARAM) GetDlgItem(hDlgChild,IDD_MODE1_CB));
		N_steps=temp3;
		SetDlgItemInt(hDlgChild,IDD_STEPS_EDIT,temp3,FALSE);
		step_type=TRUE;
		PostMessage(hDlgChild,WM_COMMAND,UPDATE_STEP,0L);
	}
	else if ((_fstrstr(buffer,"Linear")==buffer) && (_fstrstr( buffer," stepping ")))
	{  
		sscanf(buffer,"Linear %ld to %ld %s stepping %ld",&temp1,&temp2,tbuf,&temp3);
		load_limitcheck(temp1);
		load_limitcheck(temp2);
		startv.Set_Value(temp1);
		stopv.Set_Value(temp2);
		mode=99;
		SendDlgItemMessage(hDlgChild,IDD_MODE1_CB,CB_SETCURSEL,MODE_LINEAR,0L);
		SendMessage(hDlgChild,WM_COMMAND,MAKELONG(IDD_MODE1_CB,CBN_SELCHANGE), (LPARAM) GetDlgItem(hDlgChild,IDD_MODE1_CB));
		Step_Delta=temp3;
		SetDlgItemInt(hDlgChild,IDD_STEPS_EDIT,temp3,FALSE);
		step_type=FALSE;
		PostMessage(hDlgChild,WM_COMMAND,UPDATE_STEP,0L);
	}
	else if ((_fstrstr(buffer,"Log")==buffer) && (_fstrstr( buffer," in ")))
	{  
		sscanf(buffer,"Log %ld to %ld %s in %ld",&temp1,&temp2,tbuf,&temp3);
		load_limitcheck(temp1);
		load_limitcheck(temp2);
		startv.Set_Value(temp1);
		stopv.Set_Value(temp2);
		mode=99;
		SendDlgItemMessage(hDlgChild,IDD_MODE1_CB,CB_SETCURSEL,MODE_LOG,0L);
		SendMessage(hDlgChild,WM_COMMAND,MAKELONG(IDD_MODE1_CB,CBN_SELCHANGE), (LPARAM) GetDlgItem(hDlgChild,IDD_MODE1_CB));
		N_steps=temp3;
		Step_Delta=temp3;
		SetDlgItemInt(hDlgChild,IDD_STEPS_EDIT,temp3,FALSE);
		step_type=FALSE;
		PostMessage(hDlgChild,WM_COMMAND,UPDATE_STEP,0L);
	}
	else if (_fstrstr(buffer,"801-4")==buffer)
	{  
		mode=99;
		SendDlgItemMessage(hDlgChild,IDD_MODE1_CB,CB_SETCURSEL,MODE_F8014,0L);
		SendMessage(hDlgChild,WM_COMMAND,MAKELONG(IDD_MODE1_CB,CBN_SELCHANGE), (LPARAM) GetDlgItem(hDlgChild,IDD_MODE1_CB));
	}
	else if (_fstrstr(buffer,"Chirp")==buffer)
	{  
		mode=99;
		SendDlgItemMessage(hDlgChild,IDD_MODE1_CB,CB_SETCURSEL,MODE_CHIRP,0L);
		SendMessage(hDlgChild,WM_COMMAND,MAKELONG(IDD_MODE1_CB,CBN_SELCHANGE), (LPARAM) GetDlgItem(hDlgChild,IDD_MODE1_CB));
	}
	else if (_fstrstr(buffer,"List")==buffer)
	{  
		mode=99;
		SendDlgItemMessage(hDlgChild,IDD_MODE1_CB,CB_SETCURSEL,MODE_NEW_LIST,0L);
		SendMessage(hDlgChild,WM_COMMAND,MAKELONG(IDD_MODE1_CB,CBN_SELCHANGE), (LPARAM) GetDlgItem(hDlgChild,IDD_MODE1_CB));
	}
	else if (_fstrstr(buffer,"Step")==buffer)
	{ 
		sscanf(buffer,"Step:%ld",&temp1);
		load_limitcheck(temp1);
		cp=SendDlgItemMessage(hDlgChild,IDD_LIST,LB_GETTOPINDEX,0,0L);
		if (SendDlgItemMessage(hDlgChild,IDD_LIST,LB_GETITEMDATA,cp+1,0L)==0x40000001)
			SendDlgItemMessage(hDlgChild,IDD_LIST,LB_INSERTSTRING,cp+1,(long)temp1);
		else if (SendDlgItemMessage(hDlgChild,IDD_LIST,LB_GETITEMDATA,cp+2,0L)==0x40000001)
			SendDlgItemMessage(hDlgChild,IDD_LIST,LB_INSERTSTRING,cp+2,(LONG)temp1);
		else
		{ 
			SendDlgItemMessage(hDlgChild,IDD_LIST,LB_INSERTSTRING,cp+3,(LONG)temp1);
			SendDlgItemMessage(hDlgChild,IDD_LIST,LB_SETTOPINDEX,cp+2,0L);
		}
	}
	else 
	{
		Message("Unknow String to Parse!","Unknown:\n%Fs",buffer);
		return 1;
	}

	return 0;
}

void DUALS::SetListStr(LPSTR str)
{
	Old_sel=SendDlgItemMessage(hDlgChild,IDD_EXTRA_CB,CB_SELECTSTRING,-1,(long)str);
}
           
void DUALS::SetListN(int i)
{ 
	SendDlgItemMessage(hDlgChild,IDD_EXTRA_CB,CB_SETCURSEL,i,0L);
	Old_sel=i;
}
 
void DUALS::load_limitcheck(long &value)
{
	if (value>endset)
	{
		Message("Load Value Error:","%Fs value of %ld exceedes limit of %ld",Obj_Name,value,endset);
		value=endset;
	}
	else if (value<startset)
	{
		Message("Load Value Error:","%Fs value of %ld is less than the minimum of %ld",Obj_Name,value,startset);
		value=startset;
	}
}


void DUALS::setup_mode(int modev)
{  
	switch (modev)
	{
	case MODE_FIXED://Fixed
		if (Special_F8014)
			SetDlgItemText(hDlgChild,IDD_TEXT_LO,"Freqency");
		ShowWindow( GetDlgItem( hDlgChild,IDD_INSERT      ), SW_HIDE );
		ShowWindow( GetDlgItem( hDlgChild,IDD_DELETE      ), SW_HIDE );
		ShowWindow( GetDlgItem( hDlgChild,IDD_LIST  ), SW_HIDE );
		ShowWindow( GetDlgItem( hDlgChild,IDD_SINGLE_LB   ), SW_HIDE );
		ShowWindow( GetDlgItem( hDlgChild,IDD_START_TXT   ), SW_HIDE );
		ShowWindow( GetDlgItem( hDlgChild,IDD_STOP_TXT    ), SW_HIDE );
		ShowWindow( GetDlgItem( hDlgChild,IDD_STEPS_EDIT  ), SW_HIDE );
		ShowWindow( GetDlgItem( hDlgChild,IDD_STEPS_TXT   ), SW_HIDE );
		ShowWindow( GetDlgItem( hDlgChild,IDD_STEP_BUTTON ), SW_HIDE);
		ShowWindow( GetDlgItem( hDlgChild,IDD_STEP_TXT2   ), SW_HIDE);
		ShowWindow( GetDlgItem( hDlgChild,IDD_TEXT_HI     ), SW_HIDE);
		ShowWindow( GetDlgItem( hDlgChild,IDD_TEXT_LO     ), SW_SHOWNORMAL);
		startv.Hide(TRUE);
		stopv.Hide(TRUE);
		singlev.Hide(FALSE);
		enterv.Hide(TRUE);
		InvalidateRect(hDlgChild,NULL,TRUE);
		break;

	case MODE_LOG://Log
		step_type=1;
		ShowWindow( GetDlgItem( hDlgChild,IDD_STEP_BUTTON ), SW_HIDE);

	case MODE_LINEAR://Linear
		if (modev==MODE_LINEAR) 
			step_type=linear_step_type;
		if (Special_F8014)
			SetDlgItemText(hDlgChild,IDD_TEXT_LO,"Freqency");
		ShowWindow( GetDlgItem( hDlgChild,IDD_INSERT      ), SW_HIDE );
		ShowWindow( GetDlgItem( hDlgChild,IDD_DELETE      ), SW_HIDE );
		ShowWindow( GetDlgItem( hDlgChild,IDD_LIST  ), SW_HIDE );
		ShowWindow( GetDlgItem( hDlgChild,IDD_SINGLE_LB   ), SW_HIDE );
		ShowWindow( GetDlgItem( hDlgChild,IDD_START_TXT   ), SW_SHOWNORMAL );
		ShowWindow( GetDlgItem( hDlgChild,IDD_STOP_TXT    ), SW_SHOWNORMAL );
		ShowWindow( GetDlgItem( hDlgChild,IDD_STEPS_EDIT  ), SW_SHOWNORMAL );
		ShowWindow( GetDlgItem( hDlgChild,IDD_STEPS_TXT   ), SW_SHOWNORMAL );
		if (modev==1)
			ShowWindow( GetDlgItem( hDlgChild,IDD_STEP_BUTTON ), SW_SHOWNORMAL);
		ShowWindow( GetDlgItem( hDlgChild,IDD_STEP_TXT2   ), SW_SHOWNORMAL);
		ShowWindow( GetDlgItem( hDlgChild,IDD_TEXT_LO     ), SW_HIDE);
		ShowWindow( GetDlgItem( hDlgChild,IDD_TEXT_HI     ), SW_SHOWNORMAL);
		PostMessage(hDlgChild,WM_COMMAND,UPDATE_STEP,0L);
		startv.Hide(FALSE);
		stopv.Hide(FALSE);
		singlev.Hide(TRUE);
		enterv.Hide(TRUE);

		InvalidateRect(hDlgChild,NULL,TRUE);
		break;

	case MODE_NEW_LIST:
		SendDlgItemMessage(hDlgChild,IDD_LIST,LB_RESETCONTENT,0,0L);
		SendDlgItemMessage(hDlgChild,IDD_LIST,LB_ADDSTRING,0,0x40000000L);
		SendDlgItemMessage(hDlgChild,IDD_LIST,LB_ADDSTRING,0,0x40000001L);
		DBGOUT("DUALS::setup_mode - MODE_NEW_LIST.\n");
		mode=MODE_LIST;
		SendDlgItemMessage(hDlgChild,IDD_MODE1_CB,CB_SETCURSEL,3,0L);

	case MODE_LIST://List
		if (Special_F8014)SetDlgItemText(hDlgChild,IDD_TEXT_LO,"Freqency");
		ShowWindow( GetDlgItem( hDlgChild,IDD_INSERT      ), SW_SHOWNORMAL );
		ShowWindow( GetDlgItem( hDlgChild,IDD_DELETE      ), SW_SHOWNORMAL );
		ShowWindow( GetDlgItem( hDlgChild,IDD_LIST  ), SW_SHOWNORMAL );
		ShowWindow( GetDlgItem( hDlgChild,IDD_START_TXT   ), SW_HIDE );
		ShowWindow( GetDlgItem( hDlgChild,IDD_STOP_TXT    ), SW_HIDE );
		ShowWindow( GetDlgItem( hDlgChild,IDD_STEPS_EDIT  ), SW_HIDE );
		ShowWindow( GetDlgItem( hDlgChild,IDD_STEPS_TXT   ), SW_HIDE );
		ShowWindow( GetDlgItem( hDlgChild,IDD_STEP_BUTTON ), SW_HIDE);
		ShowWindow( GetDlgItem( hDlgChild,IDD_STEP_TXT2   ), SW_HIDE);
		ShowWindow( GetDlgItem( hDlgChild,IDD_TEXT_LO     ), SW_HIDE);
		ShowWindow( GetDlgItem( hDlgChild,IDD_TEXT_HI     ), SW_SHOWNORMAL);
		startv.Hide(TRUE);
		stopv.Hide(TRUE);
		singlev.Hide(TRUE);
		enterv.Hide(FALSE);
		InvalidateRect(hDlgChild,NULL,FALSE);
		break;

	case MODE_F8014://F8014 Freq
		SetDlgItemText(hDlgChild,IDD_TEXT_LO,"Freqency 5kHz for V<=2kV,  2.5kHz for V>2kV");
		ShowWindow( GetDlgItem( hDlgChild,IDD_SINGLE_LB   ), SW_HIDE );
		ShowWindow( GetDlgItem( hDlgChild,IDD_START_TXT   ), SW_HIDE );
		ShowWindow( GetDlgItem( hDlgChild,IDD_STOP_TXT    ), SW_HIDE );
		ShowWindow( GetDlgItem( hDlgChild,IDD_STEPS_EDIT  ), SW_HIDE );
		ShowWindow( GetDlgItem( hDlgChild,IDD_STEPS_TXT   ), SW_HIDE );
		ShowWindow( GetDlgItem( hDlgChild,IDD_STEP_BUTTON ), SW_HIDE);
		ShowWindow( GetDlgItem( hDlgChild,IDD_STEP_TXT2   ), SW_HIDE);
		ShowWindow( GetDlgItem( hDlgChild,IDD_TEXT_HI     ), SW_HIDE);
		ShowWindow( GetDlgItem( hDlgChild,IDD_TEXT_LO     ), SW_SHOWNORMAL);
		ShowWindow( GetDlgItem( hDlgChild,IDD_INSERT      ), SW_HIDE );
		ShowWindow( GetDlgItem( hDlgChild,IDD_DELETE      ), SW_HIDE );
		ShowWindow( GetDlgItem( hDlgChild,IDD_LIST  ), SW_HIDE );
		enterv.Hide(TRUE);

		startv.Hide(TRUE);
		stopv.Hide(TRUE);
		singlev.Hide(TRUE);
		InvalidateRect(hDlgChild,NULL,TRUE);
		break;

	case MODE_CHIRP://F8014 Freq
		SetDlgItemText(hDlgChild,IDD_TEXT_LO,"Chirp Waveform ");
		ShowWindow( GetDlgItem( hDlgChild,IDD_SINGLE_LB   ), SW_HIDE );
		ShowWindow( GetDlgItem( hDlgChild,IDD_START_TXT   ), SW_HIDE );
		ShowWindow( GetDlgItem( hDlgChild,IDD_STOP_TXT    ), SW_HIDE );
		ShowWindow( GetDlgItem( hDlgChild,IDD_STEPS_EDIT  ), SW_HIDE );
		ShowWindow( GetDlgItem( hDlgChild,IDD_STEPS_TXT   ), SW_HIDE );
		ShowWindow( GetDlgItem( hDlgChild,IDD_STEP_BUTTON ), SW_HIDE);
		ShowWindow( GetDlgItem( hDlgChild,IDD_STEP_TXT2   ), SW_HIDE);
		ShowWindow( GetDlgItem( hDlgChild,IDD_TEXT_HI     ), SW_HIDE);
		ShowWindow( GetDlgItem( hDlgChild,IDD_TEXT_LO     ), SW_SHOWNORMAL);
		startv.Hide(TRUE);
		stopv.Hide(TRUE);
		singlev.Hide(TRUE);
		InvalidateRect(hDlgChild,NULL,TRUE);
		break;
	}//Switch Selected case
}

#include "stdafx.h"
#include "comm.h"
#include "globals.h"
#include "meas.h"
#include "globals.h"
#include "module.h"
#include "message.h"

/*--------------------------------------------------------------------

  MEAS.CPP -- Surge Measurement Dialog Box.

  History:	Starting with 4.00r
		10/17/95	JLR	Added IsAuto() to check rules to allow auto mode.
----------------------------------------------------------------------*/

#define VP_BIT (1)
#define VM_BIT (2)
#define IP_BIT (4)
#define IM_BIT (8)

const char srg_names[6][3]   ={"NA","1","2","3","4","5"};
const char tricpl_names[6][3]={"NA","L1","L2","L3","N","PE"};
const char cpl_names[6][3]   ={"NA","L1","L2","PE","NA","NA"};
const char io5_names[6][3]   ={"NA","S1","S2","S3","S4","G"};
const char io_names[6][3]    ={"NA","S1","S2","G", "NA","NA"};


LPSTR Get_imeas_name(int i,int j)
{
	if (calset[i].type & TYPE_CPLIO)
	{  
		if (calset[i].options & INSTR_5CH_BIT)
			return (LPSTR)io5_names[j];
		else 
			return (LPSTR)io_names[j];
	}
	else if (calset[i].type & (TYPE_CPLSRG | TYPE_CPLEFT))
	{  
		if ((calset[i].type    & TYPE_CPLTRI) || (calset[i].options & INSTR_5CH_BIT))
			return (LPSTR)tricpl_names[j];
		else 
			return (LPSTR)cpl_names[j];
	}
	else
		return (LPSTR)srg_names[j];
}

void MEAS::Init(HANDLE hInstance,HWND hwnd, RECT& rect, LPSTR text)
{
	running=0;
	Update_Values=FALSE;
	Update_Window=NULL;
	Update_Message=0;
	last_meas=-1;
	curr_meas_value=-1;
	out_of_limits=0;
	active_bay=-99;
	yBrush=NULL;
	rBrush=NULL;
	create(hInstance,hwnd,rect,MAKEINTRESOURCE(IDD_MEASURE));
	ShowWindow(hDlgChild,SW_SHOWNORMAL); 
	strcpy(Obj_Name,text);
	created=TRUE;
}


BOOL MEAS::DlgProc (HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	int i,j;
	int cp;
	UINT tempw;
	HBRUSH lhBrush;
	POINT point;

	/*1*/
	switch (message)
	{/*2Main Message Switch */
	case WM_INITDIALOG:
		yBrush=CreateSolidBrush(RGB(255,255,0));
		rBrush=CreateSolidBrush(RGB(255,0,0));

		SendDlgItemMessage(hDlg,IDD_MEAS_BAY,CB_RESETCONTENT,0,0L);
		for (i=0; i<MAX_CAL_SETS; i++) 
			if (NOT_GHOST(i))
			{
				if ((calset[i].vmeas!=0) || (calset[i].imeas!=0))
				{ 
					cp=SendDlgItemMessage(hDlg,IDD_MEAS_BAY,CB_ADDSTRING,0,(LONG)(LPSTR)calset[i].name);
					SendDlgItemMessage(hDlg,IDD_MEAS_BAY,CB_SETITEMDATA,cp,(LONG)i);
				}
			}

		SendDlgItemMessage(hDlg,IDD_MEAS_BAY,CB_SETCURSEL,0,0L);
		GTS[0]=GTS[1]=GTS[2]=GTS[3]='>';
		SendMessage(hDlg,WM_COMMAND,MAKEWPARAM(IDD_MEAS_BAY,CBN_SELCHANGE),(LPARAM) GetDlgItem(hDlg, IDD_MEAS_BAY));
		return FALSE;

	case WM_CTLCOLORSTATIC:
	case WM_CTLCOLOREDIT:
		{
			tempw=GetWindowLong((HWND)lParam,GWL_ID);
			switch(tempw)
			{
			case IDD_VREADP:
			case IDD_IREADP:
			case IDD_VREADM:
			case IDD_IREADM:
				if (
					((tempw==IDD_VREADP) && (out_of_limits & VP_BIT)) 
					||((tempw==IDD_VREADM) && (out_of_limits & VM_BIT)) 
					||((tempw==IDD_IREADP) && (out_of_limits & IP_BIT)) 
					||((tempw==IDD_IREADM) && (out_of_limits & IM_BIT)) 
				)
				{
					SetBkColor((HDC)wParam,RGB(255,0,0)); 	   // red
					lhBrush=rBrush; 
				}
				else
				{ 
					SetBkColor((HDC)wParam,RGB(255,255,0));	   // yellow
					lhBrush=yBrush;
				}
				UnrealizeObject(lhBrush);
				point.x=point.y=0;
				ClientToScreen(hDlg,&point);
				POINT ptOld;
				SetBrushOrgEx((HDC)wParam,point.x,point.y,&ptOld);
				return ((DWORD)lhBrush);

			default:
				return NULL;
			}//The proper Edit case
		}
		break;

	case WM_COMMAND:
		switch (LOWORD(wParam))
		{/*WM_COMMAND SWITCH */
		case IDD_ISETP:
		case IDD_ISETM:
		case IDD_VSETP:
		case IDD_VSETM:
			ichanged=TRUE;
			break;

		case IDD_PVGT:
		case IDD_MVGT:
		case IDD_PIGT:
		case IDD_MIGT:
			ichanged=TRUE;
			if (GTS[wParam-IDD_PVGT]=='>')
			{
				GTS[wParam-IDD_PVGT]='<';
				SetDlgItemText(hDlg,wParam,"<");
			}
			else 
			{
				GTS[wParam-IDD_PVGT]='>';
				SetDlgItemText(hDlg,wParam,">");
			}
			break;

		case IDD_MEAS_BAY:
			if (HIWORD(wParam)==CBN_SELCHANGE)
			{
				ichanged=TRUE;
				cp=SendDlgItemMessage(hDlg,IDD_MEAS_BAY,CB_GETCURSEL,0,0L);
				if (cp==CB_ERR) 
					i=-99;
				else 
					i=SendDlgItemMessage(hDlg,IDD_MEAS_BAY,CB_GETITEMDATA,cp,0L);

				if (i!=active_bay)
				{
					SendDlgItemMessage(hDlg,IDD_ISETCB,CB_RESETCONTENT,0,0L);
					SendDlgItemMessage(hDlg,IDD_VSETCB,CB_RESETCONTENT,0,0L);
					if ((i>=0) && (i<MAX_CAL_SETS) && NOT_GHOST(i))
					{
						if ((calset[i].vmeas>=1) && (calset[i].vmeas<5))
						{
							switch (calset[i].vmeas % 5)
							{ 
							case 4:/*A-E B-E C-E D-E */
								cp=SendDlgItemMessage(hDlg,IDD_VSETCB,CB_ADDSTRING,0,(LONG)(LPSTR)"A-B");
								SendDlgItemMessage(hDlg,IDD_VSETCB,CB_SETITEMDATA,cp,0x012L);
								cp=SendDlgItemMessage(hDlg,IDD_VSETCB,CB_ADDSTRING,0,(LONG)(LPSTR)"A-C");
								SendDlgItemMessage(hDlg,IDD_VSETCB,CB_SETITEMDATA,cp,0x013L);
								cp=SendDlgItemMessage(hDlg,IDD_VSETCB,CB_ADDSTRING,0,(LONG)(LPSTR)"A-D");
								SendDlgItemMessage(hDlg,IDD_VSETCB,CB_SETITEMDATA,cp,0x014L);
								cp=SendDlgItemMessage(hDlg,IDD_VSETCB,CB_ADDSTRING,0,(LONG)(LPSTR)"A-E");
								SendDlgItemMessage(hDlg,IDD_VSETCB,CB_SETITEMDATA,cp,0x015L);
								cp=SendDlgItemMessage(hDlg,IDD_VSETCB,CB_ADDSTRING,0,(LONG)(LPSTR)"B-A");
								SendDlgItemMessage(hDlg,IDD_VSETCB,CB_SETITEMDATA,cp,0x021L);
								cp=SendDlgItemMessage(hDlg,IDD_VSETCB,CB_ADDSTRING,0,(LONG)(LPSTR)"B-C");
								SendDlgItemMessage(hDlg,IDD_VSETCB,CB_SETITEMDATA,cp,0x023L);
								cp=SendDlgItemMessage(hDlg,IDD_VSETCB,CB_ADDSTRING,0,(LONG)(LPSTR)"B-D");
								SendDlgItemMessage(hDlg,IDD_VSETCB,CB_SETITEMDATA,cp,0x024L);
								cp=SendDlgItemMessage(hDlg,IDD_VSETCB,CB_ADDSTRING,0,(LONG)(LPSTR)"B-E");
								SendDlgItemMessage(hDlg,IDD_VSETCB,CB_SETITEMDATA,cp,0x025L);
								cp=SendDlgItemMessage(hDlg,IDD_VSETCB,CB_ADDSTRING,0,(LONG)(LPSTR)"C-A");
								SendDlgItemMessage(hDlg,IDD_VSETCB,CB_SETITEMDATA,cp,0x031L);
								cp=SendDlgItemMessage(hDlg,IDD_VSETCB,CB_ADDSTRING,0,(LONG)(LPSTR)"C-B");
								SendDlgItemMessage(hDlg,IDD_VSETCB,CB_SETITEMDATA,cp,0x032L);
								cp=SendDlgItemMessage(hDlg,IDD_VSETCB,CB_ADDSTRING,0,(LONG)(LPSTR)"C-D");
								SendDlgItemMessage(hDlg,IDD_VSETCB,CB_SETITEMDATA,cp,0x034L);
								cp=SendDlgItemMessage(hDlg,IDD_VSETCB,CB_ADDSTRING,0,(LONG)(LPSTR)"C-E");
								SendDlgItemMessage(hDlg,IDD_VSETCB,CB_SETITEMDATA,cp,0x035L);
								cp=SendDlgItemMessage(hDlg,IDD_VSETCB,CB_ADDSTRING,0,(LONG)(LPSTR)"D-A");
								SendDlgItemMessage(hDlg,IDD_VSETCB,CB_SETITEMDATA,cp,0x041L);
								cp=SendDlgItemMessage(hDlg,IDD_VSETCB,CB_ADDSTRING,0,(LONG)(LPSTR)"D-B");
								SendDlgItemMessage(hDlg,IDD_VSETCB,CB_SETITEMDATA,cp,0x042L);
								cp=SendDlgItemMessage(hDlg,IDD_VSETCB,CB_ADDSTRING,0,(LONG)(LPSTR)"D-C");
								SendDlgItemMessage(hDlg,IDD_VSETCB,CB_SETITEMDATA,cp,0x043L);
								cp=SendDlgItemMessage(hDlg,IDD_VSETCB,CB_ADDSTRING,0,(LONG)(LPSTR)"D-E");
								SendDlgItemMessage(hDlg,IDD_VSETCB,CB_SETITEMDATA,cp,0x045L);
								cp=SendDlgItemMessage(hDlg,IDD_VSETCB,CB_ADDSTRING,0,(LONG)(LPSTR)"E-A");
								SendDlgItemMessage(hDlg,IDD_VSETCB,CB_SETITEMDATA,cp,0x051L);
								cp=SendDlgItemMessage(hDlg,IDD_VSETCB,CB_ADDSTRING,0,(LONG)(LPSTR)"E-B");
								SendDlgItemMessage(hDlg,IDD_VSETCB,CB_SETITEMDATA,cp,0x052L);
								cp=SendDlgItemMessage(hDlg,IDD_VSETCB,CB_ADDSTRING,0,(LONG)(LPSTR)"E-C");
								SendDlgItemMessage(hDlg,IDD_VSETCB,CB_SETITEMDATA,cp,0x053L);
								cp=SendDlgItemMessage(hDlg,IDD_VSETCB,CB_ADDSTRING,0,(LONG)(LPSTR)"E-D");
								SendDlgItemMessage(hDlg,IDD_VSETCB,CB_SETITEMDATA,cp,0x054L);

								if (IsAuto(i, 5))    // JLR, 10/17/95
								{
									cp=SendDlgItemMessage(hDlg,IDD_VSETCB,CB_ADDSTRING,0,(LONG)(LPSTR)"Auto");
									SendDlgItemMessage(hDlg,IDD_VSETCB,CB_SETITEMDATA,cp,0x0FFL);
								}
								break;

							case 3:/*A-D B-D C-D*/
								cp=SendDlgItemMessage(hDlg,IDD_VSETCB,CB_ADDSTRING,0,(LONG)(LPSTR)"A-B");
								SendDlgItemMessage(hDlg,IDD_VSETCB,CB_SETITEMDATA,cp,0x012L);
								cp=SendDlgItemMessage(hDlg,IDD_VSETCB,CB_ADDSTRING,0,(LONG)(LPSTR)"A-C");
								SendDlgItemMessage(hDlg,IDD_VSETCB,CB_SETITEMDATA,cp,0x013L);
								cp=SendDlgItemMessage(hDlg,IDD_VSETCB,CB_ADDSTRING,0,(LONG)(LPSTR)"A-D");
								SendDlgItemMessage(hDlg,IDD_VSETCB,CB_SETITEMDATA,cp,0x014L);
								cp=SendDlgItemMessage(hDlg,IDD_VSETCB,CB_ADDSTRING,0,(LONG)(LPSTR)"B-A");
								SendDlgItemMessage(hDlg,IDD_VSETCB,CB_SETITEMDATA,cp,0x021L);
								cp=SendDlgItemMessage(hDlg,IDD_VSETCB,CB_ADDSTRING,0,(LONG)(LPSTR)"B-C");
								SendDlgItemMessage(hDlg,IDD_VSETCB,CB_SETITEMDATA,cp,0x023L);
								cp=SendDlgItemMessage(hDlg,IDD_VSETCB,CB_ADDSTRING,0,(LONG)(LPSTR)"B-D");
								SendDlgItemMessage(hDlg,IDD_VSETCB,CB_SETITEMDATA,cp,0x024L);
								cp=SendDlgItemMessage(hDlg,IDD_VSETCB,CB_ADDSTRING,0,(LONG)(LPSTR)"C-A");
								SendDlgItemMessage(hDlg,IDD_VSETCB,CB_SETITEMDATA,cp,0x031L);
								cp=SendDlgItemMessage(hDlg,IDD_VSETCB,CB_ADDSTRING,0,(LONG)(LPSTR)"C-B");
								SendDlgItemMessage(hDlg,IDD_VSETCB,CB_SETITEMDATA,cp,0x032L);
								cp=SendDlgItemMessage(hDlg,IDD_VSETCB,CB_ADDSTRING,0,(LONG)(LPSTR)"C-D");
								SendDlgItemMessage(hDlg,IDD_VSETCB,CB_SETITEMDATA,cp,0x034L);
								cp=SendDlgItemMessage(hDlg,IDD_VSETCB,CB_ADDSTRING,0,(LONG)(LPSTR)"D-A");
								SendDlgItemMessage(hDlg,IDD_VSETCB,CB_SETITEMDATA,cp,0x041L);
								cp=SendDlgItemMessage(hDlg,IDD_VSETCB,CB_ADDSTRING,0,(LONG)(LPSTR)"D-B");
								SendDlgItemMessage(hDlg,IDD_VSETCB,CB_SETITEMDATA,cp,0x042L);
								cp=SendDlgItemMessage(hDlg,IDD_VSETCB,CB_ADDSTRING,0,(LONG)(LPSTR)"D-C");
								SendDlgItemMessage(hDlg,IDD_VSETCB,CB_SETITEMDATA,cp,0x043L);
								break;

							case 2:/*A-C B-C */
								cp=SendDlgItemMessage(hDlg,IDD_VSETCB,CB_ADDSTRING,0,(LONG)(LPSTR)"A-B");
								SendDlgItemMessage(hDlg,IDD_VSETCB,CB_SETITEMDATA,cp,0x012L);
								cp=SendDlgItemMessage(hDlg,IDD_VSETCB,CB_ADDSTRING,0,(LONG)(LPSTR)"A-C");
								SendDlgItemMessage(hDlg,IDD_VSETCB,CB_SETITEMDATA,cp,0x013L);
								cp=SendDlgItemMessage(hDlg,IDD_VSETCB,CB_ADDSTRING,0,(LONG)(LPSTR)"B-A");
								SendDlgItemMessage(hDlg,IDD_VSETCB,CB_SETITEMDATA,cp,0x021L);
								cp=SendDlgItemMessage(hDlg,IDD_VSETCB,CB_ADDSTRING,0,(LONG)(LPSTR)"B-C");
								SendDlgItemMessage(hDlg,IDD_VSETCB,CB_SETITEMDATA,cp,0x023L);
								cp=SendDlgItemMessage(hDlg,IDD_VSETCB,CB_ADDSTRING,0,(LONG)(LPSTR)"C-A");
								SendDlgItemMessage(hDlg,IDD_VSETCB,CB_SETITEMDATA,cp,0x031L);
								cp=SendDlgItemMessage(hDlg,IDD_VSETCB,CB_ADDSTRING,0,(LONG)(LPSTR)"C-B");
								SendDlgItemMessage(hDlg,IDD_VSETCB,CB_SETITEMDATA,cp,0x032L);

								if (IsAuto(i, 3))		// JLR, 10/17/95
								{ 
									cp=SendDlgItemMessage(hDlg,IDD_VSETCB,CB_ADDSTRING,0,(LONG)(LPSTR)"Auto");
									SendDlgItemMessage(hDlg,IDD_VSETCB,CB_SETITEMDATA,cp,0x0FFL);
								}
								break;

							case 1:/*A-B */
								cp=SendDlgItemMessage(hDlg,IDD_VSETCB,CB_ADDSTRING,0,(LONG)(LPSTR)"A-B"); 
								SendDlgItemMessage(hDlg,IDD_VSETCB,CB_SETITEMDATA,cp,0x012L);
								cp=SendDlgItemMessage(hDlg,IDD_VSETCB,CB_ADDSTRING,0,(LONG)(LPSTR)"B-A"); 
								SendDlgItemMessage(hDlg,IDD_VSETCB,CB_SETITEMDATA,cp,0x021L);
								break;
							}//switch (calset[i].vmeas % 5)
						}//if (calset[i].vmeas>=1)
						else 
						{
							cp=SendDlgItemMessage(hDlg,IDD_VSETCB,CB_ADDSTRING,0,(LONG)(LPSTR)"None");
							SendDlgItemMessage(hDlg,IDD_VSETCB,CB_SETITEMDATA,cp,(LONG)-1);
						}//Not a valid bay


					if ((i>=0) && (i<MAX_CAL_SETS) && NOT_GHOST(i))
					{
						if ((calset[i].imeas>=1) && (calset[i].imeas<=5))
						{//imeas is valid
							for (j=1; j<=calset[i].imeas; j++)
							{
								cp=SendDlgItemMessage(hDlg,IDD_ISETCB,CB_ADDSTRING,0,(LONG)(LPSTR)Get_imeas_name(i,j));
								SendDlgItemMessage(hDlg,IDD_ISETCB,CB_SETITEMDATA,cp,(LONG)j);
							}

							if ((calset[i].imeas >= 2) && IsAuto(i, calset[i].imeas)) 	// JLR, 10/17/95
							{
								cp=SendDlgItemMessage(hDlg,IDD_ISETCB,CB_ADDSTRING,0,(LONG)(LPSTR)"Auto");
								SendDlgItemMessage(hDlg,IDD_ISETCB,CB_SETITEMDATA,cp,0x0FFL);
							}
						}
						else
						{//imeas is not valid
							cp=SendDlgItemMessage(hDlg,IDD_ISETCB,CB_ADDSTRING,0,(LONG)(LPSTR)"None");
							SendDlgItemMessage(hDlg,IDD_ISETCB,CB_SETITEMDATA,cp,(LONG)-1);
						}

						SendDlgItemMessage(hDlg,IDD_VSETCB,CB_SETCURSEL,0,0L);
						SendDlgItemMessage(hDlg,IDD_ISETCB,CB_SETCURSEL,0,0L);
					}//Valid module
				}//Active Bay is different
			}

			active_bay=i;
			cp=SendDlgItemMessage(hDlg,IDD_ISETCB,CB_GETCURSEL,0,0L);
			if (cp==CB_ERR) 
				i=0;
			else 
				i=SendDlgItemMessage(hDlg,IDD_ISETCB,CB_GETITEMDATA,cp,0L);
			cp=SendDlgItemMessage(hDlg,IDD_VSETCB,CB_GETCURSEL,0,0L);
			if (cp==CB_ERR) 
				j=0;
			else 
				j=SendDlgItemMessage(hDlg,IDD_VSETCB,CB_GETITEMDATA,cp,0L);

			curr_meas_value=MAKELONG(i,j);
			if (run_shown) Update_Measurments();
		}//IDD_MEAS_BAY and SEL_CHANGED
		break;

		case IDD_ISETCB:
		case IDD_VSETCB:
			if (HIWORD(wParam)==CBN_SELCHANGE)
			{
				ichanged=TRUE;
				cp=SendDlgItemMessage(hDlg,IDD_ISETCB,CB_GETCURSEL,0,0L);
				if (cp==CB_ERR) 
					i=0;
				else 
					i=SendDlgItemMessage(hDlg,IDD_ISETCB,CB_GETITEMDATA,cp,0L);

				cp=SendDlgItemMessage(hDlg,IDD_VSETCB,CB_GETCURSEL,0,0L);
				if (cp==CB_ERR) 
					j=0;
				else 
					j=SendDlgItemMessage(hDlg,IDD_VSETCB,CB_GETITEMDATA,cp,0L);
				curr_meas_value=MAKELONG(i,j);

				if (run_shown) 
					Update_Measurments();
				//Set up the current meas value
			}
			break;

		}/*3End WM COMMAND SWITCH */
		break;

	case WM_DESTROY:
		DeleteObject(yBrush);			    
		DeleteObject(rBrush);			    
		break;
	}/*2End Main Message case */

	return FALSE;
}/*1*/

void MEAS::Setup_Values(LONG couple_values)
{ 
	cp_value=couple_values;
	Update_Measurments();
}

void MEAS::Show_Run(BOOL state)
{
	run_shown=state;
	if (state) Update_Measurments();
} 

void MEAS::Update_Measurments()
{
	if (SendDlgItemMessage(hDlgChild,IDD_MEAS_BAY,CB_GETCOUNT,0,0) > 0)
	{
		if ((active_bay<0) || (active_bay>16))
		{  
			SendMessage(hDlgChild,WM_COMMAND,MAKEWPARAM(IDD_MEAS_BAY,CBN_SELCHANGE),(LPARAM) GetDlgItem(hDlgChild, IDD_MEAS_BAY));
			SendMessage(hDlgChild,WM_COMMAND,MAKEWPARAM(IDD_VSETCB,CBN_SELCHANGE),(LPARAM) GetDlgItem(hDlgChild, IDD_VSETCB));
			SendMessage(hDlgChild,WM_COMMAND,MAKEWPARAM(IDD_ISETCB,CBN_SELCHANGE),(LPARAM) GetDlgItem(hDlgChild, IDD_ISETCB));
		}

		if ((last_bay!=active_bay) || (last_meas!=curr_meas_value))
			SendMessage(Update_Window,Update_Message,active_bay,curr_meas_value);
	}
	last_bay=active_bay;
	last_meas=curr_meas_value;
}

void MEAS::Refresh_Values()
{
	Update_Measurments();
}

int MEAS::New_Numbers(int far *results)
{ 
	int dlimitv;
	int converted;
	out_of_limits=0;
	dlimitv=GetDlgItemInt(hDlgChild,IDD_VSETP,&converted,TRUE);
	if (converted)
	{ 
		if( ((GTS[0]=='>')&& (dlimitv<results[0])) ||
			((GTS[0]=='<')&& (dlimitv>results[0])) )
		{
			out_of_limits|=VP_BIT;
			Log.logprintf("%d %c %d Positive Voltage Limit Exceeded\n",results[0],GTS[0],dlimitv);
		}
	}//converted VSETP

	dlimitv=GetDlgItemInt(hDlgChild,IDD_VSETM,&converted,TRUE);
	if (converted)
	{ 
		if( ((GTS[1]=='>')&& (dlimitv<results[1])) ||
			((GTS[1]=='<')&& (dlimitv>results[1])) )
		{
			out_of_limits|=VM_BIT;
			Log.logprintf("%d %c %d Negative Voltage Limit Exceeded\n",results[1],GTS[1],dlimitv);
		}
	}//converted VSETM

	dlimitv=GetDlgItemInt(hDlgChild,IDD_ISETP,&converted,TRUE);
	if (converted)
	{ 
		if( ((GTS[2]=='>')&& (dlimitv<results[2])) ||
			((GTS[2]=='<')&& (dlimitv>results[2])) )
		{
			out_of_limits|=IP_BIT;
			Log.logprintf("%d %c %d Positive Current Limit Exceeded\n",results[2],GTS[2],dlimitv);
		}
	}//converted ISETP

	dlimitv=GetDlgItemInt(hDlgChild,IDD_ISETM,&converted,TRUE);
	if (converted)
	{ 
		if( ((GTS[3]=='>')&& (dlimitv<results[3])) ||
			((GTS[3]=='<')&& (dlimitv>results[3])) )
		{  
			out_of_limits|=IM_BIT;
			Log.logprintf("%d %c %d Negative Current Limit Exceeded\n",results[3],GTS[3],dlimitv);
		}
	}//converted ISETM

	SetDlgItemInt(hDlgChild,IDD_VREADP,results[0],TRUE);
	SetDlgItemInt(hDlgChild,IDD_VREADM,results[1],TRUE);
	SetDlgItemInt(hDlgChild,IDD_IREADP,results[2],TRUE);
	SetDlgItemInt(hDlgChild,IDD_IREADM,results[3],TRUE);
	InvalidateRect(GetDlgItem(hDlgChild,IDD_VREADP),NULL,TRUE);
	InvalidateRect(GetDlgItem(hDlgChild,IDD_VREADM),NULL,TRUE);
	InvalidateRect(GetDlgItem(hDlgChild,IDD_IREADP),NULL,TRUE);
	InvalidateRect(GetDlgItem(hDlgChild,IDD_IREADM),NULL,TRUE);
	return out_of_limits;
}   
  
  
void MEAS::Clear(void)
{
	int i;
	SendDlgItemMessage(hDlgChild,IDD_MEAS_BAY,CB_SETCURSEL,0,0L);
	SendDlgItemMessage(hDlgChild,IDD_VSETCB,CB_SETCURSEL,0,0L);
	SendDlgItemMessage(hDlgChild,IDD_ISETCB,CB_SETCURSEL,0,0L);
	SetDlgItemText(hDlgChild,IDD_VREADP,"");
	SetDlgItemText(hDlgChild,IDD_VREADM,"");
	SetDlgItemText(hDlgChild,IDD_IREADP,"");
	SetDlgItemText(hDlgChild,IDD_IREADM,"");
	SetDlgItemText(hDlgChild,IDD_VSETP,"");
	SetDlgItemText(hDlgChild,IDD_VSETM,"");
	SetDlgItemText(hDlgChild,IDD_ISETP,"");
	SetDlgItemText(hDlgChild,IDD_ISETM,"");
	GTS[0]=GTS[1]=GTS[2]=GTS[3]='>';

	for (i=0; i<4; i++)
		if (GTS[i]=='<') 
			SetDlgItemText(hDlgChild,i+IDD_PVGT,"<");
		else 
			SetDlgItemText(hDlgChild,i+IDD_PVGT,">");
	ichanged=TRUE;
}  

int MEAS::SaveText(HANDLE hfile)
{
	int converted;
	int limitv;
	long csel;
	char buffer[20];
	csel=SendDlgItemMessage(hDlgChild,IDD_MEAS_BAY,CB_GETCURSEL,0,0L);
	converted=SendDlgItemMessage(hDlgChild,IDD_MEAS_BAY,CB_GETLBTEXT,csel,(long)(LPSTR)buffer);
	if ((converted) &&(converted!=CB_ERR))
		fhprintf(hfile,"%Fs:At:%Fs\r\n",(LPSTR)Obj_Name,(LPSTR)buffer);

	csel=SendDlgItemMessage(hDlgChild,IDD_VSETCB,CB_GETCURSEL,0,0L);
	converted=SendDlgItemMessage(hDlgChild,IDD_VSETCB,CB_GETLBTEXT,csel,(long)(LPSTR)buffer);
	if ((converted) &&(converted!=CB_ERR))
		fhprintf(hfile,"%Fs:V At:%Fs\r\n",(LPSTR)Obj_Name,(LPSTR)buffer);

	csel=SendDlgItemMessage(hDlgChild,IDD_ISETCB,CB_GETCURSEL,0,0L);
	converted=SendDlgItemMessage(hDlgChild,IDD_ISETCB,CB_GETLBTEXT,csel,(long)(LPSTR)buffer);
	if ((converted) &&(converted!=CB_ERR))
		fhprintf(hfile,"%Fs:I At:%Fs\r\n",(LPSTR)Obj_Name,(LPSTR)buffer);

	limitv=GetDlgItemInt(hDlgChild,IDD_VSETP,&converted,TRUE);
	if (converted) 
		fhprintf(hfile,"%Fs:+V Limit %c %d\r\n",(LPSTR)Obj_Name,GTS[0],limitv);

	limitv=GetDlgItemInt(hDlgChild,IDD_VSETM,&converted,TRUE);
	if (converted) 
		fhprintf(hfile,"%Fs:-V Limit %c %d\r\n",(LPSTR)Obj_Name,GTS[1],limitv);

	limitv=GetDlgItemInt(hDlgChild,IDD_ISETP,&converted,TRUE);
	if (converted) 
		fhprintf(hfile,"%Fs:+I Limit %c %d\r\n",(LPSTR)Obj_Name,GTS[2],limitv);

	limitv=GetDlgItemInt(hDlgChild,IDD_ISETM,&converted,TRUE);
	if (converted) 
		fhprintf(hfile,"%Fs:-I Limit %c %d\r\n",(LPSTR)Obj_Name,GTS[3],limitv);

	return 1;
}

int MEAS::Process_Line(LPSTR buffer)
{
	char lbuff[80];
	int tempi;
	_fstrncpy(lbuff,buffer,80);

	if (_fstrstr(buffer,"+V Limit")==buffer)
	{
		sscanf(lbuff,"+V Limit %Fc %d",&GTS[0],&tempi);
		SetDlgItemInt(hDlgChild,IDD_VSETP,tempi,TRUE);
		if (GTS[0]=='<') 
			SetDlgItemText(hDlgChild,0+IDD_PVGT,"<");
		else 
			SetDlgItemText(hDlgChild,0+IDD_PVGT,">");
	}
	else if (_fstrstr(buffer,"-V Limit")==buffer)
	{
		sscanf(lbuff,"-V Limit %Fc %d",&GTS[1],&tempi);
		SetDlgItemInt(hDlgChild,IDD_VSETM,tempi,TRUE);
		if (GTS[1]=='<') 
			SetDlgItemText(hDlgChild,1+IDD_PVGT,"<");
		else 
			SetDlgItemText(hDlgChild,1+IDD_PVGT,">");
	}
	else if (_fstrstr(buffer,"+I Limit")==buffer)
	{
		sscanf(lbuff,"+I Limit %Fc %d",&GTS[2],&tempi);
		SetDlgItemInt(hDlgChild,IDD_ISETP,tempi,TRUE);
		if (GTS[2]=='<') 
			SetDlgItemText(hDlgChild,2+IDD_PVGT,"<");
		else 
			SetDlgItemText(hDlgChild,2+IDD_PVGT,">");
	}
	else if (_fstrstr(buffer,"-I Limit")==buffer)
	{
		sscanf(lbuff,"-I Limit %Fc %d",&GTS[3],&tempi);
		SetDlgItemInt(hDlgChild,IDD_ISETM,tempi,TRUE);
		if (GTS[3]=='<') 
			SetDlgItemText(hDlgChild,3+IDD_PVGT,"<");
		else 
			SetDlgItemText(hDlgChild,3+IDD_PVGT,">");
	}    
	else if (_fstrstr(buffer,"At:")==buffer)
	{
		if (SendDlgItemMessage(hDlgChild,IDD_MEAS_BAY,CB_SELECTSTRING,-1,(long)(LPSTR)buffer+3)==CB_ERR)
			Message("Can't Set Measurments","Can't find the Module:\n%Fs",buffer+3);
		else
			SendMessage(hDlgChild,WM_COMMAND,MAKEWPARAM(IDD_MEAS_BAY,CBN_SELCHANGE),(LPARAM) GetDlgItem(hDlgChild,IDD_MEAS_BAY));
		return 0;
	}
	else if (_fstrstr(buffer,"V At:")==buffer)
	{
		if (SendDlgItemMessage(hDlgChild,IDD_VSETCB,CB_SELECTSTRING,-1,(long)(buffer+5))==CB_ERR)
			Message("Can't Set Measurments","Can't find the V Measurment:\n%Fs",buffer+5);
		else
			SendMessage(hDlgChild,WM_COMMAND,MAKEWPARAM(IDD_VSETCB,CBN_SELCHANGE),(LPARAM) GetDlgItem(hDlgChild,IDD_VSETCB));
		return 0;
	}
	else if (_fstrstr(buffer,"I At:")==buffer)
	{
		if (SendDlgItemMessage(hDlgChild,IDD_ISETCB,CB_SELECTSTRING,-1,(long)(buffer+5))==CB_ERR)
			Message("Can't Set Measurments","Can't find the I Measurment:\n%Fs",buffer+5);
		else
			SendMessage(hDlgChild,WM_COMMAND,MAKEWPARAM(IDD_ISETCB,CBN_SELCHANGE),(LPARAM) GetDlgItem(hDlgChild,IDD_ISETCB));
		return 0;
	}
	else
	{
		Message("Unknow String to Parse!","Unknown:\n%Fs",buffer);
		return 1;
	}
	return 0;
}

/* JLR, 10/17/95 Allow auto measurements for surge, if:

			- single phase AC coupler selected  or
			- measuring 5 channels and 
			  IO coupler or three phase AC coupler selected
*/

BOOL MEAS::IsAuto(int bay, int lines)					
{
	if (bay < 0 || bay > 15) return FALSE; 
	if ((calset[bay].type & TYPE_CPLSRG) == 0) return FALSE; 
	if (lines == 5)
	{
		if ((calset[bay].type & TYPE_CPLIO) ||
			((calset[bay].type & TYPE_CPLAC) && SURGE_THREE_PHASE))
		return TRUE;
	}
	else
	{
		if ((calset[bay].type & TYPE_CPLAC) && !SURGE_THREE_PHASE)
			return TRUE;
	}
	return FALSE;
}

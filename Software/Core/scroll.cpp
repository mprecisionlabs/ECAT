#include "stdafx.h"
#include "scroll.h"
#include "string.h"
#include "message.h"

//**************************************************************************//
//                             Scroll_Pair::init()                          //
//                                                                          //
// void init(HWND hDlg, //The dialog handle                                 //
//           WORD id_sb,//The ID of the Scroll Bar                          //
//           WORD id_eb,//The ID of the Edit Box                            //
//           WORD start,//The start of the Scroll Pair  range.              //
//           WORD end,  //The end of the Scroll Pair  range.                //
//           WORD initv,//The initial value for the Scroll Pair.            //
//            int exp,  //Base 10 exponent used to map Scroll Bar units     //
//                      //to Values 0 =1 to 1, -1 Means 1 SB=10 EB units    //
//            int inc,  //How many Scroll Bar units to scroll for each press//
//           long color //If the Scroll bar class is SSCROLLBAR the it sets //
//                      //The color of the hash mark on the scroll bar      //
//                      //If the Class is not SSCROLLBAR then it is ignored.//
//          );                                                              //
//**************************************************************************//

void Scroll_Pair::init(HWND ihDlg,LONG iid_sb, LONG iid_eb, LONG istart, LONG iend,LONG iinit,int iexponent,int iinc,LONG Color,LPSTR unit_txt)
{
	LONG st;
	LONG en;
	char Buffer[20];

	_fstrncpy(units,unit_txt,4);
	if (unit_txt[1]==0)
	{ 
		units[1]=' '; 
		units[2]=0;
	}

	//Set up internal variables
	id_sb=iid_sb;
	id_eb=iid_eb;
	div=1;
	mul=1;
	start=istart;
	end=iend;
	inc=iinc;
	hDlg=ihDlg;
	On=TRUE;

	//Calculate the proper Div and Mul values from the exponent 
	while (iexponent<0) {iexponent++; div*=10; }
	while (iexponent>0) {iexponent--; mul*=10; }

	//Set up and initialize the scroll Bar
	st=start*mul/div;
	en=end*mul/div;
	variable=iinit;

	SetScrollRange(GetDlgItem( hDlg,id_sb), SB_CTL,st,en, TRUE );
	SetScrollPos( GetDlgItem( hDlg, id_sb), SB_CTL,variable*mul/div, TRUE );

	//Setup the Edit box
	Suspend=TRUE;
	PrintfDlg(hDlg,id_eb,"%ld %s",variable,units);
	Suspend=FALSE;

	//Create the Pen
	if (pen) 
		DeleteObject(pen);
	
	pen=CreatePen(PS_SOLID,2,Color);

	//Check the ScrollBar to see if it is an SSCROLLBAR 
	//And if so assign the pen
	GetClassName(GetDlgItem(ihDlg,id_sb),(LPSTR)Buffer,12);
	if (strncmp(Buffer,"SSCROLLBAR",12)==0)
	{
		LONG top =GetClassLong(GetDlgItem(hDlg,id_sb),GCL_CBWNDEXTRA);
		SetWindowLong(GetDlgItem(hDlg,id_sb),top-2,(long)pen);
	}
	else 
	{
		DeleteObject(pen);
		pen=NULL;
	}
}

void Scroll_Pair::Set_Units(LPSTR new_units)
{
	_fstrncpy(units,new_units,4);
	if (new_units[1]==0)
	{ 
		units[1]=' '; 
		units[2]=0;
	}
	PrintfDlg(hDlg,id_eb,"%ld %s",variable,units);
}

BOOL Scroll_Pair::Scroll_Proc(UINT message,WPARAM wParam, LPARAM lParam)
{
	char gStr0[100];
	LONG SB_pos,SB_inc;
	int i;

	if (message==WM_HSCROLL)
	{
		ichanged=TRUE;
		if(GetWindowLong ((HWND)lParam, GWL_ID) != id_sb)
		{
			MessageBox(NULL,"Internal Scroll Pair id Error","Error in Scroll ID",MB_OK);
			return TRUE;
		}
		SB_pos=GetScrollPos(GetDlgItem( hDlg,id_sb), SB_CTL);
		SB_inc=0;

		switch (LOWORD(wParam))
		{
		case SB_PAGEDOWN:
			SB_inc+=10;
			break;

		case SB_LINEDOWN:
			SB_inc=+1;
			break;

		case SB_PAGEUP:
			SB_inc-=10;
			break;

		case SB_LINEUP:
			SB_inc=-1;
			break;

		case SB_THUMBTRACK:
		case SB_THUMBPOSITION:
			SB_pos= HIWORD(wParam);
			break;
		}

		SB_pos=SB_pos+(inc*SB_inc);
		
		if (SB_pos<(start*mul/div)) 
			SB_pos=start*mul/div;
		if (SB_pos>(end*mul/div )) 
			SB_pos=end*mul/div;

		variable=SB_pos*div/mul;
		SetScrollPos( GetDlgItem( hDlg,id_sb), SB_CTL,SB_pos, TRUE );

		Suspend=TRUE;
		PrintfDlg(hDlg,id_eb,"%ld %s",variable,units);
		Suspend=FALSE;
		if(Notify_Message) 
			PostMessage(hDlg,Notify_Message,0,MAKELONG(variable,0));
	}
	else if (message==WM_COMMAND)
	{
		ichanged=TRUE;
		if (LOWORD(wParam) != id_eb)
		{
			Message("Error ie Edit ID", "Internal Scroll Pair id Error");
			return TRUE;
		}

		SB_pos = GetScrollPos(GetDlgItem( hDlg, id_sb), SB_CTL);
		SB_inc = 0;

		// See what type of notification has come from the edit control
		switch (HIWORD(wParam))
		{
		case EN_KILLFOCUS:
			// Update units
			Suspend = TRUE;
			PrintfDlg(hDlg, id_eb, "%ld %s", variable, units);
			Suspend = FALSE;
			break;

		case EN_CHANGE:
			// If not suspended, i.e. we are updating the control, not the user, 
			// make sure the value is OK
			if (!Suspend)
			{
				GetDlgItemText(hDlg, id_eb, (LPSTR) gStr0, 100);
				if (sscanf(gStr0, "%d", &i)) 
					variable = i; 
				else 
					variable = start;
				
				if (variable < start)
					variable = start;
				
				if (variable > end) 
					variable = end;
				
				SB_pos = variable * mul / div;
				SetScrollPos(GetDlgItem(hDlg, id_sb), SB_CTL, SB_pos, TRUE);
				
				if (Notify_Message) 
					PostMessage(hDlg, Notify_Message, 0, MAKELONG(variable, 0));
			}
			break;
		}
	}

	return TRUE;
}



//Hides or Unhides the scroll Pair
void Scroll_Pair::Hide(BOOL Hideit)
{
	LONG st;
	LONG en;
	if (!Hideit)
	{
		ShowWindow( GetDlgItem( hDlg,id_sb ), SW_SHOWNORMAL );
		ShowWindow( GetDlgItem( hDlg,id_eb), SW_SHOWNORMAL );
		st=start*mul/div;
		en=end*mul/div;
		SetScrollRange(GetDlgItem( hDlg,id_sb), SB_CTL,st,en, TRUE );
		st=variable*mul/div;
		SetScrollPos( GetDlgItem( hDlg, id_sb), SB_CTL,st, TRUE );
		Suspend=TRUE;
		PrintfDlg(hDlg,id_eb,"%ld %s",variable,units);
		Suspend=FALSE;
		InvalidateRect(GetDlgItem(hDlg,id_sb), NULL, FALSE);
		On=TRUE;
	}
	else
	{	
		ShowWindow( GetDlgItem( hDlg,id_sb ), SW_HIDE );
		ShowWindow( GetDlgItem( hDlg,id_eb), SW_HIDE );
		On=FALSE;
	}
}

void Scroll_Pair::Set_Value(LONG val)
{
	variable=val;
	Hide(!On);
}

void Scroll_Pair::Set_Max_Value(LONG val)
{
	if (variable>val) variable=val;
	end=val;
	Hide(!On);
}

void Scroll_Pair::Set_Min_Value(LONG val)
{
	if (variable<val) variable=val;
	start=val;
	Hide(!On);
}

Scroll_Pair::~Scroll_Pair()
{
	DeleteObject(pen);
}

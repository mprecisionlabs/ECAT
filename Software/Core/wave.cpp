/******************************************************************************

	WAVE.CPP -- Waveforms and associated surge module information

	History:		Start with 4.0A version.

		5/31/95		Add special case delay for E521 like E522.	JFL
		6/15/95		Test for erroneous 0 wavecount return value
							 in fill_waves.  JFL
      11/20-21/95		Add allow_front changes in scan_list.  JFL/JLR
******************************************************************************/
#include "stdafx.h"
#include <ctype.h>
#include "WAVES.h"
#include "kt_comm.h"
#include "comm.h"
#include "simwaves.h"
#include "globals.h"
#include "module.h"
#include "message.h"

void WAVE::Init(HANDLE hInstance, HWND hwnd, RECT& rect, HICON hicon,LPSTR text)
{
	running=0;
	icon=hicon;
	Update_Values=FALSE;
	Update_Window=NULL;
	Update_Message=0;
	Last_Active_Wave=-99;

	VlimitAC = VlimitIO = 29999;
	TlimitAC = TlimitIO = 4;

	allow_front  = TRUE;
	allow_couple = TYPE_CPLAC;
	five_chan_io = TRUE;

	create(hInstance,hwnd,rect,MAKEINTRESOURCE(IDD_WAVE));
	SetDlgItemText(hDlgChild,IDD_WIN_TEXT,text);
	SetWindowLong(GetDlgItem(hDlgChild,IDD_ICON),4,(long)hicon);
	ShowWindow(hDlgChild,SW_SHOWNORMAL);
	strcpy(Obj_Name,text);
	created=TRUE;
}

int WAVE::MaxVoltage(long cplmode)
{
	if (cplmode == TYPE_CPLIO) 
		return VlimitIO;
	else if (cplmode == TYPE_CPLAC || cplmode == 0) 
		return VlimitAC;
	else
		return VlimitAC <= VlimitIO ? VlimitAC : VlimitIO;
}

int WAVE::MinTime(long cplmode)   
{
	if (cplmode == TYPE_CPLIO) 
		return TlimitIO;
	else if (cplmode == TYPE_CPLAC || cplmode == 0)
		return TlimitAC;
	else
		return TlimitAC >= TlimitIO ? TlimitAC : TlimitIO;
}

int _fatoi(LPSTR cv)
{
	LPSTR cp;
	int s;
	int v;
	s=0;
	v=0;
	cp=cv;
	while ((*cp) && (isspace(*cp))) 
		cp++;
	if ((*cp) && (*cp=='+')) 
		cp++;
	if ((*cp) && (*cp=='-')) 
	{
		s=1; 
		cp++;
	}
	
	while ((*cp) && (isdigit(*cp)))
	{
		v*=10;
		v+=((*cp)-'0');
		cp++;
	}
	
	return (s) ? -v : v;
}

BOOL parse_wave(const LPSTR wavetext, WAVEINFO &wi)
{
	istrstream is(wavetext);
	int hvcpl,hvmaxv,hvmint;
	int cnt;

	is >> cnt;
	is >> wi.front;

	is >> wi.line_couple >> hvcpl >> wi.io_couple;
	is >> wi.maxv  >> hvmaxv >> wi.maxv_io;
	is >> wi.mint  >> hvmint >> wi.mint_io;

	if (hvcpl)
	{
		wi.line_couple = 1;
		wi.maxv = hvmaxv;
		wi.mint = hvmint;
	}
	if (!is.good()) return FALSE;

	char *str = &wavetext[is.tellg()];
	str = strchr(str,',');
	if (str) str++;

	if (*str == 0) return FALSE;

	strncpy(wi.wave_name,str,WAVENAMESIZE-1);
	return TRUE;
}


void fill_waves(int bay)
{
	int wav   = 0;
	int waven = 0;

	while (wave_list[wav].mod_num!=-1) wav++;

	if (SIMULATION)
	for (int wavs=0; sim_waves[wavs][0]; wavs++)
	{
		char str[MODNAMESIZE];
		str[MODNAMESIZE-1] = 0;

		strncpy(str,sim_waves[wavs],MODNAMESIZE-1);
		char *c = strchr(str,','); if (c) *c = 0;
		c = strchr(str,' '); if (c) *c = 0;

		if (strcmp(calset[bay].name,str) == 0)
		{
			wave_list[wav].mod_num=bay;
			wave_list[wav].waven=waven++;

			const char *c = strchr(sim_waves[wavs],',');
			if (!parse_wave((LPSTR)(++c),wave_list[wav]))
			{
				wave_list[wav].mod_num=-1;
				return;
			}
			wave_list[++wav].mod_num=-1;
		}
	}
	else										//Add Real Stuff Here!
	{ 
		char rxbuffer[80];
		int err;

		int wcnt=QueryPrintfEcatInt(err,"BAY:WAVE? %d 0",bay);
		if (err || wcnt == 0)
		{
			MessageBox(NULL,"Error Reading Waveform Information","Error",MB_OK);
			return;
		}
		for (waven=0; waven<wcnt; waven++)
		{
			QueryPrintfEcat(rxbuffer,80,"BAY:WAVE? %d %d",bay,waven+1);
			wave_list[wav].mod_num=bay;
			if (!parse_wave(rxbuffer,wave_list[wav]))
			{
				wave_list[wav].mod_num=-1;
				return;
			}

			wave_list[wav].waven=waven;

			if (calset[bay].id == E522_BOXT || calset[bay].id == E521_BOXT)
				wave_list[wav].mint = 40;

			if (calset[bay].id == E510A_BOXT)
				wave_list[wav].mint = 10;

			wave_list[++wav].mod_num=-1;
		}
	} 
}

LONG WAVE::ReportSteps(void)
{
	if (mode) 
		return (SendDlgItemMessage(hDlgChild,IDD_LIST,LB_GETCOUNT,0,0L))-2;
	else 
		return 1;
}

void WAVE::Setup_Plugin(HWND hDlg,int IDD_WAVEU,int cur_plugin)
{
	int i=0;
	int cp;

	SendDlgItemMessage(hDlg,IDD_WAVEU,CB_RESETCONTENT,0,0L);
	while ((wave_list[i].mod_num!=-1) && (wave_list[i].mod_num!=cur_plugin)) i++;

	while (wave_list[i].mod_num==cur_plugin)
	{ 
		cp=SendDlgItemMessage(hDlg,IDD_WAVEU,CB_ADDSTRING,0,(LONG)(LPSTR)wave_list[i].wave_name);
		SendDlgItemMessage(hDlg,IDD_WAVEU,CB_SETITEMDATA,cp,(LONG)i);
		i++;
	}
	SendDlgItemMessage(hDlg,IDD_WAVEU,CB_SETCURSEL,0,0L);
	SendMessage(hDlg,WM_COMMAND,MAKELONG(IDD_WAVEU,CBN_SELCHANGE),(LPARAM) GetDlgItem(hDlg,IDD_WAVEU));
}

inline BOOL IsE508(int waven)		//JLR
{
	return (calset[wave_list[waven].mod_num].id == E508_BOXT);
}


void WAVE::Scan_list()
{
	long i = 0;
	int  j = 0;

	VlimitAC = VlimitIO = 29999;
	TlimitAC = TlimitIO = 4;

	if (mode)
	{
		allow_front  = TRUE;
		five_chan_io = TRUE;
		allow_couple = TYPE_CPLAC | TYPE_CPLIO;

		j=SendDlgItemMessage(hDlgChild,IDD_LIST,LB_GETCOUNT,0,0L);

		if (j>2) 
		{
			for (j=j-1; j>1; j--)
			{
				i=(SendDlgItemMessage(hDlgChild,IDD_LIST,LB_GETITEMDATA,j-1,0L));

				if (wave_list[i].maxv<VlimitAC) VlimitAC=wave_list[i].maxv;
				if (wave_list[i].mint>TlimitAC) TlimitAC=wave_list[i].mint;

				if (wave_list[i].io_couple)
				{
					if (wave_list[i].maxv_io<VlimitIO) VlimitIO=wave_list[i].maxv_io;
					if (wave_list[i].mint_io>TlimitIO) TlimitIO=wave_list[i].mint_io;
				}
				if (!wave_list[i].line_couple) allow_couple &= ~TYPE_CPLAC;
				if (!wave_list[i].io_couple)   allow_couple &= ~TYPE_CPLIO;

				if (!wave_list[i].front)       allow_front  = FALSE;
				if (IsE508(i)) five_chan_io = FALSE;	  //JLR
			}
		}            
		else
		{
			// Give it some sensible value, instead of 29999
			VlimitAC = wave_list[cur_wave].maxv;
			TlimitAC = wave_list[cur_wave].mint;
		}
	}
	else
	{
		VlimitAC = wave_list[cur_wave].maxv;
		TlimitAC = wave_list[cur_wave].mint;

		if (wave_list[cur_wave].io_couple)
		{
			VlimitIO = wave_list[cur_wave].maxv_io;
			TlimitIO = wave_list[cur_wave].mint_io;
		}
		allow_couple = 0;
		if (wave_list[cur_wave].line_couple) allow_couple |= TYPE_CPLAC;
		if (wave_list[cur_wave].io_couple)	  allow_couple |= TYPE_CPLIO;

		allow_front  = wave_list[cur_wave].front;	  //JLR
		five_chan_io = !IsE508(cur_wave);				  //JLR
	}
}


#define DIS  (*((LPDRAWITEMSTRUCT)(lParam)))
#define MIS  (*((LPMEASUREITEMSTRUCT)(lParam)))

BOOL WAVE::DlgProc (HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	/*1*/
	char buff[20];
	char buff2[80];
	int i;
	LONG cp;
	LONG lv;
	int n;
	char Insert_Str[128];

	switch (message)
	{/*2Main Message Switch */
	case WM_INITDIALOG:
	{
		TEXTMETRIC tm;
		HDC hdc=GetDC(hDlg);
		GetTextMetrics(hdc,&tm);
		tmH=tm.tmHeight;
		ReleaseDC(hDlg,hdc);

		SendDlgItemMessage(hDlg,IDD_LIST,LB_RESETCONTENT,0,0L);
		SendDlgItemMessage(hDlg,IDD_LIST,LB_ADDSTRING,0,(LONG)((LPSTR)"START"));
		SendDlgItemMessage(hDlg,IDD_LIST,LB_ADDSTRING,0,(LONG)((LPSTR)"END"));
		SendDlgItemMessage(hDlg,IDD_LIST,LB_SETCURSEL,0,0L);
		wave_list[0].mod_num=-1;
		for (i=0; i<MAX_CAL_SETS; i++)
			if (NOT_GHOST(i) && (calset[i].type & TYPE_SRG))
			{
				fill_waves(i);
				cp=SendDlgItemMessage(hDlg,IDD_PLUGIN,CB_ADDSTRING,0,(LONG)((LPSTR)calset[i].name));
				SendDlgItemMessage(hDlg,IDD_PLUGIN,CB_SETITEMDATA,cp,(LONG)i);
			}

		SendDlgItemMessage(hDlg,IDD_PLUGIN,CB_SETCURSEL,0,0L);
		SendMessage(hDlg,WM_COMMAND,MAKELONG(IDD_PLUGIN,CBN_SELCHANGE),(LPARAM) GetDlgItem(hDlg,IDD_PLUGIN));
		cur_wave=0;
		cur_plugin=SendDlgItemMessage(hDlg,IDD_PLUGIN,CB_GETITEMDATA,0,0L);
		Setup_Plugin(hDlg,IDD_WAVE,cur_plugin);

		SendDlgItemMessage(hDlg,IDD_CB,CB_RESETCONTENT,0,0L);
		SendDlgItemMessage(hDlg,IDD_CB,CB_ADDSTRING,0,(LONG)((LPSTR)"Fixed"));
		SendDlgItemMessage(hDlg,IDD_CB,CB_ADDSTRING,0,(LONG)((LPSTR)"List"));
		SendDlgItemMessage(hDlg,IDD_CB,CB_ADDSTRING,0,(LONG)((LPSTR)"New List"));
		SendDlgItemMessage(hDlg,IDD_CB,CB_SETCURSEL,0,0L);
		SendMessage(hDlg,WM_COMMAND,MAKELONG(IDD_CB,CBN_SELCHANGE),(LPARAM) GetDlgItem(hDlg,IDD_CB));

		ShowWindow(GetDlgItem(hDlg,IDD_INSERT),SW_HIDE);
		ShowWindow(GetDlgItem(hDlg,IDD_DELETE),SW_HIDE);
		ShowWindow(GetDlgItem(hDlg,IDD_LIST),SW_HIDE);
		ShowWindow(GetDlgItem(hDlg,IDD_MODE_LAB),SW_HIDE);
		ShowWindow(GetDlgItem(hDlgChild,IDD_CB),SW_SHOWNORMAL);
		ichanged=TRUE;
		Scan_list();
		SendDlgItemMessage(hDlg, IDD_ICON, STM_SETICON, (WPARAM) icon, 0);
		return FALSE ;
	}

	case WM_DRAWITEM:
		SendDlgItemMessage(hDlg,IDD_LIST,LB_GETTEXT,DIS.itemID,(LONG)((LPSTR)buff2));
		i=strlen(buff);
		POINT ptOld;
		MoveToEx(DIS.hDC,DIS.rcItem.left,DIS.rcItem.top+1,&ptOld);
		SetTextAlign(DIS.hDC,TA_UPDATECP);
		if (_stricmp(buff2,"START")== 0) 
		{
			TextOut(DIS.hDC,0,0,"Start of List",13);
		}
		else if (_stricmp(buff2,"END")==0)
		{
			TextOut(DIS.hDC,0,0,"End of List",11);
		}
		else
		{
			_itoa(DIS.itemID,buff,10);
			for (i=0; buff[i]; i++);
			while (i<5)  buff[i++]=' ';
			buff[i]=0;

			SelectObject(DIS.hDC, GetStockObject(HOLLOW_BRUSH));
			TextOut(DIS.hDC,0,0,(LPSTR)buff, 5);
			TextOut(DIS.hDC,0,0,(LPSTR)buff2,strlen(buff2));
			if ((DIS.itemState &ODS_SELECTED) && show)
			{
				SetBkColor(DIS.hDC,RGB(255,255,0));
				TextOut(DIS.hDC,0,0,"<---",4);
				SetBkColor(DIS.hDC,GetSysColor(COLOR_WINDOW));
			}
			else
			{	
				SetBkColor(DIS.hDC,GetSysColor(COLOR_WINDOW));
				TextOut(DIS.hDC,0,0,"       ",7);
			}
		}
		return TRUE;

	case WM_MEASUREITEM: if (!tmH)
	{ 
		TEXTMETRIC tm;
		HDC hdc=GetDC(hDlg);
		GetTextMetrics(hdc,&tm);
		tmH=tm.tmHeight;
		ReleaseDC(hDlg,hdc);
		MIS.itemHeight=tmH+2; 
		return TRUE;
	}

	case WM_COMMAND:
		/*WM_COMMAND SWITCH */
		switch (LOWORD(wParam))
		{
		case IDD_INSERT:
			cp=SendDlgItemMessage(hDlg,IDD_PLUGIN,CB_GETCURSEL,0,0L);
			n=SendDlgItemMessage(hDlg,IDD_PLUGIN,CB_GETLBTEXT,cp,(LONG)((LPSTR)Insert_Str));
			Insert_Str[n]=' ';
			n++;
			Insert_Str[n]=0;
			cp=SendDlgItemMessage(hDlg,IDD_WAVE,CB_GETCURSEL,0,0L);
			n+=SendDlgItemMessage(hDlg,IDD_WAVE,CB_GETLBTEXT,cp,(LONG)((LPSTR)(&Insert_Str[n])));
			lv=cur_wave;
			Insert_Str[n]=' ';
			n++;
			Insert_Str[n]=0;
			cp=SendDlgItemMessage(hDlg,IDD_LIST,LB_GETTOPINDEX,0,0L);
			SendDlgItemMessage(hDlg,IDD_LIST,LB_GETTEXT,cp+1,(LONG)(LPSTR)buff2);
			if (!_fstrstr(buff2,"END"))  cp++;
			SendDlgItemMessage(hDlg,IDD_LIST,LB_INSERTSTRING,cp+1,(LONG)((LPSTR)(Insert_Str)));
			SendDlgItemMessage(hDlg,IDD_LIST,LB_SETITEMDATA,cp+1,lv);
			SendDlgItemMessage(hDlg,IDD_LIST,LB_SETTOPINDEX,cp+1,0L);
			Scan_list();
			ichanged=TRUE;
			return TRUE;

		case IDD_DELETE:
			cp=SendDlgItemMessage(hDlg,IDD_LIST,LB_GETTOPINDEX,0,0L);
			SendDlgItemMessage(hDlg,IDD_LIST,LB_GETTEXT,cp+1,(LONG)(LPSTR)buff2);
			if (_stricmp(buff2,"END")) 
			{
				SendDlgItemMessage(hDlg,IDD_LIST,LB_DELETESTRING,cp+1,0L);
				Scan_list();
			}
			SendDlgItemMessage(hDlg,IDD_LIST,LB_SETTOPINDEX,cp,0L);
			cp=SendDlgItemMessage(hDlg,IDD_LIST,LB_GETCOUNT,0,0L);
			ichanged=TRUE;
			return TRUE;

		case IDD_CB:
			ichanged=TRUE;

			/*4IDD CB Switch */
			switch (HIWORD(wParam))
			{ 
			case CBN_SELCHANGE:
				mode=SendDlgItemMessage(hDlg,IDD_CB,CB_GETCURSEL,0,0L);
				if (mode==0)
				{
					/*5Fixed Mode */
					ShowWindow(GetDlgItem(hDlg,IDD_INSERT),SW_HIDE);
					ShowWindow(GetDlgItem(hDlg,IDD_DELETE),SW_HIDE);
					ShowWindow(GetDlgItem(hDlg,IDD_LIST),SW_HIDE);
					cp=SendDlgItemMessage(hDlg,IDD_WAVE,CB_GETCURSEL,0,0L);
					cur_wave=SendDlgItemMessage(hDlg,IDD_WAVE,CB_GETITEMDATA,cp,0L);
					Scan_list();
				}/*5Fixed Mode */
				else if (mode==1)
				{
					/*5List Mode */
					ShowWindow(GetDlgItem(hDlg,IDD_INSERT),SW_SHOWNORMAL);
					ShowWindow(GetDlgItem(hDlg,IDD_DELETE),SW_SHOWNORMAL);
					ShowWindow(GetDlgItem(hDlg,IDD_LIST),SW_SHOWNORMAL);
					Scan_list();
				}/*5List Mode */
				else
				{
					/*5New list mode */
					ShowWindow(GetDlgItem(hDlg,IDD_INSERT),SW_SHOWNORMAL);
					ShowWindow(GetDlgItem(hDlg,IDD_DELETE),SW_SHOWNORMAL);
					ShowWindow(GetDlgItem(hDlg,IDD_LIST),SW_SHOWNORMAL);
					SendDlgItemMessage(hDlg,IDD_LIST,LB_RESETCONTENT,0,0L);
					SendDlgItemMessage(hDlg,IDD_LIST,LB_ADDSTRING,0,(LONG)((LPSTR)"START"));
					SendDlgItemMessage(hDlg,IDD_LIST,LB_ADDSTRING,0,(LONG)((LPSTR)"END"));
					SendDlgItemMessage(hDlg,IDD_LIST,LB_SETCURSEL,0,0L);
					SendDlgItemMessage(hDlg,IDD_CB,CB_SETCURSEL,1,0L);
					SendMessage(hDlg,WM_COMMAND,MAKELONG(IDD_CB,CBN_SELCHANGE),(LPARAM) GetDlgItem(hDlg,IDD_CB));
					mode=1;
					Scan_list();
				}/*5New list Mode */
				break;

			case CBN_SETFOCUS:
			case CBN_KILLFOCUS:
			case CBN_DROPDOWN:
			case CBN_DBLCLK:
				return TRUE;
			}/*End of IDD_CB Switch */
			return TRUE;

		case IDD_PLUGIN:
			switch (HIWORD(wParam))
			{/*IDD PLUGIN SWITCH */
			case CBN_SELCHANGE:
				// Update the IDD_LIST if necessary
				mode=SendDlgItemMessage(hDlg,IDD_CB,CB_GETCURSEL,0,0L);
				if (mode == 1)
				{
					ShowWindow(GetDlgItem(hDlg,IDD_INSERT),SW_SHOWNORMAL);
					ShowWindow(GetDlgItem(hDlg,IDD_DELETE),SW_SHOWNORMAL);
					ShowWindow(GetDlgItem(hDlg,IDD_LIST),SW_SHOWNORMAL);
					SendDlgItemMessage(hDlg,IDD_LIST,LB_RESETCONTENT,0,0L);
					SendDlgItemMessage(hDlg,IDD_LIST,LB_ADDSTRING,0,(LONG)((LPSTR)"START"));
					SendDlgItemMessage(hDlg,IDD_LIST,LB_ADDSTRING,0,(LONG)((LPSTR)"END"));
					SendDlgItemMessage(hDlg,IDD_LIST,LB_SETCURSEL,0,0L);
					SendDlgItemMessage(hDlg,IDD_CB,CB_SETCURSEL,1,0L);
				}

				ichanged=TRUE;
				cp=SendDlgItemMessage(hDlg,IDD_PLUGIN,CB_GETCURSEL,0,0L);
				cur_plugin=SendDlgItemMessage(hDlg,IDD_PLUGIN,CB_GETITEMDATA,cp,0L);
				Setup_Plugin(hDlg,IDD_WAVE,cur_plugin);
				cp=SendDlgItemMessage(hDlg,IDD_WAVE,CB_GETCURSEL,0,0L);
				cur_wave=SendDlgItemMessage(hDlg,IDD_WAVE,CB_GETITEMDATA,cp,0L);
				Scan_list();
				break;
			}/*End of IDD_PLUGIN Switch */
			return TRUE;

		case IDD_WAVE:
			ichanged=TRUE;
			switch (HIWORD(wParam))
			{/*4IDD WAVE SWITCH */
			case CBN_SELCHANGE:
				cp=SendDlgItemMessage(hDlg,IDD_WAVE,CB_GETCURSEL,0,0L);
				cur_wave=SendDlgItemMessage(hDlg,IDD_WAVE,CB_GETITEMDATA,cp,0L);
				Scan_list();
				break;
			}/*4IDD_WAVE Switch */
		}/*3End WM COMMAND SWITCH */
	}/*2End Main Message case */
	return FALSE;

}/*1*/

 
void WAVE::Set_Start_Values()
{ 
	if (mode)
	{
		Current_wave_Pos=1;
		SendDlgItemMessage(hDlgChild,IDD_LIST,LB_SETCURSEL,Current_wave_Pos,0L);
		SendDlgItemMessage(hDlgChild,IDD_LIST,LB_SETTOPINDEX,Current_wave_Pos-1,0L);
	}
	Last_Active_Wave=-99;
	Update_State();
}

BOOL WAVE::Set_Next_Step()
{
	char buff[80];
	if (mode)
	{
		SendDlgItemMessage(hDlgChild,IDD_LIST,LB_GETTEXT,Current_wave_Pos+1,(LONG)((LPSTR)buff));
		if (_stricmp(buff,"END")==0) return TRUE;
		Current_wave_Pos++;
		SendDlgItemMessage(hDlgChild,IDD_LIST,LB_SETCURSEL,Current_wave_Pos,0L);
		SendDlgItemMessage(hDlgChild,IDD_LIST,LB_SETTOPINDEX,Current_wave_Pos-1,0L);
		Update_State();
		return FALSE;
	}
	else 
	{ 
		Update_State();
		return TRUE;
	}
}


LONG WAVE::Get_Current_Values()
{ 
	return This_Active_Wave;
}

void WAVE::Update_State()
{
	long lv,cp;
	if (mode)
	{
		lv=SendDlgItemMessage(hDlgChild,IDD_LIST,LB_GETITEMDATA,Current_wave_Pos,0L);
		Setup_Plugin(hDlgChild,IDD_WAVE,wave_list[lv].mod_num);
		SendDlgItemMessage(hDlgChild,IDD_WAVE,CB_SETCURSEL,wave_list[lv].waven,0L);
		SendMessage(hDlgChild,WM_COMMAND,MAKELONG(IDD_WAVE,CBN_SELCHANGE),(LPARAM) GetDlgItem(hDlgChild,IDD_WAVE));
	}
	else
	{
		cp=SendDlgItemMessage(hDlgChild,IDD_WAVE,CB_GETCURSEL,0,0L);
		lv=SendDlgItemMessage(hDlgChild,IDD_WAVE,CB_GETITEMDATA,cp,0L);
	}
	This_Active_Wave=lv;
	Refresh_Values();
}

void WAVE::Refresh_Values()
{
	long lv;
	lv=This_Active_Wave;
	if (This_Active_Wave!=Last_Active_Wave)
	{
		if (Update_Values)
		{
			SendMessage(Update_Window,KT_SRG_NETWORK_SETSTATE ,(WPARAM) ((WORD)wave_list[lv].mod_num & 0x0F),0l);
			SendMessage(Update_Window,KT_SRG_WAVE_SETSTATE ,(WPARAM) ((WORD)(wave_list[lv].waven) & 0x0ff),0l);
		}
		Last_Active_Wave=This_Active_Wave;
	}   
}

void WAVE::Show_Run(BOOL state)
{
	LONG csel;
	char buff [20];
	show=state;
	running=state;
	InvalidateRect(GetDlgItem(hDlgChild,IDD_LIST),NULL,0);
	if (state)
	{
		ShowWindow(GetDlgItem(hDlgChild,IDD_CB),SW_HIDE);
		ShowWindow(GetDlgItem(hDlgChild,IDD_MODE_LAB),SW_SHOWNORMAL);
		csel=SendDlgItemMessage(hDlgChild,IDD_CB,CB_GETCURSEL,0,0L);
		SendDlgItemMessage(hDlgChild,IDD_CB,CB_GETLBTEXT,(WORD)csel,(LONG)((LPSTR)buff));
		PrintfDlg(hDlgChild,IDD_MODE_LAB,"%s",buff);
	}
	else
	{
		ShowWindow(GetDlgItem(hDlgChild,IDD_CB),SW_SHOWNORMAL);
		ShowWindow(GetDlgItem(hDlgChild,IDD_MODE_LAB),SW_HIDE);
	}
}

int WAVE::Copy_Net_Name(LPSTR copyto, int len)
{
	char buff[255];
	int lv=SendDlgItemMessage(hDlgChild,IDD_PLUGIN,CB_GETCURSEL,0,0L);
	lv=SendDlgItemMessage(hDlgChild,IDD_PLUGIN,CB_GETLBTEXT,lv,(long)buff);
	if (lv>len) buff[len-1]=0;
	strcpy(copyto,buff);
	return lv;
}

void WAVE::Clear(void)
{
	int i;
	LONG cp;
	SendDlgItemMessage(hDlgChild,IDD_LIST,LB_RESETCONTENT,0,0L);
	SendDlgItemMessage(hDlgChild,IDD_LIST,LB_ADDSTRING,0,(LONG)((LPSTR)"START"));
	SendDlgItemMessage(hDlgChild,IDD_LIST,LB_ADDSTRING,0,(LONG)((LPSTR)"END"));
	SendDlgItemMessage(hDlgChild,IDD_LIST,LB_SETCURSEL,0,0L);

	SendDlgItemMessage(hDlgChild,IDD_PLUGIN,CB_RESETCONTENT,0,0L);

	wave_list[0].mod_num=-1;
	for ( i=0; i<MAX_CAL_SETS; i++)
	if ((NOT_GHOST(i)) && (calset[i].type & TYPE_SRG))
	{
		fill_waves(i);
		cp=SendDlgItemMessage(hDlgChild,IDD_PLUGIN,CB_ADDSTRING,0,(LONG)((LPSTR)calset[i].name));
		SendDlgItemMessage(hDlgChild,IDD_PLUGIN,CB_SETITEMDATA,cp,(LONG)i);
	}

	SendDlgItemMessage(hDlgChild,IDD_PLUGIN,CB_SETCURSEL,0,0L);
	SendMessage(hDlgChild,WM_COMMAND,MAKELONG(IDD_PLUGIN,CBN_SELCHANGE),(LPARAM) GetDlgItem(hDlgChild,IDD_PLUGIN));
	cur_wave=0;
	cur_plugin=SendDlgItemMessage(hDlgChild,IDD_PLUGIN,CB_GETITEMDATA,0,0L);
	Setup_Plugin(hDlgChild,IDD_WAVE,cur_plugin);

	SendDlgItemMessage(hDlgChild,IDD_CB,CB_RESETCONTENT,0,0L);
	SendDlgItemMessage(hDlgChild,IDD_CB,CB_ADDSTRING,0,(LONG)((LPSTR)"Fixed"));
	SendDlgItemMessage(hDlgChild,IDD_CB,CB_ADDSTRING,0,(LONG)((LPSTR)"List"));
	SendDlgItemMessage(hDlgChild,IDD_CB,CB_ADDSTRING,0,(LONG)((LPSTR)"New List"));
	SendDlgItemMessage(hDlgChild,IDD_CB,CB_SETCURSEL,0,0L);
	SendMessage(hDlgChild,WM_COMMAND,MAKELONG(IDD_CB,CBN_SELCHANGE),(LPARAM) GetDlgItem(hDlgChild,IDD_CB));

	ShowWindow(GetDlgItem(hDlgChild,IDD_INSERT),SW_HIDE);
	ShowWindow(GetDlgItem(hDlgChild,IDD_DELETE),SW_HIDE);
	ShowWindow(GetDlgItem(hDlgChild,IDD_LIST),SW_HIDE);
	ShowWindow(GetDlgItem(hDlgChild,IDD_MODE_LAB),SW_HIDE);
	ShowWindow(GetDlgItem(hDlgChild,IDD_CB),SW_SHOWNORMAL);
	ichanged=TRUE;
	Scan_list();
}

int WAVE::SaveText(HANDLE hfile)
{
	int i;
	int n;
	long cw;
	switch (mode)
	{
	case 0://Fixed
		fhprintf(hfile,"%Fs:Fixed %Fs:%Fs\r\n",(LPSTR)Obj_Name,(LPSTR)(calset[wave_list[cur_wave].mod_num].name),(LPSTR)(wave_list[cur_wave].wave_name));   
		break;

	case 1:
		fhprintf(hfile,"%Fs:List \r\n",(LPSTR)Obj_Name);
		n=( SendDlgItemMessage(hDlgChild,IDD_LIST,LB_GETCOUNT,0,0L)-2);
		for (i=0; i<n; i++)
		{
			cw=SendDlgItemMessage(hDlgChild,IDD_LIST,LB_GETITEMDATA,i+1,0L);
			fhprintf(hfile,"%Fs:Step:%Fs:%Fs\r\n",(LPSTR)Obj_Name,(LPSTR)(calset[wave_list[cw].mod_num].name),(LPSTR)(wave_list[cw].wave_name));
		}
		break;
	}

	return 1;
}

int WAVE::Process_Line(LPSTR buffer)
{
	int cw;
	if (_fstrstr(buffer,"Fixed")==buffer)
	{
		SendDlgItemMessage(hDlgChild,IDD_CB,CB_SETCURSEL,0,0L);
		SendMessage(hDlgChild,WM_COMMAND,MAKEWPARAM(IDD_CB,CBN_SELCHANGE),(LPARAM) GetDlgItem(hDlgChild,IDD_CB));
		cw=Get_waven(buffer+6);
		if (cw>=0) 
		{
			Set_wave(cw);

			// [aat] - this will update the other duals accordingly, so we won't get these
			// wiered messages about voltage exceeding the limits
			SendMessage(GetParent(hDlgChild), WM_TIMER, 2, NULL);
		}
		else
			Message("Can't Find Wave","Unable to find Waveform:\n%Fs",(LPSTR)buffer+5);
	}
	else if (_fstrstr(buffer,"List")==buffer)
	{ 
		SendDlgItemMessage(hDlgChild,IDD_CB,CB_SETCURSEL,2,0L);//New List
		SendMessage(hDlgChild,WM_COMMAND,MAKEWPARAM(IDD_CB,CBN_SELCHANGE),(LPARAM) GetDlgItem(hDlgChild,IDD_CB));
	}
	else if (_fstrstr(buffer,"Step")==buffer)
	{
		cw=Get_waven(buffer+5);
		if (cw>=0) 
		{
			Set_wave(cw);
			SendMessage(hDlgChild,WM_COMMAND,IDD_INSERT,0L);
		}
		else
			Message("Can't Find Wave","Unable to find Waveform:\n%Fs",(LPSTR)buffer+5);
	}
	else
	{
		Message("Unknow String to Parse!","Unknown:\n%Fs",buffer);
		return 1;
	}

	return 0;
}

void WAVE::Set_wave(int wn)
{
	long cp;
	ichanged=TRUE;
	cp=SendDlgItemMessage(hDlgChild, IDD_PLUGIN, CB_SELECTSTRING, -1, (long)(LPSTR)calset[wave_list[wn].mod_num].name);
	cur_plugin=SendDlgItemMessage(hDlgChild, IDD_PLUGIN, CB_GETITEMDATA, cp, 0L);
	Setup_Plugin(hDlgChild,IDD_WAVE,cur_plugin);
	cp=SendDlgItemMessage(hDlgChild,IDD_WAVE,CB_SELECTSTRING,-1,(long)(LPSTR)wave_list[wn].wave_name);
	cur_wave=SendDlgItemMessage(hDlgChild,IDD_WAVE,CB_GETITEMDATA,cp,0L);
	Scan_list();
}

int WAVE::Get_waven(LPSTR buff)
{
	int i;
	LPSTR colonp;
	LPSTR nn;
	LPSTR wn;
	colonp=_fstrchr(buff,':');
	colonp++;
	for (i=0; i<30; i++)
	{
		nn=calset[wave_list[i].mod_num].name;
		wn=wave_list[i].wave_name;
		if (_fstrstr(buff,nn)==buff) 
		if (_fstrstr(buff,wn)==colonp) 
		return i;
	}
	return -1;
}

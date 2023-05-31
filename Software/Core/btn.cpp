#include "stdafx.h"
#pragma hdrstop
#include "btn.h"
        
BOOL TOGLE_BTN::Create(HWND hp,int res_id,const char **n,int tid)
{
	 if (hp == 0 || res_id == 0 || n == 0) return FALSE;

	 hBtn = GetDlgItem(hp,res_id);
	 if (hBtn == 0) return FALSE;

	 parent   = hp;
	 id       = res_id;
	 id_title = tid;
	 names    = n;

	 while (names[states])
		 states++;
	 if (states == 0) {hBtn = 0; return FALSE;}

	 SendMessage(hBtn,WM_SETTEXT,0,(LONG)(LPSTR (names[0])));
	 return TRUE;
}

BOOL TOGLE_BTN::SetState(unsigned int s)
{
  if (hBtn == 0 || s >= states) return FALSE; 

	state = s;
	SendMessage(hBtn,WM_SETTEXT,0,(LONG)(LPSTR (names[state])));
	return TRUE;
}

BOOL TOGLE_BTN::SetState(const char *s)
{
	if (s == 0) return FALSE;

 	for (int i = 0; i < (int) states; i++)
 	  	if (strcmp(s,names[i]) == 0) return SetState(i);

 	return FALSE;
}

BOOL TOGLE_BTN::GetText(char *s)const
{
	if (s == 0 || hBtn == 0) return FALSE;

	strcpy(s,names[state]);
	return TRUE;
}


unsigned int TOGLE_BTN::NextState()
{
 	if (hBtn)
	{
 	   if (++state >= states) state = 0;
	   SendMessage(hBtn,WM_SETTEXT,0,(LONG)(LPSTR (names[state])));
	}
 	return state;
}

BOOL TOGLE_BTN::Show(BOOL s)
{
 	if (hBtn == 0) return FALSE; 
	
	int show = s ? SW_SHOWNORMAL : SW_HIDE;
  ShowWindow(hBtn,show);
	if (id_title) ShowWindow(GetDlgItem(parent,id_title),show);

	return TRUE;
}

BOOL TOGLE_BTN::Enable(BOOL s)
{
 	if (hBtn == 0) return FALSE; 
	EnableWindow(hBtn,s);
	return TRUE;
}



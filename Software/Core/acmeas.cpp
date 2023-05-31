#include "stdafx.h"
#pragma  hdrstop            
#include "comm.h"
#include "acmeas.h"         
#include "globals.h"
#include "module.h" 
  
#define device 0
const UINT MEAS_BIT[6]    = {1, 2, 4, 8, 16, 32};

/*--------------------------------------------------------------------
 
	ACMEAS.CPP - AC Measurements Dialog

	History: start with 400.r
		10/17/95	JLR	Fixed bug to type in edit fields in AC LIMITS dialog.

----------------------------------------------------------------------*/


// ******************************************************
//                 ACMEAS class functions     
// ******************************************************

ACMEAS::ACMEAS()
{
	hidden=FALSE;
	out_of_limits= 0;
	default_lim  = peak_max = rms_max = AC_MAX;
	rms_min      = 0;

	yBrush  = NULL;
	rBrush  = NULL;
	module  = -1;

	created=FALSE;
}


void ACMEAS::Hide(BOOL b)	
{				   
	AC_OPTION = !b;    // hidden if AC OPTION is not set EUT Mains source module
	if (b==hidden) return;
	if (!b)
		ShowWindow(hDlgChild,SW_SHOWNORMAL);
	else
		ShowWindow(hDlgChild,SW_HIDE);
	hidden=b;
	ichanged=TRUE;
}


void ACMEAS::Init(HANDLE hInstance,HWND hwnd, RECT& rect, LPSTR text,int coupler)
{                   
	yBrush=NULL;
	rBrush=NULL;
	create(hInstance,hwnd,rect,MAKEINTRESOURCE(IDD_AC_MEASURE));

	ShowWindow(hDlgChild,SW_SHOWNORMAL);	
	strcpy(Obj_Name,text);
	created=TRUE;
	module = coupler;		       
	UpdateSettings();   // show or hide, init, display & send limits to ECAT
}

void ACMEAS::Clear(void)       // clear for NEW test
{
	out_of_limits=0;

	peak_max    = AC_MAX;
	rms_max     = default_lim;
	rms_min     = 0;	       // initialize limits

	ShowLimits();		       // display limits
	PrintfLimits();	       // send limits to ECAT

	ichanged = TRUE;
}  

void ACMEAS::ShowLimits()		  // display limits
{
	SetDlgItemInt(hDlgChild,IDD_PLIM,peak_max,FALSE);
	SetDlgItemInt(hDlgChild,IDD_RLIM,rms_max,FALSE);
}

void ACMEAS::UpdateNumbers()      // called after  KT_SYS_STATUS msg to ECAT
{
	if (!hidden) 
		PostMessage(hDlgChild,WM_COMMAND,UPDATE_NUMBERS,0L);
}

void ACMEAS::UpdateEutSource()	  // called when ECAT is back from LOCAL
{
	if (!hidden) 
		PostMessage(hDlgChild,WM_COMMAND,UPDATE_EUT_SOURCE,0L);
}


void ACMEAS::ShowNumbers(int *numbers)     // display peak & RMS data
{
	if (hidden) return;
	float f;

	for (int i=0; i<6;)	     	// pk1,rms1,pk2,rms2,pk3,rms3
	{
		if (numbers[i] >= 0)        // monitoring this line & EUT power is on
		{  
			f = (float) numbers[i] /10;		   // ECAT sends (int)(%.1f * 10.)

			if (f > peak_max)       		   // is peak out of limits?
			{
				out_of_limits |= MEAS_BIT[i];	   //set bit flag & log event
				Log.logprintf("L%d: %.1f > %d, AC Current Peak Limit Exceeded\n",i/2+1,f,peak_max);
			}
			PrintfDlg(hDlgChild,IDD_FSTMEAS+i,"%.1f",f);    // display peak
		}
		else if (!out_of_limits)
			SetDlgItemText(hDlgChild,IDD_FSTMEAS+i,"");  // -1 rcved from ECAT

		i++;			// RMS
		if (numbers[i] >= 0)      	// monitoring this line & EUT power is on
		{
			f = (float) numbers[i] /10;
			if (f > rms_max || f < rms_min)     // is peak out of limits?
			{
				out_of_limits |= MEAS_BIT[i];

				if (f > rms_max)
					Log.logprintf("L%d: %.1f > %d, AC Current RMS Limit Exceeded\n",i/2+1,f,rms_max);
				else
					Log.logprintf("L%d: %.1f < %d, AC Current RMS Limit Exceeded\n",i/2+1,f,rms_min);
			}
			PrintfDlg(hDlgChild,IDD_FSTMEAS+i,"%.1f",f);    // display RMS
		}
		else if (!out_of_limits)
			SetDlgItemText(hDlgChild,IDD_FSTMEAS+i,"");  // display peak
		i++;					       // to peak of next line
	}
}

void ACMEAS::ShowError()		// show error message box
{
	char buff[250];
	ostrstream s(buff,250);

	if (ERR_AC_POWER)
		s << "ECAT EUT AC power is not present";
	else
		for (int i = 0; i < 6; i += 2)
		{
			if ((out_of_limits & MEAS_BIT[i]) && (out_of_limits & MEAS_BIT[i+1]))
				s << "\nL" << i/2 + 1 << " AC Current Peak and RMS Exceeded Limit";
			else if (out_of_limits & MEAS_BIT[i] )
				s << "\nL" << i/2 + 1 << " AC Current Peak Exceeded Limit";
			else if (out_of_limits & MEAS_BIT[i+1])
				s << "\nL" << i/2 + 1 << " AC Current RMS Exceeded Limit";
		}	   		      // at this point buff has error message string

	ERR_AC_LIMIT = FALSE;  // clear error flags now to avoid multiple msg boxes
	ERR_AC_POWER = FALSE;  

	s.put('\0');		  // Borland 3.1 does not do it!!!

	// no clearing out_of_limits to have red peak/RMS edit controls

	MessageBox((HWND)parent,s.str(),"EUT AC Power Error",MB_OK|MB_APPLMODAL);

	out_of_limits = 0;	  // clear  out_of_limits now
}


//  Dialog procedure for AC_Measurements dialog window
BOOL ACMEAS::DlgProc (HWND hDlg, UINT message, UINT wParam, LONG lParam)
{
	switch (message)
	{//Main Message Switch
	case WM_INITDIALOG:
		yBrush=CreateSolidBrush(RGB(255,255,0));   // yellow
		rBrush=CreateSolidBrush(RGB(255,0,0));	   // red
		break;

	case WM_CTLCOLORSTATIC:
	case WM_CTLCOLOREDIT:
		{
			HBRUSH lhBrush;
			POINT point;
			UINT tempw=GetWindowLong((HWND) lParam,GWL_ID);
			int i = tempw - IDD_FSTMEAS;

			if (i >= 0 && i < 6)
			{
				if (out_of_limits & MEAS_BIT[i]) 
				{
					SetBkColor((HDC) wParam,RGB(255,0,0));  // red  backgr.- limit error
					lhBrush=rBrush; 
				}
				else
				{ 
					SetBkColor((HDC) wParam,RGB(255,255,0));// yellow backgr. - no error
					lhBrush=yBrush;
				}

				UnrealizeObject(lhBrush);
				point.x=point.y=0;
				ClientToScreen(hDlg,&point);
				POINT oldPt;
				SetBrushOrgEx((HDC)wParam,point.x,point.y, &oldPt);
				return ((DWORD)lhBrush);
			}
			else 
				return NULL;
		}
		break;

	case WM_COMMAND:                                                     
		switch(LOWORD(wParam))
		{                                                                  
		case IDD_LIMBTN:   //"Limits" button pressed, do dialog to edit limits
			lim_dlg.Execute(ghinst,(HWND)parent,MAKEINTRESOURCE(IDD_AC_LIMIT),default_lim,rms_min,rms_max,peak_max);
			rms_min  = lim_dlg.GetRmsMin(); 
			rms_max  = lim_dlg.GetRmsMax(); 
			peak_max = lim_dlg.GetPeakMax();	// results of limit edit 
			out_of_limits = 0;			// clear for new limits
			ShowLimits();				// display limits
			PrintfLimits();			// send limits to ECAT
			ichanged=TRUE;
			break;

		case UPDATE_NUMBERS:
			ShowNumbers(rms_data);			// display peaks/RMS

			if (ERR_AC_LIMIT | ERR_AC_POWER)	
			ShowError();        	   // if error, show error message box
			break;

			case UPDATE_EUT_SOURCE:		       // if ECAT has been on LOCAL,
			if (hidden) break;
			PrintfECAT(device,"CO %d",module);    // EUT Mains source, or limits
			ECAT_sync();			       // could be changed from 
			PrintfLimits();		       // ECAT front panel
			break;	          // Send EUT Mains source & AC current limits to ECAT
		}
		break;

	case WM_DESTROY	:
		DeleteObject(yBrush);			    
		DeleteObject(rBrush);			    
		break;
	}/*End Main Message case */

	return FALSE;
}

		   
void ACMEAS::UpdateSettings()		// update AC_OPTION flag (in hide())
{					//   and  AC current limits

	if (module >= 0 && module < MAX_CAL_SETS && 
	(calset[module].options & AC_OPT_BIT))  // if EUT power source has AC OPT
	{
		Hide(FALSE);			     // show AC_Measurements dialog
		int d = calset[module].cal[CAL_RMS_LIMIT_OFF];
		if (d <= 0 || d > AC_MAX) d = AC_MAX;   //def.RMS limit stored in cal data
			default_lim = d;
		if (rms_max > default_lim)		     // make present RMS limits
			rms_max  = default_lim;		     //      less than default
		if (rms_min > rms_max)
			rms_min  = 0;
		ShowLimits();			     // display limits
		PrintfLimits();			     // send limits to ECAT
	}
	else
	{				   // if EUT power source does not have AC OPT
		Hide(TRUE);		     		     // hide AC_Measurements dialog
		default_lim  = peak_max = rms_max = AC_MAX;
		rms_min      = 0;
	}

	ichanged = TRUE;
}

void ACMEAS::PrintfLimits() const	     // send AC current limits to ECAT
{
	if (!hidden && !SIMULATION)
	{
		PrintfECAT(device, "AC:PLIM %d",peak_max); ECAT_sync();
		PrintfECAT(device, "AC:RMAX %d",rms_max);  ECAT_sync();
		PrintfECAT(device, "AC:RMIN %d",rms_min);  ECAT_sync();
	}
}


int ACMEAS::SaveText(HANDLE hfile)    // save AC Meas.settings to a file, when
{				   // ECAT test is being saved
	if (!hidden) 
	{
		fhprintf(hfile,"%Fs:At:%-6Fs\r\n",(LPSTR)Obj_Name,calset[module].name);
		fhprintf(hfile,"%Fs:Peak Max:%d RMS Max:%d RMS Min:%d\r\n",
						(LPSTR)Obj_Name,peak_max,rms_max,rms_min);
	}
	return 1;
}



int  ACMEAS::Process_Line(LPSTR buffer)	  // parse AC Meas.settings in buffer,
{					  // saved with SavedText() 
	if (_fstrstr(buffer,"At:"))		  	  /* EUT Mains power src */
	return 0;

	char lbuff[80];
	int pmax,rmax,rmin;
	pmax = rmax = rmin = -1;

	_fstrncpy(lbuff,buffer,80);
	istrstream s((char *)lbuff);

	for (int i = 0; i < 3; i++)
	{  
		char c;
		char name[10] = "         ";

		s >> c; s.putback(c);			  /* skip white space 	*/
		s.read(name, 8);				  

		if (_fstrstr(name,"Peak Max"))		  
			s >> c >> pmax;
		else if (_fstrstr(name,"RMS Max:"))
			s >> rmax;
		else if (_fstrstr(name,"RMS Min:"))   
			s >> rmin;
		else
			return 1;
	}

	if (pmax < 0 || rmax < 0 || rmin < 0)	  /* check, if all restored */ 
		return 1;

	peak_max = pmax;				  /* parsed ok          */
	rms_max  = rmax;
	rms_min  = rmin;				  /* check new limits,	*/
	UpdateSettings();				  /* show & send to ECAT*/

	return 0;
}


// ******************************************************
//                 ACLIM class functions     
// ******************************************************

ACLIM *ACLIM::dlg;

// execute ACLIMDLG - dialog box to edit AC current limits
void ACLIM::Execute(HANDLE hInstance,HWND hwnd,LPSTR name,int dlim,int rmin,int rmax,int pmax)
{
	def_lim  = dlim;	     
	rms_min  = rmin;
	rms_max  = rmax;
	peak_max = pmax;		   // initialize Ac_lim:: variables
	DialogBox((HINSTANCE)hInstance,name,hwnd,(DLGPROC)(ACLIM::AcLimProc));   // call dialog box to edit limits

	// now AC_LIM:: variables: rms_min, rms_max, peak_max have new limits
}


//  Dialog procedure for ACLIMDLG dialog box
BOOL __declspec(dllexport) CALLBACK ACLIM::AcLimProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{ 
	case WM_INITDIALOG:
		// initialize limits scroll pairs	
		dlg->rmin_pair.init(hDlg,IDD_RMIN_SB,IDD_RMIN_EB,0,dlg->def_lim,dlg->rms_min,0,1,RGB(0,255,0),"");
		dlg->rmax_pair.init(hDlg,IDD_RMAX_SB,IDD_RMAX_EB,0,dlg->def_lim,dlg->rms_max,0,1,RGB(0,255,0),"");
		dlg->pmax_pair.init(hDlg,IDD_PMAX_SB,IDD_PMAX_EB,0,AC_MAX,dlg->peak_max,0,1,RGB(0,255,0),"");

		SetWindowText(hDlg,"AC Current Limits");    	// window title
		break;

	case WM_HSCROLL:                                                       
		switch (GetWindowLong (HWND (lParam), GWL_ID))                      
		{                                                                    
		case IDD_RMIN_SB: return(dlg->rmin_pair.Scroll_Proc(message,wParam,lParam));
		case IDD_RMAX_SB: return(dlg->rmax_pair.Scroll_Proc(message,wParam,lParam));
		case IDD_PMAX_SB: return(dlg->pmax_pair.Scroll_Proc(message,wParam,lParam));
		}

	case WM_COMMAND:                                                     
		switch (LOWORD(wParam))                                                     
		{                                                                  
		case IDD_RMIN_EB:			// JLR, 10/17/95
		case IDD_RMIN_SB: return(dlg->rmin_pair.Scroll_Proc(message,wParam,lParam));
		case IDD_RMAX_EB:		   // JLR, 10/17/95
		case IDD_RMAX_SB: return(dlg->rmax_pair.Scroll_Proc(message,wParam,lParam));
		case IDD_PMAX_EB:			// JLR, 10/17/95
		case IDD_PMAX_SB: return(dlg->pmax_pair.Scroll_Proc(message,wParam,lParam));

		case IDCANCEL:			       	    // Cancel button pressed
			EndDialog(hDlg,TRUE);
			break;

		case IDOK:				    // Ok button pressed
			if (dlg->rmin_pair.Value() > dlg->rmax_pair.Value())
				MessageBox(hDlg,"RMS Min is greater than RMS Max","ERROR",MB_OK);
			else
			{
				EndDialog(hDlg, TRUE);			    // close window and
				dlg->rms_min  = dlg->rmin_pair.Value();		    
				dlg->rms_max  = dlg->rmax_pair.Value();		    
				dlg->peak_max = dlg->pmax_pair.Value();		    // get updated limits  
			}
			break;
		}                                                                  
	}
	return (FALSE);
}


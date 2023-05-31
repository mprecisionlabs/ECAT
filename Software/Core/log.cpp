#include "stdafx.h"
#include "globals.h"
#include "log.h"
#include "dlgctl.h"
#include "message.h"
#include <time.h>

#define MODE_APPEND 0
#define MODE_REWRITE 1
#define MODE_INCNAME 2

// INI file constants
const LPCSTR INI_LOG_SECTION =			"LOGSETTINGS";
const LPCSTR INI_LS_FMODE =				"FILEMODE";
const LPCSTR INI_LS_STYLE =				"STYLE";
const LPCSTR INI_LS_STATE =				"LOGON";

const LPCSTR INI_LS_FMODE_APPEND =		"APPEND";
const LPCSTR INI_LS_FMODE_INCNAME =		"INCNAME";
const LPCSTR INI_LS_FMODE_REWRITE =		"REWRITE";

const LPCSTR INI_LS_STYLE_TEXT =		"TEXT";
const LPCSTR INI_LS_STYLE_EXPORT =		"EXPORT";

const LPCSTR INI_SWITCH_ON =			"ON";
const LPCSTR INI_SWITCH_OFF =			"OFF";

// Set the firmware ID to "" initially
char g_strFwId[MAX_DI_STR_LEN] = {""};

void CMUFileSave( HWND hWnd, LPSTR name, int size);
static char time_buf[128]; 
static char date_buf[128]; 

// HH:MM:SS
LPSTR time_str()	
{
	_strtime(time_buf);
	return (LPSTR)time_buf;
}

//mon dd,20xx
LPSTR date_str()	
{
    struct tm *today;
    time_t long_time;

    time(&long_time);               /* Get time as long integer. */
    today = localtime(&long_time);	/* Convert to local time. */
    
	/* Use strftime to build a customized time string. */
    strftime(date_buf, 128, "%B %d,%Y", today);
	return (LPSTR)date_buf;
}


BOOL __declspec(dllexport) CALLBACK LOG::SetupProc(HWND hMainDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	LOG FAR *lpLOG=(LOG FAR *)GetWindowLong(hMainDlg,DWL_USER);

	switch (message)
	{
	case WM_INITDIALOG:
		SetWindowLong(hMainDlg,DWL_USER,lParam);
		lpLOG=(LOG FAR *)lParam;
		SetDlgItemText(hMainDlg,LSDLG_NAME,lpLOG->filename);
		switch (lpLOG->append_mode)
		{
			case MODE_APPEND:  CheckRadioButton(hMainDlg,LSDLG_APPEND,LSDLG_INCNAME,LSDLG_APPEND);  break;
			case MODE_REWRITE: CheckRadioButton(hMainDlg,LSDLG_APPEND,LSDLG_INCNAME,LSDLG_REWRITE); break;
			case MODE_INCNAME: CheckRadioButton(hMainDlg,LSDLG_APPEND,LSDLG_INCNAME,LSDLG_INCNAME); break;  
			default: break;
		}
		if (lpLOG->normal_text) 
			CheckRadioButton(hMainDlg,LSDLG_TEXT,LSDLG_EXPORT,LSDLG_TEXT);
		else 
			CheckRadioButton(hMainDlg,LSDLG_TEXT,LSDLG_EXPORT,LSDLG_EXPORT);

		return TRUE;

	case WM_COMMAND:
		switch(LOWORD(wParam))
		{
		case IDOK:
			if (IsDlgButtonChecked(hMainDlg,LSDLG_APPEND))
				lpLOG->append_mode=MODE_APPEND;
			else if (IsDlgButtonChecked(hMainDlg,LSDLG_REWRITE))
				lpLOG->append_mode=MODE_REWRITE;
			else if (IsDlgButtonChecked(hMainDlg,LSDLG_INCNAME))
				lpLOG->append_mode=MODE_INCNAME;
			lpLOG->normal_text=IsDlgButtonChecked(hMainDlg,LSDLG_TEXT);
			GetDlgItemText(hMainDlg,LSDLG_NAME,lpLOG->filename,sizeof(lpLOG->filename));
			EndDialog(hMainDlg, TRUE);
			return TRUE;

		case LSDLG_SELECT:
			char tempname[sizeof(lpLOG->filename)];
			tempname[0]=0;
			_fstrcat(tempname,lpLOG->filename);
			CMUFileSave(hMainDlg,(LPSTR)tempname,sizeof(tempname));
			if (tempname[0]) SetDlgItemText(hMainDlg,LSDLG_NAME,tempname);
			return TRUE;

		case IDCANCEL:
			EndDialog(hMainDlg,TRUE);
			return TRUE;
		}

	default: 
		break;
	}

	return FALSE;
}


BOOL __declspec(dllexport) CALLBACK LOG::StartProc(HWND hMainDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	LOG FAR *lpLOG=(LOG FAR *)GetWindowLong(hMainDlg,DWL_USER);
	LPSTR comment_lp;
	int size;

	switch (message)
	{
	case WM_INITDIALOG:
		SetWindowLong(hMainDlg,DWL_USER,lParam);
		lpLOG=(LOG FAR *)lParam;
		SetDlgItemText(hMainDlg,LSDLG_NAME,lpLOG->filename);
		SetDlgItemText(hMainDlg,LSDLG_OPERATOR,lpLOG->operator_buf);
		SetDlgItemText(hMainDlg,LSDLG_EUT,lpLOG->eut_buf);
		if (lpLOG->comment_hmem)
		{
			comment_lp=(LPSTR)GlobalLock(lpLOG->comment_hmem);
			SetDlgItemText(hMainDlg,LSDLG_COMMENT,comment_lp);
			GlobalUnlock(lpLOG->comment_hmem);
		}

		switch (lpLOG->append_mode)
		{
		case MODE_APPEND:  CheckRadioButton(hMainDlg,LSDLG_APPEND,LSDLG_INCNAME,LSDLG_APPEND);  break;
		case MODE_REWRITE: CheckRadioButton(hMainDlg,LSDLG_APPEND,LSDLG_INCNAME,LSDLG_REWRITE); break;
		case MODE_INCNAME: CheckRadioButton(hMainDlg,LSDLG_APPEND,LSDLG_INCNAME,LSDLG_INCNAME); break;  
		}

		if (lpLOG->normal_text) 
			CheckRadioButton(hMainDlg,LSDLG_TEXT,LSDLG_EXPORT,LSDLG_TEXT);
		else 
			CheckRadioButton(hMainDlg,LSDLG_TEXT,LSDLG_EXPORT,LSDLG_EXPORT);
		SetFocus(GetDlgItem(hMainDlg,IDOK));
		return FALSE;

	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case IDOK:
			if (IsDlgButtonChecked(hMainDlg,LSDLG_APPEND))
				lpLOG->append_mode=MODE_APPEND;
			else if (IsDlgButtonChecked(hMainDlg,LSDLG_REWRITE))
				lpLOG->append_mode=MODE_REWRITE;
			else if (IsDlgButtonChecked(hMainDlg,LSDLG_INCNAME))
				lpLOG->append_mode=MODE_INCNAME;
			lpLOG->normal_text=IsDlgButtonChecked(hMainDlg,LSDLG_TEXT);

			GetDlgItemText(hMainDlg,LSDLG_NAME,lpLOG->filename,sizeof(lpLOG->filename));
			GetDlgItemText(hMainDlg,LSDLG_OPERATOR,lpLOG->operator_buf,sizeof(lpLOG->operator_buf));
			GetDlgItemText(hMainDlg,LSDLG_EUT,lpLOG->eut_buf,sizeof(lpLOG->eut_buf));
			size=SendDlgItemMessage(hMainDlg,LSDLG_COMMENT,WM_GETTEXTLENGTH,0,0L)+3;
			if (size<10) size=10;
			if (!lpLOG->comment_hmem)
				lpLOG->comment_hmem=GlobalAlloc(GHND,size);
			else
				lpLOG->comment_hmem=GlobalReAlloc(lpLOG->comment_hmem,size,GMEM_ZEROINIT);
			
			comment_lp=(LPSTR)GlobalLock(lpLOG->comment_hmem);
			if (!GetDlgItemText(hMainDlg,LSDLG_COMMENT,comment_lp,size))
				comment_lp[0]=0;
			
			GlobalUnlock(lpLOG->comment_hmem);
			EndDialog(hMainDlg, TRUE);
			return TRUE;

		case LSDLG_NOLOG:
			lpLOG->logging=FALSE;
			EndDialog(hMainDlg, TRUE);
			return TRUE;

		case LSDLG_SELECT:
			char tempname[sizeof(lpLOG->filename)];
			tempname[0]=0;
			_fstrcat(tempname,lpLOG->filename);
			CMUFileSave(hMainDlg,(LPSTR)tempname,sizeof(tempname));
			if (tempname[0]) 
				SetDlgItemText(hMainDlg,LSDLG_NAME,tempname);
			return TRUE;
		
		case IDCANCEL:
			EndDialog(hMainDlg,FALSE);
			return TRUE;

		default:
			break;
		}

	default:
		break;
	}

	return FALSE;
}

BOOL __declspec(dllexport) CALLBACK LOG::CommentProc(HWND hMainDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	LOG FAR *lpLOG=(LOG FAR *) GetWindowLong(hMainDlg,DWL_USER);

	switch (message)
	{
	case WM_INITDIALOG:
		SetWindowLong(hMainDlg,DWL_USER,lParam);
		lpLOG=(LOG FAR *)lParam;
		return TRUE;

	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case IDOK:
			lpLOG->LogItem(GetDlgItem(hMainDlg,LCDLG_TEXT));
			EndDialog(hMainDlg, TRUE);
			return TRUE;

		case IDCANCEL:
			EndDialog(hMainDlg, TRUE);
			return TRUE;

		default:
			break;
		}
		break;
	}
	
	return FALSE;
}

void LOG::comment(LPSTR str)
{
	if (str==NULL)
		DoDlg((FARPROC)CommentProc,MAKEINTRESOURCE(IDD_COMMENT));
	else 
		loglpstr(str);
}

BOOL LOG::DoDlg(FARPROC fp, LPCSTR dlgname)
{
	int rv=DialogBoxParam(ghinst,dlgname,(HWND)main_window,(DLGPROC)fp,(long)((LPSTR)this));
	if (rv==-1) 
		Message("Dialog Error","Unable to create Dialog: %Fs",dlgname);
	return rv;
}

LOG::LOG(void)
{ 
	created = FALSE;
	memset(filename, 0, sizeof(filename));
	memset(operator_buf, 0, sizeof(operator_buf));
	memset(eut_buf, 0, sizeof(eut_buf));
	comment_hmem = NULL;
	rev = NULL;
	hFile = INVALID_HANDLE_VALUE;
}

LPSTR LOG::getlogtime()
{
	return time_str();
}

//Sets up Handles
void LOG::log_init(HWND hwnd, LPSTR name, LPSTR rev_string) 
{
	wsprintf(filename, "%s.log", get_app_data_file_path(name));
	rev = rev_string;

	char profile_buff[MAX_PATH + 1];
	memset(profile_buff, 0, sizeof(profile_buff));

	// File mode settings
	GetPrivateProfileString(INI_LOG_SECTION, INI_LS_FMODE, INI_LS_FMODE_APPEND, profile_buff, MAX_PATH, get_app_data_file_path(INI_NAME));
	SWITCH(profile_buff)

		CASEi(INI_LS_FMODE_APPEND)
			append_mode = MODE_APPEND;
		
		CASEi(INI_LS_FMODE_INCNAME)
			append_mode = MODE_INCNAME;

		CASEi(INI_LS_FMODE_REWRITE)
			append_mode = MODE_REWRITE;

		DEFAULT
			append_mode = MODE_APPEND;

	ENDSWITCH

	// Style settings
	GetPrivateProfileString(INI_LOG_SECTION, INI_LS_STYLE, INI_LS_STYLE_TEXT, profile_buff, MAX_PATH, get_app_data_file_path(INI_NAME));
	SWITCH(profile_buff)

		CASEi(INI_LS_STYLE_EXPORT)
			normal_text = FALSE;

		DEFAULT
			normal_text = TRUE;
	
	ENDSWITCH

	// Logging is on or off
	GetPrivateProfileString(INI_LOG_SECTION, INI_LS_STATE, INI_SWITCH_OFF, profile_buff, MAX_PATH, get_app_data_file_path(INI_NAME));
	SWITCH(profile_buff)

		CASEi(INI_SWITCH_ON)
			logging = TRUE;

		DEFAULT
			logging = FALSE;

	ENDSWITCH

	created = TRUE;
}

//Starts the log at the begining of the run
BOOL LOG::start_log()
{
	LPSTR comment_lp;
	if (!DoDlg((FARPROC)StartProc, MAKEINTRESOURCE(IDD_START_LOG))) 
		return FALSE;

	logtime = FALSE;
	if (!logging) 
		return TRUE;

	DWORD dwFlags = ((append_mode == MODE_APPEND) ? OPEN_ALWAYS : CREATE_ALWAYS);
	hFile = CreateFile(
				filename, 
				GENERIC_WRITE, 
				FILE_SHARE_READ, 
				NULL, 
				dwFlags,
				FILE_ATTRIBUTE_NORMAL,
				NULL
				);
	if (hFile == INVALID_HANDLE_VALUE)
	{
		Message("Failed to open Log ","Unable to open Log file : %Fs",(LPSTR)filename);
		return FALSE;
	}

	// If we are appending data, seek to the end of the file
	if (append_mode == MODE_APPEND)
		SetFilePointer(hFile, 0, 0, FILE_END);

	logprintf("Log File\r\n");
	logprintf("Software:%Fs\r\n",rev);
	if (SIMULATION)
		logprintf("Firmware:Simulated\r\n");
	else  
		logprintf("Firmware::%Fs\r\n", g_strFwId);

	logprintf("Modules:\r\n");
	for (int i=0; i<MAX_CAL_SETS; i++)
	{
		if (NOT_GHOST(i))
		{
			if (i &1) 
				logprintf("Row %d Left :",(i/2)+1);
			else 
				logprintf("Row %d Right:",(i/2)+1);

			//VictorGinzburg 04/22/97
			logprintf("%Fs SN:%7ld\r\n",calset[i].name,calset[i].serial);
			/////////////////////////
		}
	}

	logprintf("Test Started at %Fs on %Fs\n", (LPSTR)time_str(), (LPSTR)date_str());
	if (!fnamebuf[0])
		logprintf("Test File:Untitled\n");
	else
		logprintf("Test File:%Fs\n",(LPSTR)fnamebuf);

	logprintf("Operator :%Fs\n",(LPSTR)operator_buf);
	logprintf("EUT:%Fs\n",(LPSTR)eut_buf);
	if (comment_hmem)
	{
		comment_lp=(LPSTR)GlobalLock(comment_hmem);
		logprintf("Comments: %Fs\n",comment_lp);
		GlobalUnlock(comment_hmem);
	}
	else 
		logprintf("Comments:NA\n");
	
	logtime=TRUE;
	return TRUE;
}
 
//Runs the Setup Dialog.
void LOG::setup_log()
{
	DoDlg((FARPROC)SetupProc, MAKEINTRESOURCE(IDD_SETUP_LOG));
}

//Turns the Log on or Off
void LOG::set_log_on(BOOL on)
{
	logging = on;
}
           
//Closes the log file.
void LOG::end_log()
{
	loglpstr("Log Closed\n\n");
	if (hFile != INVALID_HANDLE_VALUE)
	{
		FlushFileBuffers(hFile);
		CloseHandle(hFile);
		hFile = INVALID_HANDLE_VALUE;
	}
}

// Logs Like a printf
void LOG::logprintf(char *szFormat,...)
{
	char szBuffer[MAX_PATH + 1];
	memset(szBuffer, 0, sizeof(szBuffer));
	va_list argList;
	va_start(argList, szFormat);
	_vsnprintf(szBuffer, MAX_PATH, szFormat, argList);
	va_end(argList);
	loglpstr((LPSTR)szBuffer);
}

//Logs a String
void LOG::loglpstr(LPSTR ss)
{
	char buffer [100];
	char buffer2[40];
	int size;
	LPSTR lp;
	LPSTR lin;
	LPSTR lout;
	LPSTR temp;
	HGLOBAL ghmem;
	ghmem=0;
	DWORD dwBytesToWrite;

	// Sanity check
	if ((hFile == INVALID_HANDLE_VALUE) || (!logging)) 
		return;

	if (logtime)
	{
		if (normal_text) 
			sprintf(buffer,"%s:",(LPSTR)time_str());
		else 
			sprintf(buffer,"%s:\t",(LPSTR)time_str());
		dwBytesToWrite = strlen(buffer);
		WriteFile(hFile, buffer, dwBytesToWrite, &dwBytesToWrite, NULL);
	}

	size=_fstrlen(ss);
	if (size >80)
	{
		ghmem=GlobalAlloc(GHND,size+20);
		if (!ghmem)  
			Message("Alloc Failure","Global Alloc Failure in LOG.CPP size: %d",size);
		lp=(LPSTR)GlobalLock(ghmem);
	}
	else 
		lp=buffer;

	lout=lp;
	lin=ss;
	
	while (*lin)
	{
		if (*lin=='\n')
		{ 
			*(lout++)='\r'; 
			size++; 
		}
		
		if (*lin=='\r')
		{
			if (*(lin+1)=='\n')
			{
				*lout++=*lin++; //Don't add an extra \n to \r\n
				if (logtime && (!normal_text)) 
				{
					*lout++=*lin++;
					sprintf(buffer2,"%Fs:\t",(LPSTR)time_str());
					temp=buffer2;
					while (*temp) 
					{
						*lout++=*temp++; 
						size++; 
					}
				}
			}
			else 
			{
				lin++; 
				size--; 
			}
		}

		if ((*lin=='\t') && (normal_text))
		{
			*lin++; 
			size--;
		}
		
		if (*lin) 
			*lout++=*lin++;
	}

	dwBytesToWrite = size;
	WriteFile(hFile, lp, dwBytesToWrite, &dwBytesToWrite, NULL);
	if (ghmem)
	{
		GlobalUnlock(ghmem);
		GlobalFree(ghmem);
	}

}

//Logs the text from a control 
void LOG::LogItem(HANDLE hwnd)
{
	int size;
	char buffer[80];
	LPSTR lpp;
	HGLOBAL ghmem;
	size=SendMessage((HWND)hwnd,WM_GETTEXTLENGTH,0,0L)+3;
	if (size <80) 
		lpp=buffer;
	else
	{
		ghmem=GlobalAlloc(GHND,size);
		if (!ghmem) 
		{
			Message("Alloc Failure","Global Alloc Failure in LOG.CPP size: %d",size);
			lpp=buffer;
			size=79;
		}
		else
			lpp=(LPSTR)GlobalLock(ghmem);
	}

	if (!SendMessage((HWND)hwnd,WM_GETTEXT,size,(LONG)(LPSTR)lpp))
		lpp[0]=0;
	
	_fstrcat(lpp,"\n");
	loglpstr(lpp);        
	
	if (size>=80)
	{ 
		GlobalUnlock(ghmem);
		GlobalFree(ghmem);
	}
}


/*********************************************************************
Using the OPENFILENAME structure and the Windows 3.1 API call
GetSaveFileName() eases the selection of files for the programmer and for
the user.  The WINHELP.EXE help file WIN31WH.HLP (found in the BORLANDC\BIN
directory) contains a detailed description of the function call and its
associated structure.  The Flags field of the structure is particularly
useful when custimization is required.
**********************************************************************/
void CMUFileSave( HWND hWnd, LPSTR name, int size)
{
	OPENFILENAME ofnTemp;
	DWORD Errval;	// Error value
	char buf[5];	// Error buffer
	char Errstr[50]="GetOpenFileName returned Error #";
	char szTemp[] = "Log Files (*.LOG)\0*.LOG\0All Files (*.*)\0*.*\0";

	/*
	Some Windows structures require the size of themselves in an effort to
	provide backward compatibility with future versions of Windows.  If the
	lStructSize member is not set the call to GetOpenFileName() will fail.
	*/
	ofnTemp.lStructSize = sizeof( OPENFILENAME );
	ofnTemp.hwndOwner = hWnd;	// An invalid hWnd causes non-modality
	ofnTemp.hInstance = 0;
	ofnTemp.lpstrFilter = (LPSTR)szTemp;	// See previous note concerning string
	ofnTemp.lpstrCustomFilter = NULL;
	ofnTemp.nMaxCustFilter = 0;
	ofnTemp.nFilterIndex = 1;
	ofnTemp.lpstrFile = name;	// Stores the result in this variable
	ofnTemp.nMaxFile = size;
	ofnTemp.lpstrFileTitle = NULL;
	ofnTemp.nMaxFileTitle = 0;
	ofnTemp.lpstrInitialDir = NULL;
	ofnTemp.lpstrTitle = "LOG FILENAME";	// Title for dialog
	ofnTemp.Flags = OFN_HIDEREADONLY | OFN_PATHMUSTEXIST;
	ofnTemp.nFileOffset = 0;
	ofnTemp.nFileExtension = 0;
	ofnTemp.lpstrDefExt =NULL;
	ofnTemp.lCustData = NULL;
	ofnTemp.lpfnHook = NULL;
	ofnTemp.lpTemplateName = NULL;
	
	/*
	If the call to GetOpenFileName() fails you can call CommDlgExtendedError()
	to retrieve the type of error that occured.
	*/
	if(GetSaveFileName( &ofnTemp ) != TRUE)
	{
		Errval=CommDlgExtendedError();
		if(Errval!=0)	// 0 value means user selected Cancel
		{
			sprintf(buf,"%ld",Errval);
			strcat(Errstr,buf);
			MessageBox(hWnd,Errstr,"WARNING",MB_OK|MB_ICONSTOP);
		}
		name[0]=0;
	}
}                      

LOG::~LOG(void)
{
	LPCSTR lpMode = NULL;
	switch (append_mode)
	{
	default:
	case MODE_APPEND: 
		lpMode = INI_LS_FMODE_APPEND;
		break;

	case MODE_REWRITE: 
		lpMode = INI_LS_FMODE_REWRITE;
		break;

	case MODE_INCNAME: 
		lpMode = INI_LS_FMODE_INCNAME;
		break;
	}
	WritePrivateProfileString(INI_LOG_SECTION, INI_LS_FMODE, lpMode, get_app_data_file_path(INI_NAME)); 
	WritePrivateProfileString(INI_LOG_SECTION, INI_LS_STYLE, (normal_text ? INI_LS_STYLE_TEXT : INI_LS_STYLE_EXPORT), get_app_data_file_path(INI_NAME));
	WritePrivateProfileString(INI_LOG_SECTION, INI_LS_STATE, (logging ? INI_SWITCH_ON : INI_SWITCH_OFF), get_app_data_file_path(INI_NAME));

	if (comment_hmem) 
		GlobalFree(comment_hmem);
}

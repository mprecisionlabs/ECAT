#ifndef LOG_H
#define LOG_H

#include "globals.h"

// Will be set in kt_info to the firmware ID string

extern char g_strFwId[MAX_DI_STR_LEN];

class LOG
{
public:
	LOG(void);
	virtual ~LOG(void);

protected:
	HANDLE main_window;
	BOOL created;
	char filename[MAX_PATH];
	char operator_buf[100];
	char eut_buf[100];
	HGLOBAL comment_hmem;
	int  append_mode;
	BOOL normal_text;
	BOOL logging;
	LPSTR rev;
	HANDLE hFile;

protected:
	static BOOL __declspec(dllexport) CALLBACK LOG::SetupProc(HWND hMainDlg, UINT message, WPARAM wParam, LPARAM lParam);
	static BOOL __declspec(dllexport) CALLBACK LOG::StartProc(HWND hMainDlg, UINT message, WPARAM wParam, LPARAM lParam);
	static BOOL __declspec(dllexport) CALLBACK LOG::CommentProc(HWND hMainDlg, UINT message, WPARAM wParam, LPARAM lParam);
	BOOL DoDlg(FARPROC fp, LPCSTR dlgname);

public:
	BOOL logtime;
	HANDLE logfile() {return hFile;}

public:
	void log_init(HWND hwnd,LPSTR name, LPSTR rev); //Sets up Handles
	BOOL start_log();//Starts the log at the begining of the run
	void setup_log();//Runs the Setup Dialog.
	void comment(LPSTR);//Runs the comment Dialog if LPSTR is NULL otherwise
					 //It logs the comment inf the log file.
	void set_log_on(BOOL);//Turns the Log on or Off
	BOOL log_on(){return logging;};//Reads that state.
	void end_log();//Closes the log file.
	int file_mode() {return append_mode; };
	BOOL normal() {return normal_text; };
	void logprintf(char *szFormat,...);// Logs Like a printf
	void loglpstr(LPSTR);//Logs a String
	void LogItem(HANDLE );//Logs the text from a control 
	LPSTR getlogtime();
};
#endif

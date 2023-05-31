#include "stdafx.h"
#include "dlgCtl.h"
#include "message.h"

void Outline(HWND hwndg,HPEN pen);
void  RegisterClasses(HANDLE hInstance);
BOOL registered;

BOOL CALLBACK dlgCtl::DlgCtlProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	dlgCtl FAR *lpdlgCtl=(dlgCtl FAR *)GetWindowLong(hDlg,DWL_USER);

	if (lpdlgCtl!=NULL) return((*lpdlgCtl).DlgProc(hDlg,message,wParam,lParam));
	if (message==WM_INITDIALOG)
	{
		SetWindowLong(hDlg,DWL_USER,lParam);
		lpdlgCtl=(dlgCtl FAR *)lParam;
		return((*lpdlgCtl).DlgProc(hDlg,message,wParam,lParam));
	}
	return FALSE;
}


void dlgCtl::Destroy()
{
	DestroyWindow(hDlgChild);
}

long __declspec(dllexport) CALLBACK dlgCtlWndProc (HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	long rv;
	rv=DefDlgProc(hwnd,message,wParam,lParam);
	return rv;
}


void dlgCtl::create(HANDLE hInstance,HWND hwnd,RECT& rect,LPSTR name)
{
	parent=hwnd;
	Instance=hInstance;
	if (!registered) 
		RegisterClasses(hInstance);
	if (lpfnDlg == 0) 
		lpfnDlg = (FARPROC)(dlgCtl::DlgCtlProc);

	hDlgChild=CreateDialogParam((HINSTANCE)hInstance,name,hwnd,(DLGPROC)lpfnDlg,(long)(this));
	GetWindowRect(hDlgChild,&Rect);
	MoveWindow(hDlgChild,rect.left,rect.top,Rect.right-Rect.left,Rect.bottom-Rect.top,FALSE);
	GetWindowRect(hDlgChild,&Rect);
	rect.top+=(Rect.bottom-Rect.top);
}

void dlgCtl::dialog(HANDLE hInstance,HWND hwnd,LPSTR name)
{
	parent=hwnd;
	Instance=hInstance;
	if (!registered) 
		RegisterClasses(hInstance);
	if (lpfnDlg == 0) 
		lpfnDlg = (FARPROC)(dlgCtl::DlgCtlProc);
	hDlgChild=(HWND)DialogBoxParam((HINSTANCE)hInstance, name, hwnd, (DLGPROC)lpfnDlg, (long)(this));
	DBGOUT("hDlgChild: 0x%08X; LastErr: %u\n", hDlgChild, GetLastError());
}

//Behaves like printf, only sends output to a dialog item
//Uses normal C format specifiers.
void PrintfDlg(HWND hDlg, int ndlg, char *szFormat, ...)
{
	char szBuffer[MAX_PATH + 1];
	memset(szBuffer, 0, sizeof(szBuffer));
	va_list argList;
	va_start(argList, szFormat);
	_vsnprintf(szBuffer, MAX_PATH, szFormat, argList);
	va_end(argList);
	if (ndlg==-1)
		SetWindowText(hDlg,szBuffer);
	else 
		SetDlgItemText(hDlg,ndlg,szBuffer);
}

//This function draws a hash mark on the center of the thumb
//of the Scroll Bar as passed in the handle hswnd
void CenterPaintScroll(HWND hswnd,HPEN hpen)
{
	HDC	hDc=GetDC(hswnd);
	RECT rect;

	//Get Dimensional information
	GetClientRect(hswnd,&rect);
	LONG  dx=rect.right-rect.left;
	LONG  x1=GetSystemMetrics(SM_CXHSCROLL);
	LONG  x2=GetSystemMetrics(SM_CXHTHUMB);
	LONG xp=GetScrollPos(hswnd,SB_CTL);

	//Calculate the actual thumb range
	dx=(dx-(2*x1+x2))+2;

	int minx,maxx;
	GetScrollRange(hswnd,SB_CTL,(LPINT)&minx,(LPINT)&maxx);

	//Calculate the thumb position scaleing 
	dx=(dx*(xp-minx))/(maxx-minx);
	SelectObject(hDc,hpen);
	POINT ptOld;
	MoveToEx(hDc,rect.left+x1+(x2/2)+dx-1,rect.top+4,&ptOld);
	LineTo(hDc,rect.left+x1+(x2/2)+dx-1,rect.bottom-5);
	SelectObject(hDc,GetStockObject(BLACK_PEN));

	MoveToEx(hDc,rect.left+x1+(x2/2)+dx+1,rect.top+2,&ptOld);
	LineTo(hDc,rect.left+x1+(x2/2)+dx+1,rect.bottom-3);
	MoveToEx(hDc,rect.left+x1+(x2/2)+dx-3,rect.top+2,&ptOld);
	LineTo(hDc,rect.left+x1+(x2/2)+dx-3,rect.bottom-3);

	ReleaseDC(hswnd,hDc);

}

long __declspec(dllexport) CALLBACK SuperScroll (HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	LONG top =GetClassLong(hwnd,GCL_CBCLSEXTRA);
	WNDPROC lpfn=(WNDPROC)GetClassLong(hwnd,top-4);
	if (lpfn==NULL) 
		return 1;
	LONG rv=CallWindowProc(lpfn,hwnd,message,wParam,lParam);
	if (message !=WM_PAINT) 
		return rv;
	top =GetClassLong(hwnd,GCL_CBWNDEXTRA);
	HPEN hpen=(HPEN)GetWindowLong(hwnd,top-2);
	return rv;
}

void  RegisterClasses(HANDLE hInstance)
{
	WNDCLASS wndclass;
	HWND hwnd;
	LPFN oldlpfn;
	GetClassInfo(NULL,"SCROLLBAR",&wndclass);
	wndclass.cbWndExtra+=4;
	wndclass.cbClsExtra+=4;
	wndclass.lpszClassName="SSCROLLBAR";
	wndclass.hInstance=(HINSTANCE)hInstance;
	oldlpfn=(LPFN)wndclass.lpfnWndProc;
	wndclass.lpfnWndProc=SuperScroll;
	RegisterClass (&wndclass);
	hwnd=CreateWindow("SSCROLLBAR","BOGUS",
					WS_OVERLAPPEDWINDOW,
					CW_USEDEFAULT, CW_USEDEFAULT,
					CW_USEDEFAULT, CW_USEDEFAULT,
					NULL, NULL, (HINSTANCE)hInstance,NULL);
	SetClassLong(hwnd,wndclass.cbClsExtra-4,(LONG)oldlpfn);
	DestroyWindow(hwnd);

	registered=TRUE;
}


LONG MoveRel(HDC hdc,int dx,int dy)
{
	POINT cp;
	GetCurrentPositionEx(hdc,&cp);
	MoveToEx(hdc,cp.x+dx,cp.y+dy,NULL);
	return MAKELONG(cp.x+dx,cp.y+dy);
}

LONG LineRel(HDC hdc,int dx,int dy)
{
	POINT cp;
	GetCurrentPositionEx(hdc,&cp);
	LineTo(hdc,cp.x+dx,cp.y+dy);
	return MAKELONG(cp.x+dx,cp.y+dy);
}

void SaveListbox(HWND hwnd,int f)
{
	char TempBuf[80];
	LONG n;
	LONG i;
	LONG len;
	n=SendMessage(hwnd,LB_GETCOUNT,0,0L);
	_lwrite(f,(LPSTR)&n,sizeof(n));
	for (i=0; i<n; i++)
	{
		len=SendMessage(hwnd,LB_GETTEXT,i,(LONG)((LPSTR)TempBuf));
		_lwrite(f,(LPSTR)&len,sizeof(len));
		_lwrite(f,(LPSTR)TempBuf,len);
	}

	n=SendMessage(hwnd,LB_GETCURSEL,0,0L);
	_lwrite(f,(LPSTR)&n,sizeof(n));
	n=SendMessage(hwnd,LB_GETTOPINDEX,0,0L);
	_lwrite(f,(LPSTR)&n,sizeof(n));
}

void LoadListbox(HWND hwnd,int f)
{
	char TempBuf[80];
	LONG n;
	LONG i;
	WORD len;
	SendMessage(hwnd,LB_RESETCONTENT,0,0L);
	_lread(f,(LPSTR)&n,sizeof(n));
	for (i=0; i<n; i++)
	{
		_lread(f,(LPSTR)&len,sizeof(len));
		_lread(f,(LPSTR)TempBuf,len);
		TempBuf[len]='\0';
		SendMessage(hwnd,LB_INSERTSTRING,-1,(LONG)((LPSTR)TempBuf));
	}

	_lread(f,(LPSTR)&n,sizeof(n));
	SendMessage(hwnd,LB_SETCURSEL,n,0L);
	_lread(f,(LPSTR)&n,sizeof(n));
	SendMessage(hwnd,LB_SETTOPINDEX,n,0L);
	SendMessage(GetParent(hwnd),WM_COMMAND,MAKEWPARAM(GetWindowLong(hwnd,GWL_ID),CBN_SELCHANGE),(LPARAM) hwnd);
}

void SaveCombobox(HWND hwnd,int f)
{
	char TempBuf[80];
	LONG n;
	LONG i;
	LONG len;
	n=SendMessage(hwnd,CB_GETCOUNT,0,0L);
	_lwrite(f,(LPSTR)&n,sizeof(n));
	for (i=0; i<n; i++)
	{
		len=SendMessage(hwnd,CB_GETLBTEXT,i,(LONG)((LPSTR)TempBuf));
		_lwrite(f,(LPSTR)&len,sizeof(len));
		_lwrite(f,(LPSTR)TempBuf,len);
	}

	n=SendMessage(hwnd,CB_GETCURSEL,0,0L);
	_lwrite(f,(LPSTR)&n,sizeof(n));
}

void LoadCombobox(HWND hwnd,int f)
{
	char TempBuf[80];
	LONG n;
	LONG i;
	WORD len;
	HANDLE parent=GetParent(hwnd);
	HANDLE Win_ID=(HANDLE)GetWindowLong(hwnd,GWL_ID);
	SendMessage(hwnd,CB_RESETCONTENT,0,0L);
	_lread(f,(LPSTR)&n,sizeof(n));
	for (i=0; i<n; i++)
	{
		_lread(f,(LPSTR)&len,sizeof(len));
		_lread(f,(LPSTR)TempBuf,len);
		TempBuf[len]='\0';
		SendMessage(hwnd,CB_INSERTSTRING,-1,(LONG)((LPSTR)TempBuf));
	}

	_lread(f,(LPSTR)&n,sizeof(n));
	SendMessage(hwnd,CB_SETCURSEL,n,0L);
	SendMessage((HWND)parent,WM_COMMAND,MAKEWPARAM(Win_ID,CBN_SELCHANGE),(LPARAM) hwnd);
}



void SaveStaticText(HWND hwnd,int f)
{
	char TempBuf[100];
	int len;
	len=GetDlgItemText(GetParent(hwnd),GetWindowLong(hwnd,GWL_ID),TempBuf,100);
	TempBuf[len]='\0';
	_lwrite(f,(LPSTR)&len,sizeof(len));
	_lwrite(f,(LPSTR)TempBuf,len);
}

void LoadStaticText(HWND hwnd,int f)
{
	char TempBuf[100];
	int len;
	_lread(f,(LPSTR)&len,sizeof(len));
	_lread(f,(LPSTR)TempBuf,len);
	TempBuf[len]='\0';
	SetDlgItemText(GetParent(hwnd),GetWindowLong(hwnd,GWL_ID),TempBuf);
}

void SaveCustListbox(HWND hwnd,int f)
{
	LONG data;
	LONG n;
	LONG i;
	n=SendMessage(hwnd,LB_GETCOUNT,0,0L);
	_lwrite(f,(LPSTR)&n,sizeof(n));
	for (i=0; i<n; i++)
	{
		data=SendMessage(hwnd,LB_GETITEMDATA,i,0L);
		_lwrite(f,(LPSTR)&data,sizeof(data));
	}

	n=SendMessage(hwnd,LB_GETCURSEL,0,0L);
	_lwrite(f,(LPSTR)&n,sizeof(n));
	n=SendMessage(hwnd,LB_GETTOPINDEX,0,0L);
	_lwrite(f,(LPSTR)&n,sizeof(n));
}

void LoadCustListbox(HWND hwnd,int f)
{
	LONG data;
	LONG n;
	LONG i;
	SendMessage(hwnd,LB_RESETCONTENT,0,0L);
	_lread(f,(LPSTR)&n,sizeof(n));
	for (i=0; i<n; i++)
	{
		_lread(f,(LPSTR)&data,sizeof(data));
		SendMessage(hwnd,LB_INSERTSTRING,-1,data);
	}

	_lread(f,(LPSTR)&n,sizeof(n));
	if (n)
		SendMessage(hwnd,LB_SETCURSEL,n,0L);
	_lread(f,(LPSTR)&n,sizeof(n));
	SendMessage(hwnd,LB_SETTOPINDEX,n,0L);
	SendMessage(GetParent(hwnd),WM_COMMAND,MAKEWPARAM(GetWindowLong(hwnd,GWL_ID),CBN_SELCHANGE),(LPARAM) hwnd);
}

// Behaves like printf, only sends output to a file handle
int fhprintf(HANDLE hFile, char* szFormat, ...)
{
	char szBuffer[MAX_PATH + 1];
	memset(szBuffer, 0, sizeof(szBuffer));
	
	va_list argList;
	va_start(argList, szFormat);
	int nb = _vsnprintf(szBuffer, MAX_PATH, szFormat, argList);
	va_end(argList);

	DWORD dwByteCnt = nb;
	if (WriteFile(hFile, szBuffer, dwByteCnt, &dwByteCnt, NULL))
		return nb;

	return 0;
}

static BOOL IsBinaryData(LPSTR lpszData)
{
	while (*lpszData)
	{
		if (*lpszData++ > 127)
			return TRUE;
	}

	return FALSE;
}

int dlgCtl::load_objects(HANDLE hFile)
{
	int err = 0;

	// Clean up dialogs
	for (int i = 0; i < dlgcnt; i++)
	{
		if (((dlgCtl*) dlglist[i])->created)
			((dlgCtl*) dlglist[i])->Clear();
	}

	// Try to obtain hFile's size 
	DWORD dwFileSize = GetFileSize(hFile, NULL); 
 
	// If we failed, return with error code 1
	if (dwFileSize == 0xFFFFFFFF) 
	{
		Message("Bad File Size", "Error getting file size, unable to parse.");
		return 1;
	}
 
	// Allocate the buffer to hold our data
	char* dataBuf = new char[dwFileSize + 1];
	if (dataBuf == NULL)
	{
		Message("Fatal Error", "Not enough memory.");
		return 1;
	}

	// Read the file into dataBuf
	dataBuf[dwFileSize] = '\0';
	if (!ReadFile(hFile, dataBuf, dwFileSize, &dwFileSize, NULL))
	{
		Message("Fatal Error", "Error occured while reading the file, unable to parse.");
		delete[] dataBuf;
		return 1;
	}

	// Get lines one by one and parse them (lines are separated by \r\n)
	BOOL bDlgFound = FALSE;
	char seps[] = "\r\n";
	LPSTR token = strtok(dataBuf, seps);
	while (token != NULL)
	{
		if ((strlen(token) > 80) || IsBinaryData(token))
		{
			Message("Bad File Format", "Specified file is either not a text file or lines are too long. Unable to parse.");
			delete[] dataBuf;
			return 1;
		}

		// While there are tokens in dataBuf, pass each to the appropriate dialog
		for (int i = 0; i < dlgcnt; i++)
		{
			BOOL cr = ((dlgCtl*) dlglist[i])->created;
			if (cr) 
			{//CR
				LPSTR name = ((dlgCtl*) dlglist[i])->Obj_Name;
				LPSTR fssr = strstr(token, name);
				if (fssr == token) // first token is the dialog's name
				{
					LPSTR cp = _fstrchr(token,':');
					if (cp) 
					{
						if (((dlgCtl*) dlglist[i])->Process_Line(cp + 1)) 
							Message("UnParsed:", "List[%d]=\n%Fs\n%Fs", i, (LPSTR)name, cp);
						bDlgFound = TRUE;
					}
					break;
				}//Fssr==token
			}//CR
		}//For

		if (!bDlgFound) 
		{
			err = 1;
			Message("Cant find appropriate handler:", "%Fs", (LPSTR) token);
		}

		// Get next token
		token = strtok(NULL, seps);
	}

	delete[] dataBuf;
	return err;

}//Function

dlgCtl::dlgCtl(void)
{
	Obj_Name[0]=0; 
	created=FALSE;
	dlglist[dlgcnt++]=this;
};

dlgCtl::~dlgCtl(void)
{

	for (int i=0; i<dlgcnt; i++)
	{
		if (dlglist[i] == this)
		{
			created=FALSE;
			dlglist[i]=0;
			dlgcnt--;
			break;
		}
	}
};

void  dlgCtl::MoveBy(int x_rt,int y_dn)
{
	RECT  rect;
	int   w,h;

	GetWindowRect(hDlgChild,&rect);
	w = rect.right -rect.left;
	h = rect.bottom-rect.top;
	ScreenToClient((HWND)parent,(POINT *)&rect);
	MoveWindow(hDlgChild,rect.left + x_rt,rect.top + y_dn,w,h,TRUE);
}


dlgCtl* dlgCtl::dlglist[30];
int  dlgCtl::dlgcnt;
FARPROC dlgCtl::lpfnDlg;

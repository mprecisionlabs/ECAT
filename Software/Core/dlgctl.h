#ifndef DLGCTRL
#define DLGCTRL

typedef DLGPROC LPFN;

void  PrintfDlg (HWND hDlg, int ndlg, char * szFormat,...);

void Outline(HWND hwndg,HPEN pen);
void LoadListbox(HWND hwnd,int f);
void SaveListbox(HWND hwnd,int f);
void LoadCustListbox(HWND hwnd,int f);
void SaveCustListbox(HWND hwnd,int f);
void SaveCombobox(HWND hwnd,int f);
void LoadCombobox(HWND hwnd,int f);
void SaveStaticText(HWND hwnd,int f);
void LoadStaticText(HWND hwnd,int f);
int fhprintf(HANDLE fh,char * szFormat,...);

LONG MoveRel(HDC,int,int);
LONG LineRel(HDC,int,int);

class far dlgCtl
{
public:
	dlgCtl(void);
	virtual ~dlgCtl(void);

private: 
	static FARPROC lpfnDlg;

protected:
	HANDLE parent;
	BOOL created;
	HWND hDlgChild;
	LONG Notify_Message;
	LONG Notify_Window;
	LONG N_steps;
	BOOL ichanged;
	BOOL running;
	HANDLE Instance;
	char Obj_Name[40];
	RECT Rect;

	static dlgCtl* dlglist[];
	static int dlgcnt;
	static BOOL CALLBACK DlgCtlProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);
	
	virtual BOOL DlgProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)=0;

public:
	static int load_objects(HANDLE hfile);

	void dialog(HANDLE hInstance,HWND hwnd,LPSTR name);
	void create(HANDLE ,HWND ,RECT&,LPSTR);
	void Notify(LONG msg, LONG parr=0){Notify_Message=msg; Notify_Window=parr;};
	void Destroy();
	BOOL changed(){if (ichanged) {ichanged=FALSE; return TRUE;} else return FALSE;};
	
	virtual BOOL fixed() {return 1;}; //???Pure Virtual
	virtual LONG ReportSteps(void){return 0;}; 
	virtual BOOL Set_Next_Step(){return TRUE;};
	virtual void  Set_Update(BOOL){};
	virtual void  Show_Run(BOOL){};
	virtual void Set_Start_Values(){};
	virtual void Clear(void)=0; //pure virtual
	virtual int SaveText(HANDLE)=0;//pure virtual
	virtual int Process_Line(LPSTR)=0;//pure virtual

	LPSTR GetName(void){return Obj_Name;};
	void  MoveBy(int x_rt,int y_dn);   //##### removed by Ken Merrithew. causes duplicate definition error.
 };

long __declspec(dllexport) CALLBACK dlgCtlWndProc (HWND, UINT, WPARAM, LPARAM);
long __declspec(dllexport) CALLBACK SuperScroll (HWND, UINT, WPARAM, LPARAM);
long __declspec(dllexport) CALLBACK SuperEdit(HWND,UINT, WPARAM, LPARAM);

#endif

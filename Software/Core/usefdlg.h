int DoFileOpenDlg (HANDLE hInst, HWND hwnd, LPSTR szFileSpecIn,
		   LPSTR  szDefExtIn, WORD wFileAttrIn,
		   LPSTR  szFileNameOut, POFSTRUCT pofIn);
int DoFileSaveDlg (HANDLE hInst, HWND hwnd, LPSTR szFileSpecIn,
		   LPSTR szDefExtIn, WORD *pwStatusOut,
		   LPSTR szFileNameOut, POFSTRUCT pofIn);

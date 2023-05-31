// Common functions/structures used by all three programs E400, E500, PQFWare.
// Include this file in stdafx.h
#ifndef __THERMO_KEYTEK_UTIL_H__
#define __THERMO_KEYTEK_UTIL_H__

// Common version defines (application-specific defines are in application project)
#define COMPANY             "Thermo Fisher Scientific"
#define COMPANY_PATH        "Thermo"
#define PRODUCT_LINE        "KeyTek"
#define PRODUCT             "ECAT"
// Format values:  PRODUCT_LINE, PRODUCT, SUBPRODUCT, SUBPRODUCT_VERSION, <present year>, COMPANY
// <present year> needs to be created at run-time from __DATE__
// SUBPRODUCT and SUBPRODUCT_VERSION are in revstr.h for specific project
#define REVISION_FORMAT     "%s %s %s v%s\r\nCopyright (c) 1992 - %s %s"

// some convenience macros 
#define kabs(_expr) ((_expr) >= 0 ? (_expr) : (-(_expr)))
#define kmin(_expr1,_expr2) ((_expr1) <= (_expr2) ? (_expr1) : (_expr2))
#define kmax(_expr1,_expr2) ((_expr1) >= (_expr2) ? (_expr1) : (_expr2))

// Some defines that are used havely in PQFWare application and really there just 
// for the convenience
#define OK					1
#define YES					1
#define NO					0
#define ON					1
#define OFF					0
#define OR					||
#define AND					&&
#define EQ					==

#define INTEGER				int
//#define SHORT				short
#define CHARACTER			char
#define INTEGER4			long
#define INTEGER2			short
#define INTEGER1			char
#define UNSIGNED			unsigned
//#define DOUBLE				double

//typedef short FAR			*LPSHORT;
//typedef DOUBLE FAR		*LPDOUBLE;
//typedef BOOL FAR			*LPBOOL;

#define REGISTER			register
#define STRUCTURE			struct
#define UNION				union
#define STATIC				static
#define LOCAL				static
#define EXTERN				extern
#define LONGNULL			0L

// Global handle to the main window and dialog
extern HWND g_hMainWnd;
extern HWND g_hMainDlg;

// Debug output prototype function - outputs debug street to the current debug dump context.
// The function does nothing in Release build
extern void DBGOUT(LPSTR lpszMsg, ...);

// Global DC helper functions:
// C2S - client to screen conversion of RECT structure
// S2C - screen to client conversion of RECT structure
extern void C2S(HWND hParent, LPRECT pRect);
extern void S2C(HWND hParent, LPRECT pRect);

// Determines whether the specified address is valid
extern BOOL IsValidAddress(void* p, UINT uiCnt, BOOL bReadWrite/* = TRUE*/);

// Centers main window within the desktop's working area
extern void CenterWindow(HWND hWnd);

extern LPSTR get_revision_string();
extern LPTSTR get_app_data_file_path(LPSTR ini_file_name);

#endif // __THERMO_KEYTEK_UTIL_H__


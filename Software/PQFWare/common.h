/*******************************************************************

COMMON.H --	Common header file for PQFWARE source modules

HISTORY:

	6/20/95	JFL	Change rev. to 2.00C
	7/21/95	JFL	Change rev. to 2.00d
	8/2/95	JFL	Change rev. to 2.00e
	8/10/95	JFL	Change rev. to 2.00f
	9/1/95	JFL   	Change rev. to 2.00g
	11/1/95	JFL	Change rev. to 2.00h
	11/8/95	JFL	Change rev. to 2.1 for release
	11/1/96	JFL	Rev. 2.12
*******************************************************************/

#ifndef GLOBALVARIABLE
#ifdef IS_MAIN_PROGRAM_FILE
#define GLOBALVARIABLE
#else
#define GLOBALVARIABLE extern
#endif
#endif

#ifdef IS_MAIN_PROGRAM_FILE
#define GLOB_INITFALSE  = FALSE
#define GLOB_INITTRUE   = TRUE
#define GLOB_INITNEG1   = (-1)
#define GLOB_INIT0      = 0
#define GLOB_INIT0L     = 0L
#define GLOB_INITNULL   = NULL
#define GLOB_INIT1      = 1
#define GLOB_INIT2      = 2
#define GLOB_INITxffff  = 0xffff
#define GLOB_INITx7fff  = 0x7fff
#else
#define GLOB_INITFALSE  
#define GLOB_INITTRUE
#define GLOB_INITNEG1   
#define GLOB_INIT0
#define GLOB_INIT0L
#define GLOB_INITNULL
#define GLOB_INIT1
#define GLOB_INIT2
#define GLOB_INITxffff
#define GLOB_INITx7fff
#endif

#define COMMON_PATH_MAXLEN 200
#define COMMON_FILEEXT_MAXLEN 3
#define COMMON_FILENAME_MAXLEN 12

//GLOBALVARIABLE HANDLE hInst;	    /* current instance			     */

GLOBALVARIABLE CHARACTER common_message[512];
GLOBALVARIABLE CHARACTER common_format[512];
GLOBALVARIABLE CHARACTER common_caption[25];
GLOBALVARIABLE WORD common_curnid GLOB_INITNULL;
GLOBALVARIABLE CHARACTER common_filespec[COMMON_PATH_MAXLEN+1];

#ifdef IS_MAIN_PROGRAM_FILE
GLOBALVARIABLE CHARACTER common_spacechar = ' ';
GLOBALVARIABLE CHARACTER *common_sstr = "S";
GLOBALVARIABLE CHARACTER *common_rnstr = "\r\n";
GLOBALVARIABLE CHARACTER *common_backslashstr = "\\";
GLOBALVARIABLE CHARACTER *common_nullstr = "";
GLOBALVARIABLE CHARACTER *common_dev_ext = ".DEV";
GLOBALVARIABLE CHARACTER *common_test_ext = ".TST";
GLOBALVARIABLE CHARACTER *common_helpcaption   = "HELP";
GLOBALVARIABLE CHARACTER *common_dbgcaption    = "DEBUG";
GLOBALVARIABLE CHARACTER *common_errcaption    = "ERROR";
GLOBALVARIABLE CHARACTER *common_rescaption    = "RESULTS";
GLOBALVARIABLE CHARACTER *common_warncaption   = "WARNING";
GLOBALVARIABLE CHARACTER *common_msgcaption    = "MESSAGE";
GLOBALVARIABLE CHARACTER *common_failcaption   = "FAILURE";
GLOBALVARIABLE CHARACTER *common_opencaption  = "Open File";
GLOBALVARIABLE CHARACTER *common_memcaption   = "OUT OF MEMORY";
GLOBALVARIABLE CHARACTER *common_fatalcaption = "PROGRAM ERROR";
GLOBALVARIABLE CHARACTER *common_errcode[21] = {
    "(00)", "(01)", "(02)", "(03)", "(04)", "(05)",
    "(06)", "(07)", "(08)", "(09)", "(10)", "(11)", "(12)", "(13)",
    "(14)", "(15)", "(16)", "(17)", "(18)", "(19)", "(20)"
};
#else
GLOBALVARIABLE CHARACTER common_spacechar;
GLOBALVARIABLE CHARACTER *common_sstr;
GLOBALVARIABLE CHARACTER *common_rnstr;
GLOBALVARIABLE CHARACTER *common_backslashstr;
GLOBALVARIABLE CHARACTER *common_nullstr;
GLOBALVARIABLE CHARACTER *common_dev_ext;
GLOBALVARIABLE CHARACTER *common_test_ext;
GLOBALVARIABLE CHARACTER *common_helpcaption;
GLOBALVARIABLE CHARACTER *common_dbgcaption;
GLOBALVARIABLE CHARACTER *common_errcaption;
GLOBALVARIABLE CHARACTER *common_rescaption;
GLOBALVARIABLE CHARACTER *common_warncaption;
GLOBALVARIABLE CHARACTER *common_msgcaption;
GLOBALVARIABLE CHARACTER *common_failcaption;
GLOBALVARIABLE CHARACTER *common_opencaption;
GLOBALVARIABLE CHARACTER *common_memcaption;
GLOBALVARIABLE CHARACTER *common_fatalcaption;
GLOBALVARIABLE CHARACTER *common_errcode[21];
#endif

GLOBALVARIABLE CHARACTER common_bUpdating   GLOB_INITFALSE;

GLOBALVARIABLE HWND hWnd_pqf GLOB_INITNULL;
GLOBALVARIABLE HWND hDlg_pqf GLOB_INITNULL;
GLOBALVARIABLE FARPROC lpfnpqfDlg GLOB_INIT0L;

GLOBALVARIABLE BOOL common_bAutoPopup GLOB_INITFALSE;
GLOBALVARIABLE BOOL common_bAutoPopdown GLOB_INITFALSE;
GLOBALVARIABLE BOOL common_bMessageBox GLOB_INITFALSE;
GLOBALVARIABLE BOOL common_bEnableCheat GLOB_INITFALSE;
GLOBALVARIABLE BOOL bIsEP3Avail;

//GLOBALVARIABLE HWND common_hWndEcat GLOB_INITNULL; /* window handle to main ecat application */

LONG PqfWndProc (HWND hWnd, WORD msg, WORD wParam, LONG lParam);
BOOL PqfDlgProc (HWND hDlg, UNSIGNED message, WORD wParam, LONG lParam);

LONG PqfGraphWndProc (HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
LONG GraphWndProc (HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

LONG MyButtonWndProc (HWND hWnd, UNSIGNED message, WORD wParam, LONG lParam);
LONG MyEditWndProc (HWND hWnd, UNSIGNED message, WORD wParam, LONG lParam);
LONG ListBoxSubProc (HWND hWnd, UNSIGNED message, WORD wParam, LONG lParam);
BOOL InrushDlgProc (HWND hDlg, UNSIGNED message, WORD wParam, LONG lParam);
BOOL EcatCommDlgProc (HWND hDlg, UNSIGNED message, WORD wParam, LONG lParam);

GLOBALVARIABLE WORD     WM_UtilMessageBox GLOB_INIT0;

GLOBALVARIABLE HANDLE common_hComboseClassList GLOB_INITNULL;
GLOBALVARIABLE HANDLE common_hButtonList GLOB_INITNULL;

#define NUM_COMBOSECLASSES				6
#define CSECLASS_STEP                   0
#define CSECLASS_START                  1
#define CSECLASS_RMSILIM                2
#define CSECLASS_PEAKILIM               3
#define CSECLASS_DURATION_SEC           4
#define CSECLASS_DURATION_CYC           5

GLOBALVARIABLE SHORT common_comboseclass_id[NUM_COMBOSECLASSES];

/*******************************************************************
General include files
*******************************************************************/
/*
#include <string.h>
#include <ctype.h>
#include <io.h>
#include <stdio.h>
#include <stdarg.h>			// JFL, 6/8/95, add to try for 4.0 build

#include <lstring.h>
#include <boxlist.h>
#include <bits.h>
#include <combose.h>
#include <button.h>
#include <dlgutil.h>
#include <util.h>

// now include files from Paul's SLL 

#include <about.h>
#include <calinfo.h>
#include <comm.h>
#include <commi.h>
#include <ilock.h>
#define PQFSTATUS_DEFINED
#include <kt_comm.h>
#include <dlgctl.h>

#include "sim.h"

// now my include files 

#include "objects.h"
#include "ecatipc.h"
#include "testmgr.h"
#include "utilk.h"
*/

#include "..\core\comm.h"
#include "..\core\commi.h"
#include "..\core\ilock.h"
#define PQFSTATUS_DEFINED
#include "..\core\kt_comm.h"
#include "..\core\dlgctl.h"
#include "testmgr.h"
#include "objects.h"
#include "pqf.h"
#include "test.h"

GLOBALVARIABLE PQFDIALOGSETUP common_pqfdlgsetup;

DWORD PqfSetGraph(
	HANDLE hWnd,		/* handle to display, NOT CALLER's hWnd */
    LPPQFSTEP lpStepList,
    SHORT     nPickStep
);
DWORD PqfSetGraph(HWND hMainDlg, LONG nPickStep);
VOID PqfGraphSetFixedView (HWND hWnd, DWORD dwFixedViewDegrees);
VOID PqfGraphPickStep (HWND hWnd, LONG stepnum);
VOID PqfGraphEnableLeftMouseZooming (HWND hWnd, BOOL bEnable);
VOID PqfGraphTestInProgress (HWND hWnd, BOOL bInProgress);

extern HINSTANCE GetInst();

/* END OF COMMON INCLUSIONS */


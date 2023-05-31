/////////////////////////////////////////////////////////////////////
// PqfMainDlg.cpp: implementation of the CPqfMainDlg class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "PqfMainDlg.h"
#include "PqfApp.h"
#include "..\core\globals.h"
#include "..\core\calinfo.h"
#include "..\core\kt_util2.h"
#include "..\core\message.h"
#include <errno.h>

// Min/Max values per duration unit (see help file for more info)
#define DUR_MIN		0
#define DUR_MAX		1
#define DUR_DEF		2
static const int st_constDur[PQF_NUM_DURATION_UNITS][3] = 
{
	//min	max		default
	{1,		30059,	10},			// m:s
	{0,		999,	100},			// sec
	{0,		9999,	600},			// cyc
};

// Min/max and default values used in paramater controls
#define PQF_LEVEL_DEF		1		// Short	
#define PQF_START_MIN		0
#define PQF_START_MAX		359
#define PQF_START_DEF		90
#define PQF_RMSI_MIN		0	
#define PQF_RMSI_MAX		320
#define PQF_RMSI_DEF		100
#define PQF_PEAKI_MIN		0
#define PQF_PEAKI_MAX		500
#define PQF_PEAKI_DEF		100

typedef struct tagTESTSTEPINFO
{
    SHORT tsi_stepi;			/* step number in PQFSTEP list */
    DWORD tsi_dwDegreesLeft;	/* time left, in degrees */
	tagTESTSTEPINFO::tagTESTSTEPINFO() {memset(this, 0, sizeof(tagTESTSTEPINFO));}
} TESTSTEPINFO, FAR *LPTESTSTEPINFO;

static LPTESTSTEPINFO st_lpTestStepInfo = NULL;
static LPPQFLIST st_lpPqfTestPacketList = NULL;

static BOOL pqfdlg_bListBoxLeft;
static PQFSTEP PqfStep;
static BOOL pqfdlg_bStandby = FALSE;
static INTEGER testlen = 0;

static WORD nLineVoltage = -1 ;
static WORD nFrequency   = -1 ;
static WORD nRMS         = -1 ;
static WORD nPeak        = -1 ;

//////////////////////////////////////////////////////////////////////
// PQF INI file support
//////////////////////////////////////////////////////////////////////
const LPCSTR INI_LINE_PARAM_SECTION =	"Line Parameters";
const LPCSTR INI_LPS_VOLTAGE =			"Voltage";
const LPCSTR INI_LPS_FREQ =				"Frequency";

const LPCSTR INI_LPS_VOLTAGE_110 =		"110";
const LPCSTR INI_LPS_VOLTAGE_220 =		"220";

const LPCSTR INI_LPS_FREQ_50 =			"50";
const LPCSTR INI_LPS_FREQ_60 =			"60";

//////////////////////////////////////////////////////////////////////
// PQF test plan files support
//////////////////////////////////////////////////////////////////////

// Since PQF files are binaries, accomodate for cookie and version number
#define PQF_FILE_COOKIE						(DWORD) 0x57465150	// PQFW
#define PQF_FILE_VERSION					(DWORD) 200			// 2.00

// File headers alligned on the byte boundary
#pragma pack(push)
#pragma pack(1)
// New style file header
typedef struct tagPQFFileHeader
{
	DWORD dwCookie;
	DWORD dwVersion;
	tagPQFFileHeader() {dwCookie = PQF_FILE_COOKIE; dwVersion = PQF_FILE_VERSION;}
} PQF_FILE_HDR, *LPPQF_FILE_HDR;

// Old style file header
#define PQFFILEHEADER_NUM_SPARES			20
typedef struct tagRev101FileHeader
{
	short file_version;
	short file_revision;
	short file_pqf_size;
	short file_pqf_numsteps;
	short file_spares[PQFFILEHEADER_NUM_SPARES];
} REV_101_FILE_HDR, *LPREV_101_FILE_HDR;
#pragma pack(pop)

//////////////////////////////////////////////////////////////////////
// Simulation mode degree-per-step value, say, .25 seconds
//////////////////////////////////////////////////////////////////////
#define MAX_DEGREES_PER_STEP 5400

//////////////////////////////////////////////////////////////////////
// Global functions related to the main dialog operations
//////////////////////////////////////////////////////////////////////

/**************************************************************************** 
**                                                                         **
**     FUNCTION: _lritoa (INTEGER i, LPSTR a, INTEGER minchars,            **
**                                          CHARACTER fillchar)            **
**                                                                         **
**     PURPOSE:  itoa but with LPSTR instead of PSTR and can spec min      **
**                     number of characters                                **
**                                                                         **
****************************************************************************/ 

static LPSTR _lritoa (INTEGER i, LPSTR a, INTEGER minchars, CHARACTER fillchar)
{
    INTEGER len, tmplen;
    CHARACTER tmps[50];

    if (i < 0) {
        a[0] = '-';
        i = (-i);
        len = 1;
    }
    else
        len = 0;

    if (i > 0) {
        tmplen = 0;
        while (i) {
            tmps[tmplen] = (CHARACTER) ((i%10) + (INTEGER) '0');
            i = i / 10;
            tmplen++;
        }
    }
    else {
        tmps[0] = '0';
        tmplen = 1;
    }

    minchars = minchars - tmplen;
    for (; len<minchars; len++)
        a[len] = fillchar;

    while (tmplen) {
        tmplen-- ;
        a[len] = tmps[tmplen];
        len++;
    }
    a[len] = '\0';
    return (a);
} /* _lritoa */

//////////////////////////////////////////////////////////////////////
static void PqfPadSpaces(LPSTR pBuf, int nLength)
{
	char szSpace [2] ;
	szSpace [0] = VK_SPACE ;
	szSpace [1] = 0 ;

	int nBufLen = lstrlen (pBuf) ;

	if (nBufLen >= nLength)
		return ;
	else
	{
		for (int i = 0 ; i < nLength - nBufLen ; i ++)
			lstrcat (pBuf, szSpace) ;
	}
}

//////////////////////////////////////////////////////////////////////
static void DlgShowControl (HWND hDlg, WORD idd, BOOL bShow)
{
    HWND hCtrl;

    hCtrl = GetDlgItem (hDlg, idd);
    if (!hCtrl)
        return;
    if (bShow) {
        EnableWindow (hCtrl, TRUE);
        ShowWindow (hCtrl, SW_SHOWNORMAL);
    }
    else {
        ShowWindow (hCtrl, SW_HIDE);
        EnableWindow (hCtrl, FALSE);
    }
}

//////////////////////////////////////////////////////////////////////
void PqfShowUploadDialog(BOOL bShow)
{
	static HWND st_hUploadDlg = NULL;
	if (st_hUploadDlg == NULL)
		st_hUploadDlg = CreateDialog(GetInst(), MAKEINTRESOURCE(IDD_TEST_UPLOAD), g_hMainWnd, (DLGPROC) DefWindowProc);

	if (st_hUploadDlg != NULL)
	{
		if (bShow)
		{
			EnableWindow(g_hMainWnd, FALSE);
			ShowWindow(st_hUploadDlg, SW_SHOW);
			UpdateWindow(st_hUploadDlg);
		}
		else
		{
			EnableWindow(g_hMainWnd, TRUE);
			DestroyWindow(st_hUploadDlg);
			UpdateWindow(g_hMainWnd);
			st_hUploadDlg = NULL;
		}
	}
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CPqfMainDlg::CPqfMainDlg()
{
	// Set default values
	m_iStartPhase = PQF_START_DEF;
	for (int i = 0; i < PQF_NUM_DURATION_UNITS; i++)
		m_arrDur[i] = st_constDur[i][DUR_DEF];
	m_iDurUnits = PQF_DURATION_UNITS_SEC;
	m_iRmsI = PQF_RMSI_DEF;			// 10.0 A
	m_iPeakI = PQF_PEAKI_DEF;		// 100 A
	m_byPhase = 1;					// L1
	m_bRepeatTest = FALSE;
	m_bQualifying = FALSE;
}

//////////////////////////////////////////////////////////////////////
CPqfMainDlg::~CPqfMainDlg()
{
	// Clean up pointers
	DeleteAllSteps(FALSE);
}

//////////////////////////////////////////////////////////////////////
// NOTE: Return 0 if the message has been processed, 1 for further 
//		 processing by the system
//////////////////////////////////////////////////////////////////////
int CPqfMainDlg::OnCmdMsg(UINT uiNotify, UINT uiCtrlId, HWND hwndCtl)
{
    LONG retval;
	SHORT eut_status;
	switch (uiCtrlId)
	{
	case IDD_PQF_INSERT:
		InsertStep();
		break;

	case IDD_PQF_REPLACE:
		ReplaceStep();
		break;

	case IDD_PQF_COPY:
		CopyStep();
		break;

	case IDD_PQF_DELETE:
		DeleteStep();
		break;

	case IDD_PQF_DELETE_ALL:
		DeleteAllSteps(TRUE);
		break;

	case IDC_BUT_LIST_UP:
		SwapListItems(TRUE);
		break;

	case IDC_BUT_LIST_DN:
		SwapListItems(FALSE);
		break;

	case IDD_PQF_GRAPH:
		// Our graph control will send notification messages in form of WM_COMMAND. If
		// notification code is LBN_SELCHANGE, then we'll have to select the corresponding
		// test item in the list view control and ensure it's visibility. Selected test index
		// is sent as lParam (hwndCtl in case of OnCmdMsg())
		if (uiNotify == LBN_SELCHANGE)
		{
			SelectListItem((int) hwndCtl);
			SendItemMsg(IDD_PQF_LIST, LVM_ENSUREVISIBLE, (WPARAM) hwndCtl, 0);						
		}
		break;

	case IDD_PQF_RUN:
		RunTest();
		break;

	case IDD_PQF_STOP:
		StopTest();
		break;

	case IDC_BUT_QUALIFY:
		Qualify();
		break;

	case IDD_PQF_EUTPOWER:
		if (SIMULATION)
		{
			WPARAM wpBmp; 
			if (testmgr_eut_status == TESTMGR_EUT_RDY)
			{
				testmgr_eut_status = TESTMGR_EUT_OFF;
				wpBmp = 0;
			}
			else
			{
				testmgr_eut_status = TESTMGR_EUT_RDY;
				wpBmp = 1;
			}

			SendDlgItemMessage(hDlg_pqf, IDD_PQF_EUTPOWER, BM_SETCHECK, wpBmp, 0L);
			break;
		}

		// Real stuff here
		if (!testmgr_hWndComm)
			break;

		eut_status = (short) testmgr_eut_status;
		if (!TestmgrReadStatus(GetHWND(), TESTMGR_CALLER_EUTPOWER)) 
		{
			ForceClosure();
			break;
		}

		if (eut_status != testmgr_eut_status)
			break;

		switch (testmgr_eut_status) 
		{
		case TESTMGR_EUT_OFF:
			//DBGOUT("TESTMGR_EUT_OFF\n");
			SendPqfIdleCmnd();
			retval = SendMessage (testmgr_hWndComm, KT_EUT_ON, 0, 0L);
			if (retval)
				break;

			SendDlgItemMessage(hDlg_pqf, IDD_PQF_EUTPOWER, BM_SETCHECK, 1/*RDY*/, 0L);
			break;

		case TESTMGR_EUT_RDY:
			//DBGOUT("TESTMGR_EUT_RDY\n");
			SendMessage (testmgr_hWndComm, KT_EUT_OFF, 0, 0L);
			SendDlgItemMessage(hDlg_pqf, IDD_PQF_EUTPOWER, BM_SETCHECK, 0/*OFF*/, 0L);
			break;

		case TESTMGR_EUT_ON:
			//DBGOUT("TESTMGR_EUT_ON\n");
			testmgr_eut_status = TESTMGR_EUT_OFF;
			SendDlgItemMessage(hDlg_pqf, IDD_PQF_EUTPOWER, BM_SETCHECK, 0/*OFF*/, 0L);
			if ((test_status EQ TEST_STATUS_TESTING) OR (test_status EQ TEST_STATUS_PAUSED)) 
			{
				SendMessage (testmgr_hWndComm, KT_PQF_STOP, 0, 0L);
				SendMessage (testmgr_hWndComm, KT_EUT_OFF, 0, 0L);
				PqfDlgTestCompleted();
			}
			else
				SendMessage (testmgr_hWndComm, KT_EUT_OFF, 0, 0L);
			break;
		} /* switch on new EUT status */
		break; /* IDD_PQF_EUTPOWER */

	// If it's one of the check- or radiobuttons that affect the test, 
	// set modified flag
	case IDD_PQF_TESTREPEAT:
		changed_since_save = TRUE;
		break;

	case IDD_PQF_PHASEL1:
	case IDD_PQF_PHASEL2:
	case IDD_PQF_PHASEL3:
		m_byPhase = GetPhase();
		//SendPhaseCmnd();
		changed_since_save = TRUE;
		break;

	case IDC_RDB_OPEN:
	case IDC_RDB_HUNDRED:
		changed_since_save = TRUE;
		pqf_lpStepListhdr.hdr_standby_level = GetStandbyLevel();
		SendPqfIdleCmnd();
		break;

	default:
		return CEcatMainDlg::OnCmdMsg(uiNotify, uiCtrlId, hwndCtl);
	}

	return 0;
}

//////////////////////////////////////////////////////////////////////
void CPqfMainDlg::FillTestStep(PQFSTEP& pqfStep)
{
	int iSel = SendItemMsg(IDC_COMBO_LEVEL, CB_GETCURSEL, 0, 0);
	if (iSel == CB_ERR)
		iSel = 0;
	pqfStep.step_level = iSel;
    pqfStep.step_start = m_iStartPhase;
    pqfStep.step_duration = m_arrDur[m_iDurUnits];
	if (m_iDurUnits == PQF_DURATION_UNITS_SEC)
		pqfStep.step_duration *= 10;
    pqfStep.step_duration_units = m_iDurUnits;
    pqfStep.step_rms_ilim = m_iRmsI;
    pqfStep.step_peak_ilim = m_iPeakI;
}

//////////////////////////////////////////////////////////////////////
void CPqfMainDlg::TestStep2ListItem(int iListItem, const PQFSTEP& pqfStep)
{
	if (iListItem == -1)
		return;

	char buf[28];
	LVITEM lvItem;
	memset(&lvItem, 0, sizeof(lvItem));
	lvItem.mask = LVIF_TEXT;
	lvItem.iItem = iListItem;
	lvItem.pszText = buf;
	for (lvItem.iSubItem = 0; lvItem.iSubItem < 6; lvItem.iSubItem++)
	{
		switch (lvItem.iSubItem)
		{
		case 0:
			// step number
			sprintf(buf, "%d", pqfStep.step_num);
			break;

		case 1:
			// level
			strcpy(buf, pqf_level_display[pqfStep.step_level]);
			break;

		case 2:
			// start phase
			sprintf(buf, "%d dg", pqfStep.step_start);
			break;

		case 3:
			// duration
			switch (pqfStep.step_duration_units)
			{
			case PQF_DURATION_UNITS_MIN:
				sprintf(buf, "%3d:%02d m:s", pqfStep.step_duration / 60, pqfStep.step_duration % 60);
				break;

			case PQF_DURATION_UNITS_SEC:
				sprintf(buf, "%.1f sec", (double) (pqfStep.step_duration / 100.0));
				break;

			case PQF_DURATION_UNITS_CYC:
				sprintf(buf, "%d cyc", pqfStep.step_duration);
				break;
			}
			break;

		case 4:
			// rms-i
			sprintf(buf, "%.1f A", (double) (pqfStep.step_rms_ilim / 10.0));
			break;

		case 5:
			// peak-i
			sprintf(buf, "%d A", pqfStep.step_peak_ilim);
			break;
		}
		if (SendItemMsg(IDD_PQF_LIST, LVM_SETITEM, 0, (LPARAM) &lvItem) == -1)
			return;
	}

}

//////////////////////////////////////////////////////////////////////
BOOL CPqfMainDlg::IsTestTooLong(LPPQFSTEP pPqfStep/* = NULL*/)
{
    if ((PQF_MAX_DURATION_DEGREES - pqf_lpStepListhdr.hdr_dwTotDegrees) < PqfUtilStep2Degrees(pPqfStep)) 
	{
        UtilMessageBox(hDlg_pqf, "This step will cause the test to be too long;\015\012The longest test allowed is 50:59:59.99\015\012Step cannot be inserted into list", common_errcaption, MB_OK | MB_ICONHAND | MB_ICONEXCLAMATION);
        return TRUE;
    }

	return FALSE;
}

//////////////////////////////////////////////////////////////////////
void CPqfMainDlg::InsertStep()
{
	// Create new step based on the data in the parameter boxes
	UpdateData(TRUE);

    // Make sure the test will not run for too long
	if (IsTestTooLong())
		return;

	// Find selected item. If there are no items in the list, the function will return -1.
	int iSel = GetSelItem();
	int iItemIndx = iSel + 1;

	// Allocate PQFSTEP structure
	LPPQFSTEP pPqfStep = new PQFSTEP;
	if (pPqfStep == NULL)
	{
		ShowOutOfMemoryDlg();
		return;
	}

	// Set the parameters and attach them to the listview control's entry
	pPqfStep->step_num = iItemIndx + 1;
	FillTestStep(*pPqfStep);

	LVITEM lvItem;
	memset(&lvItem, 0, sizeof(lvItem));
	lvItem.mask = LVIF_PARAM | LVIF_STATE;
	lvItem.iItem = iItemIndx;
	lvItem.stateMask = (UINT) -1;
	lvItem.state = LVIS_SELECTED;
	lvItem.lParam = (LPARAM) pPqfStep;
	SendItemMsg(IDD_PQF_LIST, LVM_INSERTITEM, 0, (LPARAM) &lvItem);

	TestStep2ListItem(iItemIndx, *pPqfStep);
	PqfSetTotalTime(pqf_lpStepListhdr.hdr_dwTotDegrees + PqfUtilStep2Degrees(pPqfStep));

	// See, if we need to adjust step numbers
	char buf[28];
	lvItem.mask = LVIF_TEXT;
	lvItem.iSubItem = 0;
	lvItem.pszText = buf;
	for (lvItem.iItem = (iItemIndx + 1); lvItem.iItem < GetListItemCnt(); lvItem.iItem++)
	{
		// Update it's parameters
		pPqfStep = GetListItemData(lvItem.iItem);
		if (pPqfStep == NULL)
			continue;
		pPqfStep->step_num++;
		sprintf(buf, "%d", pPqfStep->step_num);
		SendItemMsg(IDD_PQF_LIST, LVM_SETITEM, 0, (LPARAM) &lvItem);
	}

	PqfSetGraph(GetHWND(), iItemIndx);
	changed_since_save = TRUE;
}

//////////////////////////////////////////////////////////////////////
void CPqfMainDlg::ReplaceStep()
{
	UpdateData(TRUE);

    // Make sure the test will not run for too long
	if (IsTestTooLong())
		return;

	int iSel = GetSelItem();
	LPPQFSTEP pPqfStep = GetListItemData(iSel);
	if (pPqfStep)
	{
		pqf_lpStepListhdr.hdr_dwTotDegrees -= PqfUtilStep2Degrees(pPqfStep);
		FillTestStep(*pPqfStep);
		PqfSetTotalTime(pqf_lpStepListhdr.hdr_dwTotDegrees + PqfUtilStep2Degrees(pPqfStep));
		TestStep2ListItem(iSel, *pPqfStep);
	}

	PqfSetGraph(GetHWND(), iSel);
	changed_since_save = TRUE;
}

//////////////////////////////////////////////////////////////////////
void CPqfMainDlg::CopyStep()
{
	int iSel = GetSelItem();
	LPPQFSTEP pPqfStep = GetListItemData(iSel);
	if (pPqfStep)
	{
		// Make sure the test will not run for too long
		if (IsTestTooLong(pPqfStep))
			return;

		// Allocate PQFSTEP structure
		LPPQFSTEP pPqfStepNew = new PQFSTEP;
		if (pPqfStepNew == NULL)
		{
			ShowOutOfMemoryDlg();
			return;
		}

		memcpy(pPqfStepNew, pPqfStep, sizeof(PQFSTEP));
		pPqfStepNew->step_num = iSel + 1;

		LVITEM lvItem;
		memset(&lvItem, 0, sizeof(lvItem));
		lvItem.mask = LVIF_PARAM | LVIF_STATE;
		lvItem.iItem = pPqfStepNew->step_num;
		lvItem.stateMask = (UINT) -1;
		lvItem.state = LVIS_SELECTED;
		lvItem.lParam = (LPARAM) pPqfStepNew;
		SendItemMsg(IDD_PQF_LIST, LVM_INSERTITEM, 0, (LPARAM) &lvItem);

		TestStep2ListItem(pPqfStepNew->step_num, *pPqfStepNew);
		PqfSetTotalTime(pqf_lpStepListhdr.hdr_dwTotDegrees + PqfUtilStep2Degrees(pPqfStepNew));

		// See, if we need to adjust step numbers
		char buf[28];
		lvItem.mask = LVIF_TEXT;
		lvItem.iSubItem = 0;
		lvItem.pszText = buf;
		for (lvItem.iItem = pPqfStepNew->step_num; lvItem.iItem < GetListItemCnt(); lvItem.iItem++)
		{
			// Update it's parameters
			pPqfStep = GetListItemData(lvItem.iItem);
			if (pPqfStep == NULL)
				continue;
			pPqfStep->step_num++;
			sprintf(buf, "%d", pPqfStep->step_num);
			SendItemMsg(IDD_PQF_LIST, LVM_SETITEM, 0, (LPARAM) &lvItem);
		}

		PqfSetGraph(GetHWND(), pPqfStepNew->step_num);
		changed_since_save = TRUE;
	}
}

//////////////////////////////////////////////////////////////////////
void CPqfMainDlg::DeleteStep()
{
	if (GetListItemCnt() == 0)
		return;

	int iSel = GetSelItem();
	if (iSel == -1)
		return;

	if (MessageBox(GetHWND(), "Are you sure?", "Delete Selected Step", MB_YESNO | MB_ICONQUESTION) == IDYES)
	{
		LPPQFSTEP pPqfStep = GetListItemData((WPARAM) iSel);
		SendItemMsg(IDD_PQF_LIST, LVM_DELETEITEM, (WPARAM) iSel, 0); 

		int iItemCnt = GetListItemCnt();
		if (iItemCnt > 0)
		{
			PqfSetTotalTime(pqf_lpStepListhdr.hdr_dwTotDegrees - PqfUtilStep2Degrees(pPqfStep));

			// See, if we need to adjust step numbers
			char buf[28];
			LVITEM lvItem;
			memset(&lvItem, 0, sizeof(lvItem));
			lvItem.mask = LVIF_TEXT;
			lvItem.iSubItem = 0;
			lvItem.pszText = buf;
			for (lvItem.iItem = iSel; lvItem.iItem < iItemCnt; lvItem.iItem++)
			{
				// Update parameters
				pPqfStep = GetListItemData(lvItem.iItem);
				if (pPqfStep == NULL)
					continue;
				pPqfStep->step_num--;
				sprintf(buf, "%d", pPqfStep->step_num);
				SendItemMsg(IDD_PQF_LIST, LVM_SETITEM, 0, (LPARAM) &lvItem);
			}

			iSel = 0;
			SelectListItem(iSel);
			PqfSetGraph(GetHWND(), iSel);
		}
		else
		{
			PqfSetTotalTime(0);
			PqfSetGraph(GetHWND(), -1);
		}

		changed_since_save = TRUE;
	}
}

//////////////////////////////////////////////////////////////////////
void CPqfMainDlg::DeleteAllSteps(BOOL bAsk/* = FALSE*/)
{
	if (GetListItemCnt() == 0)
		return;

	// If bAsk is FALSE it's the final clean-up before exiting the app or
	// we called from DeleteContext()
	if (bAsk && (MessageBox(GetHWND(), "Are you sure?", "Delete All Steps", MB_YESNO | MB_ICONQUESTION) == IDNO))
		return;

	for (int i = 0; i < GetListItemCnt(); i++)
	{
		LPPQFSTEP pPqfStep = GetListItemData(i);
		if (IsValidAddress(pPqfStep, sizeof(PQFSTEP), FALSE))
			delete pPqfStep;
	}

	SendItemMsg(IDD_PQF_LIST, LVM_DELETEALLITEMS, 0, 0);
	if (bAsk)
	{
		PqfSetGraph(GetHWND(), -1);
		PqfSetTotalTime(0);
		changed_since_save = TRUE;
	}
}

//////////////////////////////////////////////////////////////////////
void CPqfMainDlg::SwapListItems(BOOL bMoveUp)
{
	int iSel = GetSelItem();
	if (iSel == -1)
		return;

	int iNext = -1;
	if (bMoveUp)
	{
		if (iSel == 0)
			return;
		iNext = iSel - 1;
	}
	else
	{
		if (iSel == (GetListItemCnt() - 1))
			return;
		iNext = iSel + 1;
	}

	LPPQFSTEP pSelStep = GetListItemData(iSel);
	LPPQFSTEP pNextStep = GetListItemData(iNext);
	int iStepNum = pSelStep->step_num;
	pSelStep->step_num = pNextStep->step_num;
	pNextStep->step_num = iStepNum;
	TestStep2ListItem(iSel, *pNextStep);
	SetListItemData(iSel, pNextStep);
	TestStep2ListItem(iNext, *pSelStep);
	SetListItemData(iNext, pSelStep);
	SelectListItem(iNext);
	PqfSetGraph(GetHWND(), iNext);
	changed_since_save = TRUE;
}

//////////////////////////////////////////////////////////////////////
void CPqfMainDlg::SelectListItem(int iItem)
{
	if ((iItem == -1) || (GetListItemCnt() == 0))
		return;

	LVITEM lvItem;
	memset(&lvItem, 0, sizeof(lvItem));
	lvItem.mask = LVIF_STATE;
	lvItem.iItem = iItem;
	lvItem.stateMask = (UINT) -1;
	lvItem.state = LVIS_SELECTED;
	SendItemMsg(IDD_PQF_LIST, LVM_SETITEM, 0, (LPARAM) &lvItem);

	UpdateTestCtrls();
}

//////////////////////////////////////////////////////////////////////
void CPqfMainDlg::SetListItemData(int iItem, LPPQFSTEP pStep)
{
	if ((iItem == -1) || (pStep == NULL))
		return;

	LVITEM lvItem;
	memset(&lvItem, 0, sizeof(lvItem));
	lvItem.mask = LVIF_PARAM;
	lvItem.iItem = iItem;
	lvItem.lParam = (LPARAM) pStep;
	SendItemMsg(IDD_PQF_LIST, LVM_SETITEM, 0, (LPARAM) &lvItem);
}

//////////////////////////////////////////////////////////////////////
LPPQFSTEP CPqfMainDlg::GetListItemData(int iItem)
{
	if (iItem == -1)
		return NULL;

	LVITEM lvItem;
	memset(&lvItem, 0, sizeof(lvItem));
	lvItem.mask = LVIF_PARAM;
	lvItem.iItem = iItem;
	SendItemMsg(IDD_PQF_LIST, LVM_GETITEM, 0, (LPARAM) &lvItem);
	return (LPPQFSTEP) lvItem.lParam;
}

//////////////////////////////////////////////////////////////////////
int CPqfMainDlg::GetListItemCnt()
{
	return (int) SendItemMsg(IDD_PQF_LIST, LVM_GETITEMCOUNT, 0, 0);
}

//////////////////////////////////////////////////////////////////////
int CPqfMainDlg::GetNextListItem(int iItem, int iFlags)
{
	return (int) SendItemMsg(IDD_PQF_LIST, LVM_GETNEXTITEM, (WPARAM) iItem, MAKELPARAM(iFlags, 0));
}

//////////////////////////////////////////////////////////////////////
int CPqfMainDlg::GetSelItem()
{
	// In no items in the list, return -1;
	if (GetListItemCnt() == 0)
		return -1;

	return (int) GetNextListItem(-1, LVNI_SELECTED);
}

//////////////////////////////////////////////////////////////////////
void CPqfMainDlg::InsertColumn(int nCol, LPTSTR lpszHeading, int nWidth, int nFormat/* = LVCFMT_LEFT*/)
{
	LVCOLUMN lvColumn;
	memset(&lvColumn, 0, sizeof(lvColumn));
	lvColumn.mask = LVCF_FMT | LVCF_TEXT | LVCF_WIDTH;
	lvColumn.fmt = nFormat;
	lvColumn.cx = nWidth;
	lvColumn.pszText = lpszHeading;
	int i = SendItemMsg(IDD_PQF_LIST, LVM_INSERTCOLUMN, (WPARAM) nCol, (LPARAM) &lvColumn);
}

//////////////////////////////////////////////////////////////////////
BOOL CPqfMainDlg::OnInitDialog(HWND hWnd)
{
	// Read INI settings for the nominal voltage and frequency line values and
	// initialize pqf_lpStepListhdr structure
	pqf_lpStepListhdr.hdr_numsteps = 0;
	pqf_lpStepListhdr.hdr_standby_level = PQF_LEVEL_100;

	char profile_buff[MAX_PATH + 1];
	memset(profile_buff, 0, sizeof(profile_buff));
	
    GetPrivateProfileString(INI_LINE_PARAM_SECTION, INI_LPS_VOLTAGE, INI_LPS_VOLTAGE_110,
        profile_buff, MAX_PATH, get_app_data_file_path(INI_NAME));
	SWITCH(profile_buff)

		CASEi(INI_LPS_VOLTAGE_220)
			pqf_lpStepListhdr.hdr_line_voltage = 220;

		DEFAULT
			pqf_lpStepListhdr.hdr_line_voltage = 110;

	ENDSWITCH

	GetPrivateProfileString(INI_LINE_PARAM_SECTION, INI_LPS_FREQ, INI_LPS_FREQ_60, profile_buff, MAX_PATH, get_app_data_file_path(INI_NAME));
	SWITCH(profile_buff)

		CASEi(INI_LPS_FREQ_50)
			pqf_lpStepListhdr.hdr_line_freq = 50;

		DEFAULT
			pqf_lpStepListhdr.hdr_line_freq = 60;

	ENDSWITCH


	pqf_dwCyclesPerSecond = pqf_lpStepListhdr.hdr_line_freq;
	pqf_dwDegreesPerSecond = pqf_dwCyclesPerSecond * PQF_DEGREES_PER_CYCLE;
	pqf_dwDegreesPerMinute = 60 * pqf_dwDegreesPerSecond;
	pqf_dwDegreesPerHour = 60 * pqf_dwDegreesPerMinute;

	// Call the base class function to assign m_hWnd
	CEcatMainDlg::OnInitDialog(hWnd);

	// Some special initialization to make the app backward compatible with the old code
	hDlg_pqf = hWnd;
	obj_hbr_white = CreateSolidBrush(GetSysColor(COLOR_WINDOW));
    obj_hbr_yellow = yBrush;
    obj_hbr_palegreen = CreateSolidBrush (RGBpalegreen);

	// Subclass the buttons
	// EXIT
	UINT arrIds[MAX_BTN_STATES];
	arrIds[0] = IDD_ECAT_EXIT;
	m_btnExit.Subclass(hWnd, IDD_ECAT_EXIT, GetInst(), arrIds, 1);

	// STOP			
	arrIds[0] = IDD_RAW_STOP;
	arrIds[1] = IDD_RAW_STOP_ON;
	m_btnStop.Subclass(hWnd, IDD_PQF_STOP, GetInst(), arrIds, 2);

	// RUN
	arrIds[0] = IDD_RUN;
	arrIds[1] = IDD_RUN_ON;
	m_btnRun.Subclass(hWnd, IDD_PQF_RUN, GetInst(), arrIds, 2);
	
	// EUT
	arrIds[0] = IDD_EUT_POWER;
	arrIds[1] = IDD_EUT_POWER_RDY;
	arrIds[2] = IDD_EUT_POWER_ON;
	m_btnEut.Subclass(hWnd, IDD_PQF_EUTPOWER, GetInst(), arrIds, 3);

	// Update icons on the Up/Down list buttons
	HICON hIcon = (HICON) ::LoadImage(
		GetInst(), 
		MAKEINTRESOURCE(IDI_ARR_UP),
		IMAGE_ICON,
		12,
		12,
		LR_DEFAULTCOLOR);
	SendItemMsg(IDC_BUT_LIST_UP, BM_SETIMAGE, IMAGE_ICON, (LPARAM) hIcon);

	hIcon = (HICON) ::LoadImage(
		GetInst(), 
		MAKEINTRESOURCE(IDI_ARR_DOWN),
		IMAGE_ICON,
		12,
		12,
		LR_DEFAULTCOLOR);
	SendItemMsg(IDC_BUT_LIST_DN, BM_SETIMAGE, IMAGE_ICON, (LPARAM) hIcon);

	// Initialize the list view control
	int iCol = 0;
	InsertColumn(iCol++, "Step", 55, LVCFMT_RIGHT);
	InsertColumn(iCol++, "Level", 62, LVCFMT_CENTER);
	InsertColumn(iCol++, "Start", 62, LVCFMT_CENTER);
	InsertColumn(iCol++, "Duration", 150, LVCFMT_CENTER);
	InsertColumn(iCol++, "RMS-I", 80, LVCFMT_LEFT);
	InsertColumn(iCol++, "Peak-I", 80, LVCFMT_LEFT);
	SendItemMsg(IDD_PQF_LIST, LVM_SETEXTENDEDLISTVIEWSTYLE, LVS_EX_FULLROWSELECT, LVS_EX_FULLROWSELECT);

	// Limit text in edit boxes
	SendItemMsg(IDD_PQF_STARTEDIT, EM_SETLIMITTEXT, (WPARAM) 3, 0);
	SendItemMsg(IDD_PQF_DURATIONEDIT, EM_SETLIMITTEXT, (WPARAM) 6, 0);
	SendItemMsg(IDD_PQF_RMSIEDIT, EM_SETLIMITTEXT, (WPARAM) 4, 0);
	SendItemMsg(IDD_PQF_PEAKIEDIT, EM_SETLIMITTEXT, (WPARAM) 3, 0);

	// Add data strings to the combo box
	for (int i = 0; i < NUM_PQF_LEVELS; i++)
		SendItemMsg(IDC_COMBO_LEVEL, CB_ADDSTRING, 0, (LPARAM) pqf_level_entry[i]);

	testmgr_rms_imax = PQF_RMSI_MAX;
	testmgr_peak_imax = PQF_PEAKI_MAX;

	// Start hidden window's message pump
	if (!SIMULATION)
	{
		DWORD dwPqfInfo = SendMessage (testmgr_hWndComm, KT_PQF_READ_DATA, 0, 0L);
		testmgr_rms_imax = LOWORD(dwPqfInfo);
		testmgr_peak_imax = HIWORD(dwPqfInfo);

		// Initialize ECAT with 100% power if EP3 installed. Default init - left same.  JFL 8/10/95
		SendPqfIdleCmnd();
	}

	// Set limits on parameter controls
	// Start [0-359 dg; 90 - default]
	SendItemMsg(IDC_SPIN_START, UDM_SETRANGE, 0, (LPARAM) MAKELONG((short) PQF_START_MAX, (short) PQF_START_MIN));

	// Duration [min/max/default depends on units selected]
	SendItemMsg(IDC_SPIN_DURATION, UDM_SETRANGE, 0, 
			(LPARAM) MAKELONG((short) st_constDur[m_iDurUnits][DUR_MAX], (short) st_constDur[m_iDurUnits][DUR_MIN]));
	//SendItemMsg(IDC_SPIN_DURATION, UDM_SETRANGE32, st_constDur[m_iDurUnits][DUR_MIN], st_constDur[m_iDurUnits][DUR_MAX]);

	// Duration units [1-3 units; 1 - default]
	SendItemMsg(IDC_SPIN_DUR_UNIT, UDM_SETRANGE, 0, (LPARAM) MAKELONG((short) PQF_DURATION_UNITS_CYC, (short) PQF_DURATION_UNITS_MIN));

	// RMS-I [0.0-32.0; 10.0 - default (320 total)]
	SendItemMsg(IDC_SPIN_RMSI, UDM_SETRANGE, 0, (LPARAM) MAKELONG((short) testmgr_rms_imax, (short) PQF_RMSI_MIN));

	// Peak-I [0-500 A; 100 - default]
	SendItemMsg(IDC_SPIN_PICKI, UDM_SETRANGE, 0, (LPARAM) MAKELONG((short) testmgr_peak_imax, (short) PQF_PEAKI_MIN));

	// Enable/Disable three phase and standby selection controls
	bIsEP3Avail = check_type();
	EnableWindow(GetDlgItem(IDD_PQF_PHASEL1), bIsEP3Avail);
	EnableWindow(GetDlgItem(IDD_PQF_PHASEL2), bIsEP3Avail);
	EnableWindow(GetDlgItem(IDD_PQF_PHASEL3), bIsEP3Avail);

	EnableWindow(GetDlgItem(IDC_RDB_OPEN), !bIsEP3Avail);
	EnableWindow(GetDlgItem(IDC_RDB_HUNDRED), !bIsEP3Avail);

	// Set line nominal values
	Int2Text(IDD_PQF_LINE_VNOMINAL, pqf_lpStepListhdr.hdr_line_voltage);
	Int2Text(IDD_PQF_LINE_FREQNOMINAL, pqf_lpStepListhdr.hdr_line_freq);

	// Stop button
	EnableWindow(GetDlgItem(IDD_PQF_STOP), FALSE);

	// make it a virgin start, or open the file, if supplied
	if (theApp.IsDdeFile())
	{
		strcpy(fnamebuf, theApp.GetDdeFileName());
		OpenFile();
	}
	else
		DoFileNew();

	Last_State = ILOCK_OK;
	This_State = ECAT_Ilock();

   // Start the timer
    if (!SIMULATION) 
	{
        if (!StartTimer(TIMER1, 500))
            MessageBox(hWnd, "Not Enough Timers", "Not Enough Timers", MB_OK | MB_ICONWARNING);
    }

	return TRUE;
}

//////////////////////////////////////////////////////////////////////
void CPqfMainDlg::OnDestroy()
{
	// Write back values to the INI file
	WritePrivateProfileString(INI_LINE_PARAM_SECTION, INI_LPS_VOLTAGE, 
		(pqf_lpStepListhdr.hdr_line_voltage == 220) ? INI_LPS_VOLTAGE_220 : INI_LPS_VOLTAGE_110, get_app_data_file_path(INI_NAME));
	WritePrivateProfileString(INI_LINE_PARAM_SECTION, INI_LPS_FREQ, 
		(pqf_lpStepListhdr.hdr_line_freq == 50) ? INI_LPS_FREQ_50 : INI_LPS_FREQ_60, get_app_data_file_path(INI_NAME));

	DeleteObject(obj_hbr_white);
    DeleteObject(obj_hbr_palegreen);
	StopTimer();
	CEcatMainDlg::OnDestroy();
}

//////////////////////////////////////////////////////////////////////
void CPqfMainDlg::OnTimer(UINT uiTimerId)
{
	if (uiTimerId == TIMER1)
	{
		if (testmgr_bSimulate) 
		{
			StopTimer();
			DBGOUT("in OnTimer, simulating.\n");
			SimulateStatusUpdate(FALSE);
			PqfTestNextStep();
			if (test_status != TEST_STATUS_DONE)
				StartTimer(TIMER1, 250);
		}
		else
		{
			if (testmgr_bGettingStatus)
				return;

			if (!TestmgrReadStatus(GetHWND(), TESTMGR_CALLER_TIMER)) 
				ForceClosure();
		}
	}
}

//////////////////////////////////////////////////////////////////////
void CPqfMainDlg::UpdateData(BOOL bSaveAndValidate)
{
	if (bSaveAndValidate)
	{
		// We are extracting values from the parameter controls
		int iVal;
		double dVal;
		const BYTE byStartOK = 0x01;
		const BYTE byDurOK = 0x02;
		const BYTE byRmsiOK = 0x04;
		const BYTE byPeakOK = 0x08;
		const BYTE byAllOK = 0x0F;
		BYTE byOK = byAllOK;
		
		///////////////////
		// START
		///////////////////
		if (Text2Int(IDD_PQF_STARTEDIT, iVal))
		{
			m_iStartPhase = iVal;

			// Make sure the value is within the limits
			if (m_iStartPhase > PQF_START_MAX)
			{
				m_iStartPhase = PQF_START_MAX;
				byOK &= ~byStartOK;
			}

			if (m_iStartPhase < PQF_START_MIN)
			{
				m_iStartPhase = PQF_START_MIN;
				byOK &= ~byStartOK;
			}
		}
		else
			byOK &= ~byStartOK;

		// Reset the text in the control back to what it was before if there was an error
		if ((byOK & byStartOK) == 0)
			Int2Text(IDD_PQF_STARTEDIT, m_iStartPhase);

		///////////////////
		// DURATION
		///////////////////
		if (!Text2Dur())
			byOK &= ~byDurOK;

		// Make sure the values are within the corresponding limits
		if (m_arrDur[m_iDurUnits] > st_constDur[m_iDurUnits][DUR_MAX])
		{
			m_arrDur[m_iDurUnits] = st_constDur[m_iDurUnits][DUR_MAX];
			byOK &= ~byDurOK;
		}

		if (m_arrDur[m_iDurUnits] < st_constDur[m_iDurUnits][DUR_MIN])
		{
			m_arrDur[m_iDurUnits] = st_constDur[m_iDurUnits][DUR_MIN];
			byOK &= ~byDurOK;
		}

		// Reset the text in the control back to what it was before if there was an error
		if ((byOK & byDurOK) == 0)
			Dur2Text();

		///////////////////
		// RMS-I
		///////////////////
		if (Text2Dbl(IDD_PQF_RMSIEDIT, dVal))
		{
			m_iRmsI = (int) (dVal * 10.0);

			// Make sure the value is within the limits
			if (m_iRmsI > testmgr_rms_imax)
			{
				m_iRmsI = testmgr_rms_imax;
				byOK &= ~byRmsiOK;
			}

			if (m_iRmsI < PQF_RMSI_MIN)
			{
				m_iRmsI = PQF_RMSI_MIN;
				byOK &= ~byRmsiOK;
			}
		}
		else
			byOK &= ~byRmsiOK;

		// Reset the text in the control back to what it was before if there was an error
		if ((byOK & byRmsiOK) == 0)
		{
			dVal = m_iRmsI / 10.0;
			Dbl2Text(IDD_PQF_RMSIEDIT, dVal);
		}

		///////////////////
		// Peak-I
		///////////////////
		if (Text2Int(IDD_PQF_PEAKIEDIT, iVal))
		{
			m_iPeakI = iVal;

			// Make sure the value is within the limits
			if (m_iPeakI > testmgr_peak_imax)
			{
				m_iPeakI = testmgr_peak_imax;
				byOK &= ~byPeakOK;
			}

			if (m_iPeakI < PQF_PEAKI_MIN)
			{
				m_iPeakI = PQF_PEAKI_MIN;
				byOK &= ~byPeakOK;
			}
		}
		else
			byOK &= ~byPeakOK;

		// Reset the text in the control back to what it was before if there was an error
		if ((byOK & byPeakOK) == 0)
			Int2Text(IDD_PQF_PEAKIEDIT, m_iPeakI);

		// Warn the user that we changed the values
		if (byOK != byAllOK)
			MessageBox(GetHWND(), "Some of the specified test step parameters are invalid and will be reset to the last known valid state. Press OK to continue.", "Invalid Parameters", MB_OK | MB_ICONINFORMATION);
	}
	else
	{
		// Select first item
		SendItemMsg(IDC_COMBO_LEVEL, CB_SETCURSEL, (WPARAM) PQF_LEVEL_DEF, 0);
		
		// Start [0-359 dg; 90 - default]
		SendItemMsg(IDC_SPIN_START, UDM_SETPOS, 0, (LPARAM) MAKELONG((short) m_iStartPhase, 0));

		// Duration [min/max/default depends on units selected]
		SendItemMsg(IDC_SPIN_DURATION, UDM_SETPOS, 0, (LPARAM) MAKELONG((short) m_arrDur[m_iDurUnits], 0));

		// Duration units [1-3 units; 1 - default]
		SendItemMsg(IDC_SPIN_DUR_UNIT, UDM_SETPOS, 0, (LPARAM) MAKELONG((short) m_iDurUnits, 0));

		// RMS-I [0.0-32.0; 10.0 - default (320 total)]
		SendItemMsg(IDC_SPIN_RMSI, UDM_SETPOS, 0, (LPARAM) MAKELONG((short) m_iRmsI, 0));

		// Peak-I [0-500 A; 100 - default]
		SendItemMsg(IDC_SPIN_PICKI, UDM_SETPOS, 0, (LPARAM) MAKELONG((short) m_iPeakI, 0));

		// Set the value of duration units
		SetItemText(IDC_DUR_UNITS_TEXT, pqf_duration_units[m_iDurUnits]);

		// Update the rest of the edit boxes with the valid values that we have
		Int2Text(IDD_PQF_STARTEDIT, m_iStartPhase);
		Dur2Text();
		double dVal = m_iRmsI / 10.0;
		Dbl2Text(IDD_PQF_RMSIEDIT, dVal);
		Int2Text(IDD_PQF_PEAKIEDIT, m_iPeakI);

		SetStandbyLevel(pqf_lpStepListhdr.hdr_standby_level);
		SetPhase(m_byPhase);
		SetRepeatFlag(m_bRepeatTest);

		PqfSetTotalTime(0);
		PqfSetTimeLeft(0, TRUE);
	}
}

//////////////////////////////////////////////////////////////////////
void CPqfMainDlg::UpdateTestCtrls()
{
	int iSelItem = GetSelItem();
	//DBGOUT("UpdateTestCtrls(). sel item: %d\n", iSelItem);

	if (iSelItem == -1) return;

	LPPQFSTEP pqfStep = GetListItemData(iSelItem);
	if (pqfStep == NULL) return;
		
	if (pqfStep->step_level < PQF_LEVEL_OPEN)
		pqfStep->step_level = PQF_LEVEL_OPEN;
	else if (pqfStep->step_level > PQF_LEVEL_150)
		pqfStep->step_level = PQF_LEVEL_150;
	
	m_iStartPhase = pqfStep->step_start;
	// Make sure the value is within the limits
	if (m_iStartPhase > PQF_START_MAX)
		m_iStartPhase = PQF_START_MAX;
	else if (m_iStartPhase < PQF_START_MIN)
		m_iStartPhase = PQF_START_MIN;

	m_iDurUnits = pqfStep->step_duration_units;
	if (m_iDurUnits < 0)
		m_iDurUnits = 0;
	else if (m_iDurUnits > 2)
		m_iDurUnits = 2;

	m_arrDur[m_iDurUnits] = pqfStep->step_duration;
	if (m_iDurUnits == PQF_DURATION_UNITS_SEC)
		m_arrDur[m_iDurUnits] /= 10;
	if (m_arrDur[m_iDurUnits] > st_constDur[m_iDurUnits][DUR_MAX])
		m_arrDur[m_iDurUnits] = st_constDur[m_iDurUnits][DUR_MAX];
	else if (m_arrDur[m_iDurUnits] < st_constDur[m_iDurUnits][DUR_MIN])
		m_arrDur[m_iDurUnits] = st_constDur[m_iDurUnits][DUR_MIN];

	m_iRmsI = pqfStep->step_rms_ilim;
	// Make sure the value is within the limits
	if (m_iRmsI > testmgr_rms_imax)
		m_iRmsI = testmgr_rms_imax;
	else if (m_iRmsI < PQF_RMSI_MIN)
		m_iRmsI = PQF_RMSI_MIN;

	m_iPeakI = pqfStep->step_peak_ilim;
	// Make sure the value is within the limits
	if (m_iPeakI > testmgr_peak_imax)
		m_iPeakI = testmgr_peak_imax;
	else if (m_iPeakI < PQF_PEAKI_MIN)
		m_iPeakI = PQF_PEAKI_MIN;

	// Select current step level
	SendItemMsg(IDC_COMBO_LEVEL, CB_SETCURSEL, (WPARAM) pqfStep->step_level, 0);
	
	// Start [0-359 dg; 90 - default]
	SendItemMsg(IDC_SPIN_START, UDM_SETPOS, 0, (LPARAM) MAKELONG((short) m_iStartPhase, 0));

	// Duration [min/max/default depends on units selected]
	SendItemMsg(IDC_SPIN_DURATION, UDM_SETPOS, 0, (LPARAM) MAKELONG((short) m_arrDur[m_iDurUnits], 0));

	// Duration units [1-3 units; 1 - default]
	SendItemMsg(IDC_SPIN_DUR_UNIT, UDM_SETPOS, 0, (LPARAM) MAKELONG((short) m_iDurUnits, 0));

	// RMS-I [0.0-32.0; 10.0 - default (320 total)]
	SendItemMsg(IDC_SPIN_RMSI, UDM_SETPOS, 0, (LPARAM) MAKELONG((short) m_iRmsI, 0));

	// Peak-I [0-500 A; 100 - default]
	SendItemMsg(IDC_SPIN_PICKI, UDM_SETPOS, 0, (LPARAM) MAKELONG((short) m_iPeakI, 0));

	// Set the value of duration units
	SetItemText(IDC_DUR_UNITS_TEXT, pqf_duration_units[m_iDurUnits]);

	// Update the rest of the edit boxes with the valid values that we have
	Int2Text(IDD_PQF_STARTEDIT, m_iStartPhase);
	Dur2Text();
	double dVal = m_iRmsI / 10.0;
	Dbl2Text(IDD_PQF_RMSIEDIT, dVal);
	Int2Text(IDD_PQF_PEAKIEDIT, m_iPeakI);
}

//////////////////////////////////////////////////////////////////////
void CPqfMainDlg::UpdateStart(LPNMUPDOWN lpNMH)
{
	UpdateInt(IDD_PQF_STARTEDIT, m_iStartPhase, PQF_START_MIN, PQF_START_MAX, (lpNMH->iDelta > 0));
}

//////////////////////////////////////////////////////////////////////
void CPqfMainDlg::UpdateDuration(LPNMUPDOWN lpNMH)
{
	// Record present duration value 
	Text2Dur();

	m_arrDur[m_iDurUnits] += (lpNMH->iDelta > 0) ? 1 : -1;

	// Make sure the values are within the corresponding limits
	if (m_arrDur[m_iDurUnits] > st_constDur[m_iDurUnits][DUR_MAX])
		m_arrDur[m_iDurUnits] = st_constDur[m_iDurUnits][DUR_MAX];
	
	if (m_arrDur[m_iDurUnits] < st_constDur[m_iDurUnits][DUR_MIN])
		m_arrDur[m_iDurUnits] = st_constDur[m_iDurUnits][DUR_MIN];

	// Update duration value 
	Dur2Text();
}

//////////////////////////////////////////////////////////////////////
void CPqfMainDlg::UpdateDurUnits(LPNMUPDOWN lpNMH)
{
	// Record present duration value 
	Text2Dur();

	// Make sure the values are within the corresponding limits
	if (m_arrDur[m_iDurUnits] > st_constDur[m_iDurUnits][DUR_MAX])
		m_arrDur[m_iDurUnits] = st_constDur[m_iDurUnits][DUR_MAX];

	if (m_arrDur[m_iDurUnits] < st_constDur[m_iDurUnits][DUR_MIN])
		m_arrDur[m_iDurUnits] = st_constDur[m_iDurUnits][DUR_MIN];

	// Increment/Decrement present duration units
	m_iDurUnits += (lpNMH->iDelta > 0) ? 1 : -1;
	
	// Wrap the values, if necessary
	if (m_iDurUnits < PQF_DURATION_UNITS_MIN)
		m_iDurUnits = PQF_DURATION_UNITS_CYC;

	if (m_iDurUnits > PQF_DURATION_UNITS_CYC)
		m_iDurUnits = PQF_DURATION_UNITS_MIN;

	// Set the value
	SetItemText(IDC_DUR_UNITS_TEXT, pqf_duration_units[m_iDurUnits]);
	
	// Change limits on the Duration spin control
	SendItemMsg(IDC_SPIN_DURATION, UDM_SETRANGE, 0, 
			(LPARAM) MAKELONG((short) st_constDur[m_iDurUnits][DUR_MAX], (short) st_constDur[m_iDurUnits][DUR_MIN]));
	SendItemMsg(IDC_SPIN_DURATION, UDM_SETPOS, 0, (LPARAM) MAKELONG((short) m_arrDur[m_iDurUnits], 0));

	// Update duration value 
	Dur2Text();
}

//////////////////////////////////////////////////////////////////////
void CPqfMainDlg::UpdateRmsI(LPNMUPDOWN lpNMH)
{
	UpdateDbl(IDD_PQF_RMSIEDIT, m_iRmsI, PQF_RMSI_MIN, testmgr_rms_imax, (lpNMH->iDelta > 0));
}

//////////////////////////////////////////////////////////////////////
void CPqfMainDlg::UpdatePeakI(LPNMUPDOWN lpNMH)
{
	UpdateInt(IDD_PQF_PEAKIEDIT, m_iPeakI, PQF_PEAKI_MIN, testmgr_peak_imax, (lpNMH->iDelta > 0));
}

//////////////////////////////////////////////////////////////////////
LRESULT CALLBACK CPqfMainDlg::LocalDlgProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	// Local window procedure
	switch (msg) 
	{
	// Change background for some of the edit/static controls
	case WM_CTLCOLORSTATIC:
	case WM_CTLCOLOREDIT:
	case WM_CTLCOLORLISTBOX:
		if (lParam != NULL)
		{
			HBRUSH hBrush = NULL;
			switch (GetDlgCtrlID((HWND) lParam))
			{
			case IDD_PQF_STARTEDIT:
			case IDD_PQF_DURATIONEDIT:
			case IDD_PQF_RMSIEDIT:
			case IDD_PQF_PEAKIEDIT:
			case IDC_COMBO_LEVEL:
			case IDC_DUR_UNITS_TEXT:
				hBrush = obj_hbr_white; 
				SetBkColor((HDC) wParam, GetSysColor(COLOR_WINDOW));
				break;

			case IDC_INRUSH_POS:
			case IDC_INRUSH_NEG:
				if (m_bQualifying)
				{
					hBrush = obj_hbr_palegreen;
					SetBkColor((HDC) wParam, RGBpalegreen);
				}
				else if (test_status == TEST_STATUS_TESTING)
					break;
				else
				{
					int iTemp;
					if (!Text2Int(IDC_INRUSH_POS, iTemp) ||
						!Text2Int(IDC_INRUSH_NEG, iTemp))
						break;

					hBrush = obj_hbr_yellow; 
					SetBkColor((HDC) wParam, RGByellow);
				}
				break;

			case IDD_PQF_ACTUAL_RMSI:
			case IDD_PQF_ACTUAL_PEAKI:
			case IDD_PQF_LINE_VACTUAL:
			case IDD_PQF_LINE_FREQACTUAL:
			case IDD_PQF_LINE_VNOMINAL:
			case IDD_PQF_LINE_FREQNOMINAL:
				if (test_status == TEST_STATUS_TESTING)
				{
					hBrush = obj_hbr_palegreen;
					SetBkColor((HDC) wParam, RGBpalegreen);
				}
				else
				{
					hBrush = obj_hbr_yellow; 
					SetBkColor((HDC) wParam, RGByellow);
				}
				break;

			default:
				break;
			}

			if (hBrush != NULL)
			{
				UnrealizeObject(hBrush);
				POINT point;
				point.x = point.y = 0;
				ClientToScreen(hWnd, &point);
				SetBrushOrgEx((HDC) wParam, point.x, point.y, NULL);
				return ((DWORD) hBrush);
			}
		}
		return NULL;

	case WM_DRAWITEM:
		// This case is necessary to make sure the bitmap buttons (EUT, Exit, Start, etc.) will draw themselves.
		// We are basically relaying the message back to the buttons (MFC's reflections idea)
		switch (wParam)
		{
		case IDD_PQF_RUN:
		case IDD_PQF_STOP:
		case IDD_ECAT_EXIT:
		case IDD_PQF_EUTPOWER:
			SendItemMsg((int) wParam, msg, wParam, lParam);
			return TRUE;
		}
		break;

	case WM_NOTIFY:
		// See if we should validate and update spin controled edit boxes.
		// If the user types some garbage into the edit box, we should 
		// account for that situation. Also, updating duration units 
		// causes update of the limits on the spin controls.
		switch (wParam)
		{
		case IDC_SPIN_START:
			UpdateStart((LPNMUPDOWN) lParam);
			break;

		case IDC_SPIN_DURATION:
			UpdateDuration((LPNMUPDOWN) lParam);
			break;

		case IDC_SPIN_DUR_UNIT:
			UpdateDurUnits((LPNMUPDOWN) lParam);
			break;

		case IDC_SPIN_RMSI:
			UpdateRmsI((LPNMUPDOWN) lParam);
			break;

		case IDC_SPIN_PICKI:
			UpdatePeakI((LPNMUPDOWN) lParam);
			break;

		case IDD_PQF_LIST:
			{
				LPNMLISTVIEW pLVI = (LPNMLISTVIEW) lParam;
				if (pLVI->hdr.code == NM_CLICK)
				{
					//DBGOUT("NM_CLICK notify from IDD_PQF_LIST\n");
					int iSelItem = GetSelItem();
					if (iSelItem != -1)
					{
						UpdateTestCtrls();
						PqfGraphPickStep(GetDlgItem(IDD_PQF_GRAPH), iSelItem);
                        if ((iSelItem >= 0) AND pqfdlg_bListBoxLeft AND (iSelItem != test_stepnum) AND 
							((test_status EQ TEST_STATUS_TESTING) OR (test_status EQ TEST_STATUS_PAUSED))) 
						{
                            test_stepnum = iSelItem + 1;
                            test_dwDegreesUsedInStep = 0L;
                            PqfCalcTimeLeft();
                        }
						
					}
				}
			}
			break;

		default:
			break;
		}
		break;

	default:
		return CEcatMainDlg::LocalDlgProc(hWnd, msg, wParam, lParam);
	}

   return 0;
}

//////////////////////////////////////////////////////////////////////
// helpers
//////////////////////////////////////////////////////////////////////
BOOL CPqfMainDlg::Text2Dbl(UINT uiCtrlId, double& dVal)
{
	dVal = 0.0;
	LPTSTR lpStr;
	dVal = strtod(GetItemText(uiCtrlId), &lpStr);
	
	// See if some garbage is in the string
	if ((errno == ERANGE) || (*lpStr != '\0'))
	{
		dVal = 0.0;
		return FALSE;
	}

	return TRUE;
}

//////////////////////////////////////////////////////////////////////
BOOL CPqfMainDlg::Text2Int(UINT uiCtrlId, int& iVal)
{
	BOOL bTranslated;
	iVal = (int) GetDlgItemInt(GetHWND(), uiCtrlId, &bTranslated, TRUE);
	return bTranslated;
}

//////////////////////////////////////////////////////////////////////
BOOL CPqfMainDlg::Text2Int(LPSTR lpszVal, int& iVal)
{
	LPTSTR lpStr;
	iVal = strtol(lpszVal, &lpStr, 10);
	
	// See if some garbage is in the string
	if ((errno == ERANGE) || (*lpStr != '\0'))
		return FALSE;

	return TRUE;
}

//////////////////////////////////////////////////////////////////////
void CPqfMainDlg::Dbl2Text(UINT uiCtrlId, const double& dVal)
{
	char lpszBuf[56];
	sprintf(lpszBuf, "%.1f", dVal);
	SetItemText(uiCtrlId, lpszBuf);
}

//////////////////////////////////////////////////////////////////////
void CPqfMainDlg::Int2Text(UINT uiCtrlId, int iVal)
{
	SetDlgItemInt(GetHWND(), uiCtrlId, iVal, TRUE);
}

//////////////////////////////////////////////////////////////////////
void CPqfMainDlg::UpdateInt(UINT uiWndId, int& iHost, int iMin, int iMax, BOOL bInc)
{
	// Get the value from the edit box first. If we can successfully
	// convert the value, work with the converted value. Otherwise, 
	// revert to the present value (iHost). Make sure it's within the 
	// limits.
	int iVal;
	if (Text2Int(uiWndId, iVal))
		iHost = iVal;

	iHost += (bInc) ? 1 : -1;
	if (iHost > iMax)
		iHost = iMax;

	if (iHost < iMin)
		iHost = iMin;
	
	// Now, set the value
	Int2Text(uiWndId, iHost);
}

//////////////////////////////////////////////////////////////////////
void CPqfMainDlg::UpdateDbl(UINT uiWndId, int& iHost, int iMin, int iMax, BOOL bInc)
{
	// Get the value from the edit box first. If we can successfully
	// convert the value, work with the converted value. Otherwise, 
	// revert to the present value (iHost). Make sure it's within the 
	// limits.
	double dVal;
	if (Text2Dbl(uiWndId, dVal))
		iHost = (int) (dVal * 10.0);

	iHost += (bInc) ? 1 : -1;
	if (iHost > iMax)
		iHost = iMax;

	if (iHost < iMin)
		iHost = iMin;
	
	// Now, set the value
	dVal = iHost / 10.0;
	Dbl2Text(uiWndId, dVal);
}

//////////////////////////////////////////////////////////////////////
BOOL CPqfMainDlg::Text2Dur()
{
	// Text is different depending on the duration units selected. If we
	// can't convert the value, retain previous valid value.
	BOOL bOK = TRUE;
	int iVal;
	double dVal;
	switch (m_iDurUnits)
	{
	case PQF_DURATION_UNITS_MIN:
		{
			// Most complicated case - convert MM:SS value to seconds
			char lpszBuf[MAX_PATH + 1];
			strcpy(lpszBuf, GetItemText(IDD_PQF_DURATIONEDIT));
			char* lpszSS = NULL;
			char* lpszMM = strtok(lpszBuf, ":");
			if (lpszMM != NULL)
				lpszSS = strtok(NULL, ":");

			int iMM, iSS;
			if (Text2Int(lpszMM, iMM) && (Text2Int(lpszSS, iSS)))
				m_arrDur[m_iDurUnits] = iMM * 60 + iSS;
			else
				bOK = FALSE;
		}
		break;

	case PQF_DURATION_UNITS_SEC:
		// Convert text to double
		if (Text2Dbl(IDD_PQF_DURATIONEDIT, dVal))
			m_arrDur[m_iDurUnits] = (int) (dVal * 10.0);
		else
			bOK = FALSE;
		break;

	case PQF_DURATION_UNITS_CYC:
		// Simpliest case - convert text to int
		if (Text2Int(IDD_PQF_DURATIONEDIT, iVal))
			m_arrDur[m_iDurUnits] = iVal;
		else
			bOK = FALSE;
		break;
	}

	return bOK;
}

//////////////////////////////////////////////////////////////////////
void CPqfMainDlg::Dur2Text()
{
	switch (m_iDurUnits)
	{
	case PQF_DURATION_UNITS_MIN:
		{
			// Convert int to "MM:SS" string
			char lpszBuf[MAX_PATH + 1];
			sprintf(lpszBuf, "%3d:%02d", m_arrDur[m_iDurUnits] / 60, m_arrDur[m_iDurUnits] % 60);
			SetItemText(IDD_PQF_DURATIONEDIT, lpszBuf);
		}
		break;

	case PQF_DURATION_UNITS_SEC:
		{
			// Convert double to text
			double dVal = m_arrDur[m_iDurUnits] / 10.0;
			Dbl2Text(IDD_PQF_DURATIONEDIT, dVal);
		}
		break;

	case PQF_DURATION_UNITS_CYC:
		// Convert int to text
		Int2Text(IDD_PQF_DURATIONEDIT, m_arrDur[m_iDurUnits]);
		break;
	}

}

//////////////////////////////////////////////////////////////////////
void CPqfMainDlg::SendPqfIdleCmnd()
{
	// Initialize ECAT with 100% power if EP3 installed. Otherwise, depending on the option selcted.
	if (bIsEP3Avail)
		SendMessage (testmgr_hWndComm, KT_PQF_IDLE, 100, 0L);
	else
		SendMessage (testmgr_hWndComm, KT_PQF_IDLE, ((pqf_lpStepListhdr.hdr_standby_level == PQF_LEVEL_OPEN) ? 0 : 100), 0L);
}

//////////////////////////////////////////////////////////////////////
void CPqfMainDlg::SetStandbyLevel(short shLevel)
{
	switch (shLevel)
	{
	case PQF_LEVEL_OPEN:
		CheckRadioButton(GetHWND(), IDC_RDB_OPEN, IDC_RDB_HUNDRED, IDC_RDB_OPEN);
		break;

	default:
	case PQF_LEVEL_100:
		CheckRadioButton(GetHWND(), IDC_RDB_OPEN, IDC_RDB_HUNDRED, IDC_RDB_HUNDRED);
		break;
	}
}

//////////////////////////////////////////////////////////////////////
short CPqfMainDlg::GetStandbyLevel()
{
	if (IsDlgButtonChecked(GetHWND(), IDC_RDB_OPEN))
		return PQF_LEVEL_OPEN;

	return PQF_LEVEL_100;
}

//////////////////////////////////////////////////////////////////////
void CPqfMainDlg::SetPhase(BYTE byPhase)
{
	switch (byPhase)
	{
	default:
	case 1:
		CheckRadioButton(GetHWND(), IDD_PQF_PHASEL1, IDD_PQF_PHASEL3, IDD_PQF_PHASEL1);
		break;

	case 2:
		CheckRadioButton(GetHWND(), IDD_PQF_PHASEL1, IDD_PQF_PHASEL3, IDD_PQF_PHASEL2);
		break;

	case 3:
		CheckRadioButton(GetHWND(), IDD_PQF_PHASEL1, IDD_PQF_PHASEL3, IDD_PQF_PHASEL3);
		break;
	}
}

//////////////////////////////////////////////////////////////////////
BYTE CPqfMainDlg::GetPhase()
{
	if (IsDlgButtonChecked(GetHWND(), IDD_PQF_PHASEL2))
		return 2;
	else if (IsDlgButtonChecked(GetHWND(), IDD_PQF_PHASEL3))
		return 3;
	
	return 1;
}

//////////////////////////////////////////////////////////////////////
void CPqfMainDlg::PqfUpdateTestStatus(BOOL bStart)
{
	STATIC short oldstepnum = -1;

    if (testmgr_bSimulate)
		return;
        //PqfCalcTimeLeft();

    if (bStart OR (test_status EQ TEST_STATUS_DONE)) 
	{
        test_stepnum = 1;
        SelectListItem(test_stepnum - 1);
		SendItemMsg(IDD_PQF_LIST, LVM_ENSUREVISIBLE, (WPARAM) test_stepnum - 1, 0);						
		PqfGraphPickStep(GetDlgItem(IDD_PQF_GRAPH), test_stepnum - 1);
        oldstepnum = test_stepnum;
    } 
	else if (testmgr_bShowStatus AND (bStart OR (test_stepnum != oldstepnum))) 
	{
        SelectListItem(test_stepnum - 1);
        PqfGraphPickStep(GetDlgItem(IDD_PQF_GRAPH), test_stepnum - 1);
		SendItemMsg(IDD_PQF_LIST, LVM_ENSUREVISIBLE, (WPARAM) test_stepnum - 1, 0);						
        oldstepnum = test_stepnum;
    }

	// LOG begin
	LogStatusUpdate();
	// LOG end

	double dRMSI = (double) testmgr_PqfStatus.pqfs_rms_iread / 10.0;
	Dbl2Text(IDD_PQF_ACTUAL_RMSI, dRMSI);
	Int2Text(IDD_PQF_ACTUAL_PEAKI, testmgr_PqfStatus.pqfs_peak_iread);
}

//////////////////////////////////////////////////////////////////////
void CPqfMainDlg::PqfCalcTimeLeft()
{
    DWORD dwDegrees = 0L;
	for (int stepi = test_stepi; stepi < pqf_lpStepListhdr.hdr_numsteps; stepi++)
	{
		LPPQFSTEP pPgqStep = GetListItemData(stepi);
		if (pPgqStep)
	        dwDegrees += PqfUtilStep2Degrees(pPgqStep);
	}

    dwDegrees -= test_dwDegreesUsedInStep;
    PqfSetTimeLeft (dwDegrees);
}

//////////////////////////////////////////////////////////////////////
void CPqfMainDlg::PqfDlgTestCompleted()
{
    BOOL bInProgress = (test_status == TEST_STATUS_DONE);

    PqfUpdateTestStatus (FALSE);
    test_status = TEST_STATUS_DONE;
	nRunMode = IDLE_MODE;

	if (testmgr_bSimulate) 
	{
        StopTimer();
        sprintf (common_message, "Test Completed\015\012Standby voltage level resumed (%s)", pqf_level_display[pqf_lpStepListhdr.hdr_standby_level]);
    }
    else if (!testmgr_bSeti) 
	{ /* aborted by user choice, after losing touch with ECAT */
        if (bInProgress) /* unable to send STOP_TEST command */
          sprintf (common_message, "Test Aborted by user choice, after communications error with ECAT\015\012Unable to send STOP command to ECAT -- present voltage level UNKNOWN");
        else
          sprintf (common_message, "Test Aborted by user choice, after communications error with ECAT\015\012Standby voltage level resumed (%s)", pqf_level_display[pqf_lpStepListhdr.hdr_standby_level]);
    }
    else if (testmgr_PqfStatus.pqfs_error OR testmgr_PqfStatus.pqfs_nIlock) 
	{
        if (testmgr_eut_status != TESTMGR_EUT_ON)
          sprintf (common_message, "Test Aborted\015\012Voltage has been turned off");
        else
          sprintf (common_message, "Test Aborted\015\012Standby voltage level resumed (%s)", pqf_level_display[pqf_lpStepListhdr.hdr_standby_level]);
    }
    else if (testmgr_eut_status != TESTMGR_EUT_ON)
      sprintf (common_message, "Test Aborted\015\012Voltage has been turned off");
    else
      sprintf (common_message, "Test Completed\015\012Standby voltage level resumed (%s)", pqf_level_display[pqf_lpStepListhdr.hdr_standby_level]);
	
	///////////////////////////////////////////////////////////////////
	// Victor Ginzburg
	if (Log.log_on()) 
	  {
      Log.logtime = FALSE ;
      char szBuf [256] ;
      sprintf (szBuf, "%Fs: %s\r\n", 
	          (LPSTR)Log.getlogtime(), 
              common_message);
      Log.loglpstr((LPSTR)szBuf);
      Log.logtime = TRUE ;
	  Log.end_log();
	  }
	///////////////////////////////////////////////////////////////////
    
	UtilMessageBox (hDlg_pqf, common_message, (char*) CPqfApp::GetAppName(), MB_OK | MB_ICONINFORMATION);
	PqfSetTimeLeft(0, TRUE);
	EnableControls(FALSE/* not testing*/);
    TestmgrKludge();
}

//////////////////////////////////////////////////////////////////////
DWORD CPqfMainDlg::PqfUtilStep2Degrees(LPPQFSTEP lpStep)
{
    DWORD dwTotDegrees = 0;
	int iDurUnits;
	DWORD dwStepDegrees, dwPhase;
	if (!IsValidAddress(lpStep, sizeof(PQFSTEP), FALSE))
	{
		iDurUnits = m_iDurUnits;
		dwStepDegrees = m_arrDur[m_iDurUnits]; 
		dwPhase = (DWORD) m_iStartPhase;
	}
	else
	{
		iDurUnits = lpStep->step_duration_units;
		dwStepDegrees = lpStep->step_duration; 
		dwPhase = (DWORD) lpStep->step_start;
	}

    switch (iDurUnits) 
	{
    case PQF_DURATION_UNITS_MIN:
        dwTotDegrees = dwStepDegrees * pqf_dwDegreesPerSecond;
        break;

	default:
    case PQF_DURATION_UNITS_SEC:
        dwTotDegrees = (dwStepDegrees * pqf_dwDegreesPerSecond) / 100L;
        break;
    
	case PQF_DURATION_UNITS_CYC:
        dwTotDegrees = dwStepDegrees * PQF_DEGREES_PER_CYCLE;
        break;
    }

    dwTotDegrees += dwPhase;
    return dwTotDegrees;
}

//////////////////////////////////////////////////////////////////////
void CPqfMainDlg::PqfSetTotalTime(DWORD dwTotDegrees)
{
    CHARACTER buffer[50];
    SHORT hours, minutes, seconds, hundredths;

	pqf_lpStepListhdr.hdr_dwTotDegrees = dwTotDegrees;
	hours = (SHORT) (dwTotDegrees / pqf_dwDegreesPerHour);
	minutes = (SHORT) ((dwTotDegrees % pqf_dwDegreesPerHour) / pqf_dwDegreesPerMinute);
	seconds = (SHORT) ((dwTotDegrees % pqf_dwDegreesPerMinute) / pqf_dwDegreesPerSecond);
	hundredths = (SHORT) ((((dwTotDegrees % pqf_dwDegreesPerMinute) * 100L) / pqf_dwDegreesPerSecond) % 100);
	sprintf (buffer, "Total Time: %02d:%02d:%02d.%02d", hours, minutes, seconds, hundredths);
	SetDlgItemText(hDlg_pqf, IDD_PQF_TOTALTIME, buffer);
}

//////////////////////////////////////////////////////////////////////
void CPqfMainDlg::PqfSetTimeLeft(DWORD dwTotDegrees, BOOL bClear/*FALSE*/)
{
    char buffer[256];
	buffer[0] = '\0';
	if (!bClear)
	{
		SHORT hours, minutes, seconds, hundredths;

		if (!testmgr_bShowStatus)
			return;

		hours = (SHORT) (dwTotDegrees / pqf_dwDegreesPerHour);
		minutes = (SHORT) ((dwTotDegrees % pqf_dwDegreesPerHour) / pqf_dwDegreesPerMinute);
		seconds = (SHORT) ((dwTotDegrees % pqf_dwDegreesPerMinute) / pqf_dwDegreesPerSecond);
		hundredths = (SHORT) ((((dwTotDegrees % pqf_dwDegreesPerMinute) * 100L) / pqf_dwDegreesPerSecond) % 100);
		sprintf (buffer, "Time Left: %02d:%02d:%02d.%02d", hours, minutes, seconds, hundredths);
	}

    SetDlgItemText (hDlg_pqf, IDD_PQF_TIME, buffer);
}

//////////////////////////////////////////////////////////////////////
BOOL CPqfMainDlg::PqfValidateTest()
{
	char szBuf[1024];
    int channel[3], nlevels = 0;
    channel[1] = channel[2] = -1;
    BOOL bShortDur = FALSE;

    for (int i = 0; i < GetListItemCnt(); i++)
	{
		// Verify test data for each step in the test plan list
		LPPQFSTEP pqfStep = GetListItemData(i);
		if (pqfStep == NULL)
		{
			Message(common_errcaption, "Test data member of the test list is invalid. Please, restart the program.");
			return FALSE;
		}

		if (pqfStep->step_rms_ilim > testmgr_rms_imax) 
		{
			sprintf(common_message, "The rms-ilimit (%s) for step %d exceeds tester limits (%s).", pqfStep->step_rms_ilim, i, testmgr_rms_imax);

			// LOG begin
			if (Log.log_on()) 
			{ 
				Log.logtime = FALSE;
				sprintf (szBuf, "%Fs: %s\r\n", (LPSTR)Log.getlogtime(), common_message);
				Log.loglpstr((LPSTR)szBuf);
				Log.logtime = TRUE ;
				Log.end_log();
			}
			// LOG end

			Message(common_errcaption, common_message);
			return FALSE;
		}

		if (pqfStep->step_peak_ilim > testmgr_peak_imax) 
		{
			sprintf(common_message, "The peak-ilimit (%s) for step %d exceeds tester limits (%s)", pqfStep->step_peak_ilim, i, testmgr_peak_imax);

			// LOG begin
			if (Log.log_on()) 
			{
				Log.logtime = FALSE ;
				sprintf (szBuf, "%Fs: %s\r\n", (LPSTR)Log.getlogtime(), common_message);
				Log.loglpstr((LPSTR)szBuf);
				Log.logtime = TRUE ;
				Log.end_log();
			}
			// LOG end

			Message(common_errcaption, common_message);
			return FALSE;
		}

		if ((pqfStep->step_duration_units == PQF_DURATION_UNITS_CYC) && (pqfStep->step_duration < 6))
			bShortDur = TRUE;

		if ((pqfStep->step_level == PQF_LEVEL_OPEN) || (pqfStep->step_level == PQF_LEVEL_100))
			continue;

		switch (nlevels)
		{
		case 0:
			nlevels++;
			channel[nlevels] = pqfStep->step_level;
			break;

		case 1:
			if (channel[1] != pqfStep->step_level) 
			{
				nlevels++;
				channel[nlevels] = pqfStep->step_level;
			}
			break;

		case 2:
		default:
			if ((channel[1] != pqfStep->step_level) && (channel[2] != pqfStep->step_level))
				nlevels = 3;
				break;
		}
	} // each step in test plan list 

	if (bShortDur AND (nlevels > 2)) 
	{
		strcpy(common_message,
		"Test exceeds internal timing capabilities, and cannot be supported as it is presently defined.\n"
		"Test must either have each of its steps be of more than 6 cycles duration, or use only two voltage levels\n"
		"(excepting Open-0%% and 100%% which don't count).");

		// LOG begin
		if (Log.log_on()) 
		{
			Log.logtime = FALSE;
			sprintf (szBuf, "%Fs: %s\r\n", (LPSTR)Log.getlogtime(), common_message);
			Log.loglpstr((LPSTR)szBuf);
			Log.logtime = TRUE;
			Log.end_log();
		}
		// LOG end

		Message(common_errcaption, common_message);
		return FALSE;
	}

	return TRUE;
}

//////////////////////////////////////////////////////////////////////
BOOL CPqfMainDlg::GetRepeatFlag()
{
    return IsDlgButtonChecked(GetHWND(), IDD_PQF_TESTREPEAT);
}

//////////////////////////////////////////////////////////////////////
void CPqfMainDlg::SetRepeatFlag(BOOL bRepeat)
{
	CheckDlgButton(GetHWND(), IDD_PQF_TESTREPEAT, (bRepeat ? BST_CHECKED : BST_UNCHECKED));
}

//////////////////////////////////////////////////////////////////////
WORD CPqfMainDlg::TestmgrCreateTest()
{
	// [aat] 07/02/02 Cleanup any pointers that were allocated earlier and 
	//		 not deleted for some reason
	TestmgrDestroyTest();

    testlen = pqf_lpStepListhdr.hdr_numsteps + 2;
    st_lpPqfTestPacketList = new PQFLIST[testlen];
    st_lpTestStepInfo = new TESTSTEPINFO[testlen];
	if ((st_lpPqfTestPacketList == NULL) || (st_lpTestStepInfo == NULL))
	{
		TestmgrDestroyTest();
		ShowOutOfMemoryDlg();
		return 0;
	}

	pqf_lpStepListhdr.hdr_standby_level = GetStandbyLevel();
	m_bRepeatTest = GetRepeatFlag();

    DWORD dwDegreesLeft = pqf_lpStepListhdr.hdr_dwTotDegrees;
    SHORT pnum = 0;
    for (int stepi = 0; stepi < pqf_lpStepListhdr.hdr_numsteps; stepi++) 
	{
		LPPQFSTEP pPqfStep = GetListItemData(stepi);
		if (pPqfStep == NULL)
		{
			Message("Internal Error", "FATAL ERROR: PQF test object is invalid. Please, restart the program and try again.");
			TestmgrDestroyTest();
			return 0;
		}

        st_lpPqfTestPacketList[pnum].tp_action = (BYTE) pPqfStep->step_level;
        if (pPqfStep->step_duration_units EQ PQF_DURATION_UNITS_CYC)
            st_lpPqfTestPacketList[pnum].tp_action |= 0x40;
        else if (pPqfStep->step_duration EQ 0)
            st_lpPqfTestPacketList[pnum].tp_action |= 0x40;
        
		switch (pPqfStep->step_duration_units) 
		{
            case PQF_DURATION_UNITS_MIN:
                st_lpPqfTestPacketList[pnum].tp_duration = pPqfStep->step_duration * 100L;
                break;
            case PQF_DURATION_UNITS_SEC:
                st_lpPqfTestPacketList[pnum].tp_duration = pPqfStep->step_duration;
                break;
            case PQF_DURATION_UNITS_CYC:
                st_lpPqfTestPacketList[pnum].tp_duration = (DWORD) pPqfStep->step_duration;
                break;
        } // switch (duration_units) 
    
		st_lpPqfTestPacketList[pnum].tp_start_angle = (WORD) pPqfStep->step_start;
        st_lpPqfTestPacketList[pnum].tp_RMS_limit = (WORD) pPqfStep->step_rms_ilim;
        st_lpPqfTestPacketList[pnum].tp_PEAK_limit = (WORD) pPqfStep->step_peak_ilim;
        st_lpTestStepInfo[pnum].tsi_stepi = stepi;
        st_lpTestStepInfo[pnum].tsi_dwDegreesLeft = dwDegreesLeft;
        dwDegreesLeft -= PqfUtilStep2Degrees(pPqfStep);
        pnum++;
    } // for each step in list 

	// Special extra steps
    if (m_bRepeatTest) 
	{
		// repeat test
        st_lpPqfTestPacketList[pnum].tp_action = (CHAR) 0x80;
        st_lpPqfTestPacketList[pnum].tp_duration = 0;
        st_lpPqfTestPacketList[pnum].tp_start_angle = 0;
        st_lpPqfTestPacketList[pnum].tp_RMS_limit = st_lpPqfTestPacketList[pnum-1].tp_RMS_limit;
        st_lpPqfTestPacketList[pnum].tp_PEAK_limit = st_lpPqfTestPacketList[pnum-1].tp_PEAK_limit;
        st_lpTestStepInfo[pnum].tsi_stepi = st_lpTestStepInfo[pnum-1].tsi_stepi;
        pnum++;
    }
    else 
	{ 
		// go to standby power level right before the test is done
        st_lpPqfTestPacketList[pnum].tp_action = (CHAR) pqf_lpStepListhdr.hdr_standby_level;
        st_lpPqfTestPacketList[pnum].tp_duration = 10; // .1 sec 
        st_lpPqfTestPacketList[pnum].tp_start_angle = 0; // ASAP 
        st_lpPqfTestPacketList[pnum].tp_RMS_limit = testmgr_rms_imax;
        st_lpPqfTestPacketList[pnum].tp_PEAK_limit = testmgr_peak_imax;
        st_lpTestStepInfo[pnum].tsi_stepi = st_lpTestStepInfo[pnum-1].tsi_stepi;
        pnum++;
    }

	// end of list indicator
    st_lpPqfTestPacketList[pnum].tp_action = 0x85; 
    st_lpPqfTestPacketList[pnum].tp_RMS_limit = st_lpPqfTestPacketList[pnum-1].tp_RMS_limit;
    st_lpPqfTestPacketList[pnum].tp_PEAK_limit = st_lpPqfTestPacketList[pnum-1].tp_PEAK_limit;
    st_lpTestStepInfo[pnum].tsi_stepi = st_lpTestStepInfo[pnum-1].tsi_stepi;
    pnum++;

    if (pnum != testlen) 
	{
        sprintf (common_message, "pnum=%d, testlen=%d (!)", pnum, testlen);
        Message(common_fatalcaption, common_message);
    }

    if (pqf_lpStepListhdr.hdr_dwTotDegrees <= (2L * pqf_dwDegreesPerSecond))
        testmgr_bShowStatus = FALSE;
    else
        testmgr_bShowStatus = TRUE;

    return (testlen);
}

//////////////////////////////////////////////////////////////////////
void CPqfMainDlg::TestmgrDestroyTest()
{
	if (st_lpTestStepInfo != NULL)
	{
		delete[] st_lpTestStepInfo;
		st_lpTestStepInfo = NULL;
	}

	if (st_lpPqfTestPacketList != NULL)
	{
		delete[] st_lpPqfTestPacketList;
		st_lpPqfTestPacketList = NULL;
	}
}

STATIC INTEGER last_stepi = (-1);
STATIC INTEGER same_step_count = 0;

//////////////////////////////////////////////////////////////////////
void CPqfMainDlg::TestmgrKludge()
{
    last_stepi = (-1);
    same_step_count = 0;
} /* TestmgrKludge */

//////////////////////////////////////////////////////////////////////
BOOL CPqfMainDlg::TestmgrReadStatus (HWND hDlg, INTEGER caller)
{
    BOOL bRetval;
    INTEGER idd_eutpower, idd_eutcheat;

    idd_eutpower = IDD_PQF_EUTPOWER;
    idd_eutcheat = IDD_PQF_CHEAT;

	/*
    if (hDlg EQ hWnd_pqf)
        UtilMessageBox (hDlg, "Can't call ReadStatus with hWnd -- must be hDlg", common_errcaption, MB_OK);
    
	if (hDlg != hDlg_pqf) 
	{
        idd_eutpower = IDD_INRUSH_EUTPOWER;
        idd_eutcheat = IDD_INRUSH_CHEAT;
    }
	*/

    testmgr_bGettingStatus = TRUE;
    bRetval = Testmgr___ReadStatusExec (hDlg, caller, idd_eutpower, idd_eutcheat);
    
	if (!bRetval)
        testmgr_bEcatCommExit = TRUE;
    
	testmgr_bGettingStatus = FALSE;

    return (bRetval);
} /* TestmgrReadStatus */

//////////////////////////////////////////////////////////////////////
BOOL CPqfMainDlg::Testmgr___ReadStatusExec (HWND hDlg, INTEGER caller, INTEGER idd_eutpower, INTEGER idd_eutcheat)
{
	//DBGOUT("In Testmgr___ReadStatusExec().\n");
    HINSTANCE hInst = GetInst();
	STATIC INTEGER glitches = 0;
    INTEGER nid, eut_status, retval;
    CHARACTER buffer[100];
    DWORD dwDegreesLeft, dwDegreesGone, statret;

    statret = (DWORD) SendMessage (testmgr_hWndComm, KT_PQF_STATUS, 0, (LPARAM) ((LPPQFSTATUS) &testmgr_PqfStatus));
    while (!statret) 
	{
        glitches++;
        same_step_count++;
        if ((glitches > 3) AND (test_status EQ TEST_STATUS_TESTING))
            return (FALSE);

		EnableWindow (hWnd_pqf, FALSE);
		char bufMsg[2048];
		UINT uiFlags = MB_ICONERROR;
        if ((test_status EQ TEST_STATUS_PAUSED) OR (test_status EQ TEST_STATUS_TESTING))
		{
            strcpy(bufMsg,	"Communications error with ECAT during testing. Would like to save the test, if necessary, and exit the application?\n"
							"Yes:\tsave the test and exit;\n"
							"No:\ttry to reestablish communications;\n"
							"Cancel:\tECAT is not presently accessible - stop testing;\n");
			uiFlags |= (MB_YESNOCANCEL | MB_DEFBUTTON2);
		}
        else
		{
            strcpy(bufMsg,	"Communications error with ECAT. Would like to save the test, if necessary, and exit the application?\n"
							"Yes:\tsave the test and exit;\n"
							"No:\tcontinue;\n");
			uiFlags |= (MB_YESNO | MB_DEFBUTTON1);
		}

		switch (MessageBox(hWnd_pqf, bufMsg, common_errcaption, uiFlags))
		{
		case IDYES:
			if ((changed_since_save) && (MessageBox(hWnd_pqf, "Save the test before exiting?", "Save", MB_YESNO | MB_ICONQUESTION) == IDYES))
				DoFileSave();

			// The app will quit here
            return (FALSE);

		case IDNO:
			COMMERROR = FALSE;
	        statret = (DWORD) SendMessage (testmgr_hWndComm, KT_PQF_STATUS, 0, (LPARAM) ((LPPQFSTATUS) &testmgr_PqfStatus));
			break;

		default:
		case IDCANCEL:
            retval = SendMessage (testmgr_hWndComm, KT_PQF_STOP, 0, 0L);
            if (retval) 
                nid = UtilMessageBox (hDlg, "ALERT: Unable to communicate with ECAT to stop testing!!!", common_errcaption, MB_OK);
            
			EnableWindow (hWnd_pqf, TRUE);
			testmgr_bSeti = FALSE;
            PqfDlgTestCompleted();
            last_stepi = (-1);
            same_step_count = 0;
            testmgr_PqfStatus.pqfs_step_number = -1;
            return (TRUE);
		}

		EnableWindow (hWnd_pqf, TRUE);
	} /* while can't get status */

    glitches = 0;
    if (testmgr_PqfStatus.pqfs_eut_status != testmgr_eut_status) 
	{
		//DBGOUT("EUT status changed.\n");
        eut_status = testmgr_eut_status;
        testmgr_eut_status = testmgr_PqfStatus.pqfs_eut_status;
        TestmgrUpdateEUTPower (hDlg, idd_eutpower, idd_eutcheat);
    } /* if EUT status has changed */

    if (testmgr_PqfStatus.pqfs_nIlock AND
        ((eut_status EQ TESTMGR_EUT_ON) OR
         (testmgr_eut_status EQ TESTMGR_EUT_ON) OR
         (test_status EQ TEST_STATUS_PAUSED) OR
         (test_status EQ TEST_STATUS_TESTING) OR
         (caller EQ TESTMGR_CALLER_RUNTEST) ) ) 
	{
        if ((test_status EQ TEST_STATUS_PAUSED) OR
            (test_status EQ TEST_STATUS_TESTING)) 
            SendMessage (testmgr_hWndComm, KT_PQF_STOP, 0, 0L);

        Ilock.Check_Interlocks(testmgr_PqfStatus.pqfs_nIlock);
        testmgr_nIlock = testmgr_PqfStatus.pqfs_nIlock;
        if ((test_status EQ TEST_STATUS_PAUSED) OR
            (test_status EQ TEST_STATUS_TESTING)) 
		{
            PqfDlgTestCompleted();
            last_stepi = (-1);
            same_step_count = 0;
            testmgr_PqfStatus.pqfs_step_number = -1;
        }
    }

    if (testmgr_PqfStatus.pqfs_error) 
	{
		//DBGOUT("Error detected: %d.\n", testmgr_PqfStatus.pqfs_error);
        char lpszErrStr[256];
        SendMessage (testmgr_hWndComm, KT_PQF_GET_ERROR, (WPARAM) testmgr_PqfStatus.pqfs_error, (LPARAM) lpszErrStr);
		if (IsSameNoCase(lpszErrStr, "OK"))
		{
			//DBGOUT("Returning from get error routine.\n");
			return TRUE;
		}

		// LOG begin
		if (Log.log_on()) 
		{ 
			Log.logtime = FALSE ;
			char szBuf [256] ;
			lstrcpy (szBuf, (LPSTR)Log.getlogtime());
			lstrcat (szBuf, " ");
			lstrcat (szBuf, lpszErrStr);
			lstrcat (szBuf, " \r\n");

			Log.loglpstr((LPSTR)szBuf);
			Log.logtime = TRUE ;
			Log.end_log();
		}
		// LOG end

		if ((test_status EQ TEST_STATUS_PAUSED) OR
			(test_status EQ TEST_STATUS_TESTING)) 
		{
			PqfDlgTestCompleted();
			last_stepi = (-1);
			same_step_count = 0;
			testmgr_PqfStatus.pqfs_step_number = -1;
		}
		UtilMessageBox (hDlg, lpszErrStr, "Tester Error", MB_OK);
    }

    if ((testmgr_PqfStatus.pqfs_step_number >= 0) AND st_lpPqfTestPacketList) 
	{
        if (testmgr_bShowStatus) 
		{
            if (testmgr_PqfStatus.pqfs_step_number >= testlen) 
			{
                sprintf (common_message, "active stepnum=%d, testlen=%d", testmgr_PqfStatus.pqfs_step_number, testlen);
                UtilMessageBox (hDlg_pqf, common_message, common_errcaption, MB_OK);
            }
            
			test_stepi = st_lpTestStepInfo[testmgr_PqfStatus.pqfs_step_number].tsi_stepi;
            dwDegreesLeft = st_lpTestStepInfo[testmgr_PqfStatus.pqfs_step_number].tsi_dwDegreesLeft;

			LPPQFSTEP pPqfStep = GetListItemData(test_stepi); 
            test_stepnum = (short) pPqfStep->step_num;
            if (last_stepi EQ test_stepi) 
			{
                same_step_count++;
                dwDegreesGone = same_step_count * (pqf_dwDegreesPerSecond/2);
                if (dwDegreesLeft > dwDegreesGone)
                    dwDegreesLeft -= dwDegreesGone;
                else
                    dwDegreesLeft = 0L;
            }
            else 
			{
                last_stepi = test_stepi;
                same_step_count = 0;
            }
            
			PqfSetTimeLeft (dwDegreesLeft);
        }

        PqfUpdateTestStatus (FALSE);

    }
    else 
	{  /* not presently testing */
        if ((test_status EQ TEST_STATUS_PAUSED) OR
            (test_status EQ TEST_STATUS_TESTING)) /* test just ended */
            PqfDlgTestCompleted();
        
		last_stepi = (-1);
        same_step_count = 0;
    }

    if (test_status EQ TEST_STATUS_TESTING) 
	{
        SetDlgItemText (hDlg, IDD_PQF_LINE_VACTUAL, _lritoa (testmgr_PqfStatus.pqfs_line_voltage, buffer, 3, ' '));
        sprintf (common_message, "%2d.%1d", testmgr_PqfStatus.pqfs_line_freq/10, testmgr_PqfStatus.pqfs_line_freq%10);
        SetDlgItemText (hDlg, IDD_PQF_LINE_FREQACTUAL, common_message);
    }
    else if (caller != TESTMGR_CALLER_INRUSH) 
	{ /* power off */
        SetDlgItemText (hDlg, IDD_PQF_LINE_VACTUAL, common_nullstr);
        SetDlgItemText (hDlg, IDD_PQF_LINE_FREQACTUAL, common_nullstr);
    }

    return (TRUE);
}

//////////////////////////////////////////////////////////////////////
void CPqfMainDlg::TestmgrCheat()
{
	PrintfECAT (0,(char*)"PQF:CHEAT");
	ECAT_sync();
} /* TestmgrCheat */

//////////////////////////////////////////////////////////////////////
void CPqfMainDlg::TestmgrUpdateEUTPower(HWND hDlg, INTEGER idd_eutpower, INTEGER idd_eutcheat)
{
    switch (testmgr_eut_status) 
	{
        case TESTMGR_EUT_OFF:
        case TESTMGR_EUT_RDY:
        case TESTMGR_EUT_ON:
			SendMessage(GetDlgItem(IDD_PQF_EUTPOWER), BM_SETCHECK, (WPARAM) testmgr_eut_status, 0L);
            break;

		default:
			break;
    }
}

//////////////////////////////////////////////////////////////////////
void CPqfMainDlg::EnableControls(BOOL bTesting)
{
	// Test graph
	PqfGraphTestInProgress(GetDlgItem(IDD_PQF_GRAPH), bTesting);

	// List view
	EnableWindow(GetDlgItem(IDD_PQF_LIST), !bTesting);

	// Test list buttons
	EnableWindow(GetDlgItem(IDD_PQF_INSERT), !bTesting);
	EnableWindow(GetDlgItem(IDD_PQF_REPLACE), !bTesting);
	EnableWindow(GetDlgItem(IDD_PQF_COPY), !bTesting);
	EnableWindow(GetDlgItem(IDD_PQF_DELETE), !bTesting);
	EnableWindow(GetDlgItem(IDD_PQF_DELETE_ALL), !bTesting);
	EnableWindow(GetDlgItem(IDD_PQF_INSERT), !bTesting);
	EnableWindow(GetDlgItem(IDC_BUT_LIST_UP), !bTesting);
	EnableWindow(GetDlgItem(IDC_BUT_LIST_DN), !bTesting);
	
	// Radio buttons
	if (bIsEP3Avail)
	{
		EnableWindow(GetDlgItem(IDD_PQF_PHASEL1), !bTesting);
		EnableWindow(GetDlgItem(IDD_PQF_PHASEL2), !bTesting);
		EnableWindow(GetDlgItem(IDD_PQF_PHASEL3), !bTesting);
	}
	else
	{
		EnableWindow(GetDlgItem(IDC_RDB_OPEN), !bTesting);
		EnableWindow(GetDlgItem(IDC_RDB_HUNDRED), !bTesting);
	}

	// CheckBox
	EnableWindow(GetDlgItem(IDD_PQF_TESTREPEAT), !bTesting);

	// Edit controls
	SendMessage(GetDlgItem(IDD_PQF_STARTEDIT), EM_SETREADONLY, bTesting, 0L);
	SendMessage(GetDlgItem(IDD_PQF_DURATIONEDIT), EM_SETREADONLY, bTesting, 0L);
	SendMessage(GetDlgItem(IDD_PQF_PEAKIEDIT), EM_SETREADONLY, bTesting, 0L);
	SendMessage(GetDlgItem(IDD_PQF_RMSIEDIT), EM_SETREADONLY, bTesting, 0L);
	
	// ComboBox
	EnableWindow(GetDlgItem(IDC_COMBO_LEVEL), !bTesting);

	// spin buttons
	EnableWindow(GetDlgItem(IDC_SPIN_START), !bTesting);
	EnableWindow(GetDlgItem(IDC_SPIN_DURATION), !bTesting);
	EnableWindow(GetDlgItem(IDC_SPIN_DUR_UNIT), !bTesting);
	EnableWindow(GetDlgItem(IDC_SPIN_PICKI), !bTesting);
	EnableWindow(GetDlgItem(IDC_SPIN_RMSI), !bTesting);

	// Static text
	RedrawWindow(GetDlgItem(IDC_DUR_UNITS_TEXT));
	RedrawWindow(GetDlgItem(IDD_PQF_LINE_VACTUAL));
	RedrawWindow(GetDlgItem(IDD_PQF_LINE_FREQACTUAL));
	RedrawWindow(GetDlgItem(IDD_PQF_ACTUAL_RMSI));
	RedrawWindow(GetDlgItem(IDD_PQF_ACTUAL_PEAKI));
	RedrawWindow(GetDlgItem(IDD_PQF_LINE_VNOMINAL));
	RedrawWindow(GetDlgItem(IDD_PQF_LINE_FREQNOMINAL));
	RedrawWindow(GetDlgItem(IDC_INRUSH_POS));
	RedrawWindow(GetDlgItem(IDC_INRUSH_NEG));

	// Run button bitmap
	SendMessage(GetDlgItem(IDD_PQF_RUN), BM_SETCHECK, (bTesting ? 1 : 0), 0L);

	// Stop and exit buttons
	EnableWindow(GetDlgItem(IDD_PQF_STOP), bTesting);
	EnableWindow(GetDlgItem(IDD_ECAT_EXIT), !bTesting);

	// Qualify button
	EnableWindow(GetDlgItem(IDC_BUT_QUALIFY), !bTesting);

	// Menu
	SendMessage(GetParent(), WM_ENABLE_RUN, 0, (LPARAM) bTesting);
}

//////////////////////////////////////////////////////////////////////
void CPqfMainDlg::RunTest()
{
	if (test_status EQ TEST_STATUS_TESTING)
		return;

	pqf_lpStepListhdr.hdr_numsteps = GetListItemCnt();
	if (pqf_lpStepListhdr.hdr_numsteps == 0)
	{
		MessageBox(GetParent(), "The test plan is empty, please add test(s) to the list box and try again.", "Empty Test Plan", MB_OK | MB_ICONINFORMATION);
		return;
	}

	if (!PqfValidateTest())
		return;

	HWND hDlg = GetHWND();
	
	testmgr_bSeti = TRUE; /* assume in touch with ECAT */
	if (!testmgr_bSimulate) 
	{
		if (!TestmgrReadStatus (hDlg, TESTMGR_CALLER_RUNTEST)) 
		{
			ForceClosure();
			return;
		}

		if (testmgr_eut_status EQ TESTMGR_EUT_OFF) 
		{
			UtilMessageBox (hDlg, "EUT power is OFF; testing requires power to be ON.\015\012To turn on EUT power, press button in EUT section at lower right,\015\012then cycle the EUT power button on the E100", common_errcaption, MB_OK | MB_ICONINFORMATION);
			return;
		}
		else if (testmgr_eut_status EQ TESTMGR_EUT_RDY) 
		{
			UtilMessageBox (hDlg, "EUT power is RDY but not yet ON; testing requires power to be ON.\015\012To turn on EUT power from the RDY state, cycle the EUT power\015\012button on the E100", common_errcaption, MB_OK | MB_ICONINFORMATION);
			return;
		}
	}

	CEcatApp::BeginWaitCursor();
	testlen = TestmgrCreateTest();
	CEcatApp::EndWaitCursor();
	if (testlen == 0)
		return;

	// Log begin
	if (Log.log_on()) 
	{
		if (!Log.start_log()) //Log Failed 
			return;

		UpdateWindow(hDlg);
	}

	if (Log.log_on()) 
	{
		Log.logtime = FALSE ;
		char szBuf [256] ;
		Log.loglpstr("[Test Specification]\r\n");

		char szSimulation [32] ;
		lstrcpy (szSimulation, "Simulation Mode: ") ;
		char szMode [16] ;
		if (testmgr_bSimulate) 
			lstrcpy (szMode, "On\r\n");
		else
			lstrcpy (szMode, "Off\r\n");
		lstrcat (szSimulation, szMode) ;
		Log.loglpstr(szSimulation);

		char szRepeat [32] ;
		lstrcpy (szRepeat, "Repeat State: ") ;
		char szRepeatState [16] ;
		UINT nID = IsDlgButtonChecked (hDlg, IDD_PQF_TESTREPEAT);
		if (nID > 0)
			lstrcpy (szRepeatState, "On\r\n");
		else
			lstrcpy (szRepeatState, "Off\r\n");
		lstrcat (szRepeat, szRepeatState) ;
		Log.loglpstr(szRepeat);

		if (bIsEP3Avail)
		{
			char szPhase [32] ;
			lstrcpy (szPhase, "Phase Selected: ") ;
			char szPhaseState [16] ;
			nID = -1 ;
			for (int i = IDD_PQF_PHASEL1 ; i <= IDD_PQF_PHASEL3 ; i ++)
			{
				if (IsDlgButtonChecked (hDlg, i))
				{
					nID = i;
					break; 
				}
			}

			switch (nID)
			{
				case IDD_PQF_PHASEL1: lstrcpy (szPhaseState, "L1\r\n"); break ;
				case IDD_PQF_PHASEL2: lstrcpy (szPhaseState, "L2\r\n"); break ;
				case IDD_PQF_PHASEL3: lstrcpy (szPhaseState, "L3\r\n"); break ;
				default:              lstrcpy (szPhaseState, "\r\n");	break ;
			}

			lstrcat (szPhase, szPhaseState) ;
			Log.loglpstr(szPhase);
		}

		Log.loglpstr("Step        Level       Start       Duration    RMS Limit   Peak Limit\r\n");
		for (int i = 0; i < pqf_lpStepListhdr.hdr_numsteps; i ++)
		{
			LPPQFSTEP pPqfStep = GetListItemData(i);
			char szDur [PQF_COLUMN_LEN] ;
			switch (pPqfStep->step_duration_units) 
			{
			case PQF_DURATION_UNITS_MIN:
				sprintf (szDur, "%3d:%02d", pPqfStep->step_duration / 60, pPqfStep->step_duration % 60);
				break;
		
			case PQF_DURATION_UNITS_SEC:
				sprintf (szDur, "%3d.%1d", pPqfStep->step_duration / 100, (pPqfStep->step_duration / 10) % 10);
				break;

			case PQF_DURATION_UNITS_CYC:
				sprintf (szDur, "%d", pPqfStep->step_duration);
				break;
			} /* switch on duration units */

			char szRMSLimit [PQF_COLUMN_LEN + 1] ;
			sprintf (szRMSLimit, "%2d.%1d A", pPqfStep->step_rms_ilim / 10, pPqfStep->step_rms_ilim % 10);
			PqfPadSpaces(szRMSLimit, PQF_COLUMN_LEN) ;

			char szPeakLimit [PQF_COLUMN_LEN + 1] ;
			sprintf (szPeakLimit, "%d A", pPqfStep->step_peak_ilim);
			PqfPadSpaces(szPeakLimit, PQF_COLUMN_LEN) ;
			
			char szStep [PQF_COLUMN_LEN + 1] ;
			sprintf (szStep, "%d", i) ; 
			PqfPadSpaces(szStep, PQF_COLUMN_LEN) ;
			
			char szLevel [PQF_COLUMN_LEN + 1] ;
			sprintf (szLevel, "%s", pqf_level_display[pPqfStep->step_level]);
			PqfPadSpaces(szLevel, PQF_COLUMN_LEN) ;
			
			char szStart [PQF_COLUMN_LEN + 1] ;
			sprintf (szStart, "%d dg", pPqfStep->step_start) ;
			PqfPadSpaces(szStart, PQF_COLUMN_LEN) ;
			
			char szDuration [PQF_COLUMN_LEN + 1] ;
			sprintf (szDuration, "%s%s", szDur, pqf_duration_units[pPqfStep->step_duration_units]) ;
			PqfPadSpaces(szDuration, PQF_COLUMN_LEN) ;

			sprintf (szBuf, 
					"%s%s%s%s%s%s\r\n", 
					szStep,
					szLevel,
					szStart,
					szDuration,
					szRMSLimit,
					szPeakLimit);
			Log.loglpstr((LPSTR)szBuf);
		}
		Log.logtime = TRUE ;
	}
	// Log end

	pqf_lpStepListhdr.hdr_standby_level = GetStandbyLevel();

	// 03/12/95 (JMM)	if added to keep PHASE command from being sent if
	//					no EP3 is installed in the system.
	// 11/1/96  (JFL)	EP3 operation moved here from SwitchMode function.
	if (bIsEP3Avail)
	{
		m_byPhase = GetPhase();
		SendMessage(testmgr_hWndComm, KT_PQF_SET_PHASE, (WPARAM) m_byPhase, 0L);
	}

	// Log begin
	nLineVoltage = -1 ;
	nFrequency   = -1 ;
	nRMS         = -1 ;
	nPeak        = -1 ;
	if (Log.log_on()) 
	{
		Log.logtime = FALSE ;
		char szBuf [256] ;
		Log.loglpstr("[Test Execution]\r\n");
		sprintf (szBuf, "%Fs:  Test started\r\n", (LPSTR)Log.getlogtime()) ;
		Log.loglpstr((LPSTR)szBuf);
		Log.loglpstr("Time        Voltage     Frequency   RMS         Peak\r\n");
		Log.logtime = TRUE ;
	}
	// Log end

	if (testmgr_bSimulate) 
	{
		testmgr_bShowStatus = TRUE;
		SimulateStatusUpdate(TRUE);

		test_status = TEST_STATUS_TESTING;
		nRunMode = RUN_MODE;
		StartTimer(TIMER1, 250);
		if (test_status EQ TEST_STATUS_PAUSED) /* resume */
			return;

		pqfdlg_bListBoxLeft = TRUE;
		test_stepnum = 1;
		test_stepi = 0;
		test_dwDegreesUsedInStep = 0L;

		EnableControls(TRUE/*testing*/);
		return;
	}

	test_stepnum = 1;
	PqfUpdateTestStatus (TRUE);
	test_status = TEST_STATUS_TESTING;
	nRunMode = RUN_MODE;
	EnableControls(TRUE/*testing*/);
	
	if (SendMessage (testmgr_hWndComm, KT_PQF_SEND_LIST, (WPARAM) st_lpPqfTestPacketList, testlen) ||
		SendMessage (testmgr_hWndComm, KT_PQF_START, 0, 0L))
	{
		UtilMessageBox(hDlg, "Failure on sending the test list and starting the test: KT_PQF_SEND_LIST or KT_PQF_START command", common_errcaption, MB_OK | MB_ICONWARNING);
		test_status = TEST_STATUS_DONE;
		nRunMode = IDLE_MODE;
		EnableControls(FALSE/*not testing*/);
		PqfSetTimeLeft(0, TRUE);
		TestmgrKludge();
		return;
	}
}

//////////////////////////////////////////////////////////////////////
void CPqfMainDlg::StopTest()
{
	if ((test_status EQ TEST_STATUS_DONE) || (test_status EQ TEST_STATUS_PRESTART))
		return;

	pqfdlg_bListBoxLeft = FALSE;
	HWND hDlg = GetHWND();
	if (testmgr_bSimulate) 
	{
		StopTimer();
	}
	else 
	{
		if (SendMessage (testmgr_hWndComm, KT_PQF_STOP, 0, 0L)) 
		{
			UtilMessageBox (hDlg, "ALERT ALERT: Unable to communicate with ECAT to stop testing!!!", common_errcaption, MB_OK);
			return;
		}
	}

	TestmgrDestroyTest();
	pqf_lpStepListhdr.hdr_numsteps = 0;
	test_status = TEST_STATUS_DONE;
	nRunMode = IDLE_MODE;

	int nStopStep = test_stepnum ;
	PqfUpdateTestStatus (FALSE);
	PqfSetTimeLeft(0, TRUE);

	sprintf (common_message, (LPSTR) "Test Halted\015\012Standby voltage level resumed (%s)", pqf_level_display[pqf_lpStepListhdr.hdr_standby_level]);

	// LOG begin
	if (Log.log_on()) 
	{
		Log.logtime = FALSE ;
		char szBuf [256] ;
		sprintf (szBuf, "%Fs: Step %d %s\r\n", (LPSTR)Log.getlogtime(), nStopStep, common_message);
		Log.loglpstr((LPSTR)szBuf);
		Log.comment((LPSTR) NULL);
		Log.logtime = TRUE ;
		Log.end_log();
	}
	else
		UtilMessageBox (hDlg, common_message, (char*) CPqfApp::GetAppName(), MB_OK | MB_ICONINFORMATION);
	// LOG end

	EnableControls(FALSE/*not testing*/);
	TestmgrKludge();
}

//////////////////////////////////////////////////////////////////////
static void File2Data(HANDLE hFile, LPVOID pData, DWORD dwSize)
{
	DWORD dwByteCnt = dwSize;
	if (ReadFile(hFile, pData, dwSize, &dwByteCnt, NULL) && (dwByteCnt == dwSize))
		return;

	throw CSaveLoadEx();
}

//////////////////////////////////////////////////////////////////////
static void Data2File(HANDLE hFile, LPCVOID pData, DWORD dwSize)
{
	DWORD dwByteCnt = dwSize;
	if (WriteFile(hFile, pData, dwSize, &dwByteCnt, NULL) && (dwByteCnt == dwSize))
		return;

	throw CSaveLoadEx();
}

//////////////////////////////////////////////////////////////////////
BOOL CPqfMainDlg::Load(HANDLE hFile)
{
	CEcatApp::BeginWaitCursor();
	BOOL bOK = FALSE;
	try
	{
		PQF_FILE_HDR fileHdr;
		memset(&fileHdr, 0, sizeof(fileHdr));

		// Read the header first
		File2Data(hFile, &fileHdr, sizeof(fileHdr));
		
		// See what type of file we are dealing with
		DWORD dwVersion = 200;
		DWORD dwItemCnt = 0;
		if (fileHdr.dwCookie == PQF_FILE_COOKIE)
		{
			// New file, proceed
			LPPQFSTEP pPqfStep = NULL;
			try
			{
				dwVersion = fileHdr.dwVersion;
				File2Data(hFile, &dwItemCnt, sizeof(dwItemCnt));
				for (DWORD i = 0; i < dwItemCnt; i++)
				{
					pPqfStep = new PQFSTEP;
					if (pPqfStep == NULL)
							throw CSaveLoadEx();
					File2Data(hFile, pPqfStep, sizeof(PQFSTEP));

					// Set the parameters and attach them to the listview control's entry
					LVITEM lvItem;
					memset(&lvItem, 0, sizeof(lvItem));
					lvItem.mask = LVIF_PARAM | LVIF_STATE;
					lvItem.iItem = i;
					lvItem.stateMask = (UINT) -1;
					lvItem.state = LVIS_SELECTED;
					lvItem.lParam = (LPARAM) pPqfStep;
					SendItemMsg(IDD_PQF_LIST, LVM_INSERTITEM, 0, (LPARAM) &lvItem);
					pPqfStep = NULL;
					TestStep2ListItem(i, *((LPPQFSTEP) lvItem.lParam));
				}

				File2Data(hFile, &pqf_lpStepListhdr.hdr_standby_level, sizeof(pqf_lpStepListhdr.hdr_standby_level));
				File2Data(hFile, &m_byPhase, sizeof(m_byPhase));
				File2Data(hFile, &m_bRepeatTest, sizeof(m_bRepeatTest));
			}
			catch(...)		// memory and stuff
			{
				if (pPqfStep != NULL)
					delete pPqfStep;

				// DeleteContext() will be called in the base class to clean up 
				throw CSaveLoadEx();
			}
		}
		else
		{
			// Old file - rewind to the beginning of the file and read the data again
			SetFilePointer(hFile, 0, NULL, FILE_BEGIN);
			REV_101_FILE_HDR rev101Hdr;
			File2Data(hFile, &rev101Hdr, sizeof(rev101Hdr));
			dwVersion = rev101Hdr.file_revision;
			dwItemCnt = rev101Hdr.file_pqf_numsteps - 1;
			if (rev101Hdr.file_pqf_numsteps < 1)
				throw CSaveLoadEx();

			// Since in the old format first step was the header and subsequent steps
			// where not in order we have to rearrange the steps and skip the header.
			LPPQFSTEP_16 pPqfOldStepList = NULL;
			LPPQFSTEP pPqfStep = NULL;
			try
			{
				// Read old test plan list as one memory block
				pPqfOldStepList = new PQFSTEP_16[rev101Hdr.file_pqf_numsteps];
				if (pPqfOldStepList == NULL)
					throw CSaveLoadEx();

				File2Data(hFile, pPqfOldStepList, rev101Hdr.file_pqf_numsteps * sizeof(PQFSTEP_16));
				LPPQFHEADER_16 pOldListHdr = (LPPQFHEADER_16) pPqfOldStepList;
				pqf_lpStepListhdr.hdr_standby_level = pOldListHdr->hdr_standby_level;

				LPPQFSTEP_16 pPqfStep_16 = NULL;
				WORD wOldStep = pOldListHdr->hdr_next;
				for (DWORD dwStep = 0; dwStep < dwItemCnt; dwStep++)
				{
					LPPQFSTEP pPqfStep = new PQFSTEP;
					if (pPqfStep == NULL)
						throw CSaveLoadEx();

					*pPqfStep = pPqfOldStepList[wOldStep];

					// Set the parameters and attach them to the listview control's entry
					LVITEM lvItem;
					memset(&lvItem, 0, sizeof(lvItem));
					lvItem.mask = LVIF_PARAM | LVIF_STATE;
					lvItem.iItem = (int) dwStep;
					lvItem.stateMask = (UINT) -1;
					lvItem.state = LVIS_SELECTED;
					lvItem.lParam = (LPARAM) pPqfStep;
					SendItemMsg(IDD_PQF_LIST, LVM_INSERTITEM, 0, (LPARAM) &lvItem);
					pPqfStep = NULL;
					TestStep2ListItem(lvItem.iItem, *((LPPQFSTEP) lvItem.lParam));
					wOldStep = pPqfOldStepList[wOldStep].step_next;
				}

				// Default value
				m_byPhase = 1;	// L1

				// Get rid of the old style step list
				delete[] pPqfOldStepList;
			}
			catch(...)		// memory and stuff
			{
				if (pPqfOldStepList != NULL)
					delete[] pPqfOldStepList; 

				if (pPqfStep != NULL)
					delete pPqfStep;
				throw CSaveLoadEx();
			}

		}

		// If standby is 0, and we have EP3, tell user that it is unsuitable
		if (bIsEP3Avail)
		{
			if (pqf_lpStepListhdr.hdr_standby_level == PQF_LEVEL_OPEN)
			{
				Message("Invalid Option", "This test file calls for 0 standby power.\nThis is illegal with EP3 and will not be used.");
				pqf_lpStepListhdr.hdr_standby_level = PQF_LEVEL_100;
			}
		}
		else
			m_byPhase = 1;

		pqf_lpStepListhdr.hdr_numsteps = (short) dwItemCnt;
		UpdateData(FALSE);
		DWORD dwTotDegrees = PqfSetGraph(GetHWND(), 0);
		SelectListItem(0);
		PqfSetTotalTime(dwTotDegrees);
		bOK = TRUE;
	}
	catch(CSaveLoadEx ex)
	{
		Message("File Read Error", "Error reading test plan file - bad format.");
	}

	CEcatApp::EndWaitCursor();
	return bOK;
}

//////////////////////////////////////////////////////////////////////
void CPqfMainDlg::Save(HANDLE hFile)
{
	CEcatApp::BeginWaitCursor();

	try
	{
		// Get the standby level and the phase
		if (bIsEP3Avail)
		{
			m_byPhase = GetPhase();
			pqf_lpStepListhdr.hdr_standby_level = PQF_LEVEL_100;	// always 100% for EP3
		}
		else
		{
			m_byPhase = 1;	// L1
			pqf_lpStepListhdr.hdr_standby_level = GetStandbyLevel();
		}
		m_bRepeatTest = GetRepeatFlag();

		// Write the header first
		PQF_FILE_HDR fileHdr;
		Data2File(hFile, &fileHdr, sizeof(fileHdr));

		// Now the data
		DWORD dwItemCnt = GetListItemCnt();
		Data2File(hFile, &dwItemCnt, sizeof(dwItemCnt));

		for (DWORD i = 0; i < dwItemCnt; i++)
		{
			LPPQFSTEP pPqfStep = GetListItemData(i);
			Data2File(hFile, pPqfStep, sizeof(PQFSTEP));
		}

		Data2File(hFile, &pqf_lpStepListhdr.hdr_standby_level, sizeof(pqf_lpStepListhdr.hdr_standby_level));
		Data2File(hFile, &m_byPhase, sizeof(m_byPhase));
		Data2File(hFile, &m_bRepeatTest, sizeof(m_bRepeatTest));
	}
	catch(CSaveLoadEx ex)
	{
		Message("File Write Error", "Error [0x%08X] writing test plan data to the file.", GetLastError());
	}

	CEcatApp::EndWaitCursor();
}

//////////////////////////////////////////////////////////////////////
void CPqfMainDlg::DeleteContext()
{
	// Set default values
	m_iStartPhase = PQF_START_DEF;
	m_iDurUnits = PQF_DURATION_UNITS_SEC;
	m_iRmsI = PQF_RMSI_DEF;			// 10.0 A
	m_iPeakI = PQF_PEAKI_DEF;		// 100 A
	m_byPhase = 1;					// L1
	m_bRepeatTest = FALSE;

	pqf_lpStepListhdr.hdr_numsteps = 0;
	pqf_lpStepListhdr.hdr_standby_level = PQF_LEVEL_100;

	DeleteAllSteps();
	PqfSetGraph(GetHWND(), -1);
	UpdateData(FALSE);

	changed_since_save = FALSE;
}

//////////////////////////////////////////////////////////////////////
void CPqfMainDlg::ForceClosure()
{
	// We are here because the communications failed with ECAT, number of errors exceeded 
	// allowed number, or the user chose to abort the app.
	StopTimer();
	test_status = TEST_STATUS_DONE;
	nRunMode = IDLE_MODE;
	changed_since_save = FALSE;
	EUT_POWER = 0;
	MessageBox(GetHWND(), "Unable to communicate with ECAT. The application will now quit.", "Fatal Error", MB_OK | MB_ICONERROR);
	PostMessage(hWnd_pqf, WM_CLOSE, 0, 0L);
}

//////////////////////////////////////////////////////////////////////
void CPqfMainDlg::SimulateStatusUpdate(BOOL bStart)
{
    STATIC short oldStepNumSim = -1;
	PqfCalcTimeLeft();
    if (bStart || (test_status == TEST_STATUS_DONE)) 
	{
        test_stepnum = 1;
        SelectListItem(test_stepnum - 1);
		SendItemMsg(IDD_PQF_LIST, LVM_ENSUREVISIBLE, (WPARAM) test_stepnum - 1, 0);						
		PqfGraphPickStep(GetDlgItem(IDD_PQF_GRAPH), test_stepnum - 1);
        oldStepNumSim = test_stepnum;

    } 
	else if (test_stepnum != oldStepNumSim) 
	{
        SelectListItem(test_stepnum - 1);
		SendItemMsg(IDD_PQF_LIST, LVM_ENSUREVISIBLE, (WPARAM) test_stepnum - 1, 0);						
        PqfGraphPickStep(GetDlgItem(IDD_PQF_GRAPH), test_stepnum - 1);
        oldStepNumSim = test_stepnum;
    }

	// LOG begin
	LogStatusUpdate();
	// LOG end

	double dRMSI = (double) testmgr_PqfStatus.pqfs_rms_iread / 10.0;
	Dbl2Text(IDD_PQF_ACTUAL_RMSI, dRMSI);
	Int2Text(IDD_PQF_ACTUAL_PEAKI, testmgr_PqfStatus.pqfs_peak_iread);
}

//////////////////////////////////////////////////////////////////////
void CPqfMainDlg::LogStatusUpdate()
{
	if ((testmgr_PqfStatus.pqfs_line_voltage != nLineVoltage)  ||
		(testmgr_PqfStatus.pqfs_line_freq    != nFrequency)    ||
		(testmgr_PqfStatus.pqfs_rms_iread    !=	nRMS)      	  ||
		(testmgr_PqfStatus.pqfs_peak_iread   !=	nPeak))
	{
		nLineVoltage	= testmgr_PqfStatus.pqfs_line_voltage ;
		nFrequency  	= testmgr_PqfStatus.pqfs_line_freq    ;
		nRMS        	= testmgr_PqfStatus.pqfs_rms_iread    ;	
		nPeak       	= testmgr_PqfStatus.pqfs_peak_iread   ;	

		if (Log.log_on()) 
		{					 
			Log.logtime = FALSE ;
			char szBuf [256] ;
			char szTime [PQF_COLUMN_LEN + 1] ;
			sprintf (szTime, "%Fs:", (LPSTR)Log.getlogtime()) ;
			PqfPadSpaces(szTime, PQF_COLUMN_LEN) ;

			char szVoltage [PQF_COLUMN_LEN + 1] ;
			sprintf (szVoltage, "%3d VAC", nLineVoltage) ;
			PqfPadSpaces(szVoltage, PQF_COLUMN_LEN) ;
			
			char szFreq [PQF_COLUMN_LEN + 1] ;
			sprintf (szFreq, "%2d.%1d Hz", nFrequency /10, nFrequency %10);
			PqfPadSpaces(szFreq, PQF_COLUMN_LEN) ;
			
			char szRMS [PQF_COLUMN_LEN + 1] ;
			sprintf (szRMS, "%2d.%1d A", nRMS / 10, nRMS % 10);
			PqfPadSpaces(szRMS, PQF_COLUMN_LEN) ;
			
			char szPeak [PQF_COLUMN_LEN + 1] ;
			sprintf (szPeak, "%d A", nPeak);
			PqfPadSpaces(szPeak, PQF_COLUMN_LEN) ;

			sprintf (szBuf, "%s%s%s%s%s\r\n", 
						szTime,
						szVoltage,
						szFreq,
						szRMS,
						szPeak);
			Log.loglpstr((LPSTR)szBuf);
			Log.logtime = TRUE ;
		}
	}	 		 
}

//////////////////////////////////////////////////////////////////////
DWORD CPqfMainDlg::PqfUtilStep2DurationDegrees(const PQFSTEP& pPqfStep)
{
    DWORD dwTotDegrees;
    switch (pPqfStep.step_duration_units) 
	{
        case PQF_DURATION_UNITS_MIN:
            dwTotDegrees = pPqfStep.step_duration * pqf_dwDegreesPerSecond;
            break;
        case PQF_DURATION_UNITS_SEC:
            dwTotDegrees = (pPqfStep.step_duration * pqf_dwDegreesPerSecond) / 100L;
            break;
        case PQF_DURATION_UNITS_CYC:
            dwTotDegrees = (DWORD) pPqfStep.step_duration * PQF_DEGREES_PER_CYCLE;
            break;
    }

	int next = test_stepi + 1;
	if (next < GetListItemCnt())
	{
		LPPQFSTEP pPqfStep = GetListItemData(next);
		if (pPqfStep != NULL)
			dwTotDegrees += pPqfStep->step_start;
	}

    return (dwTotDegrees);
}

//////////////////////////////////////////////////////////////////////
void CPqfMainDlg::PqfTestNextStep()
{
	static BOOL testrun_bExecuting = FALSE;

    if (testrun_bExecuting) return;
    testrun_bExecuting = TRUE;

	LPPQFSTEP pPqfStep = GetListItemData(test_stepi);
	if (pPqfStep == NULL)
	{
		Message("Invalid Data", "Test plan is invalid. Simulation has been stopped.");
		PqfDlgTestCompleted();
	    testrun_bExecuting = FALSE;
		return;
	}

    DWORD dwStepDegrees = PqfUtilStep2DurationDegrees(*pPqfStep);
    DWORD dwDegrees = dwStepDegrees - test_dwDegreesUsedInStep;
    dwDegrees = kmin (dwDegrees, MAX_DEGREES_PER_STEP);
    DWORD dwmSec = (DWORD) ((dwDegrees * 1000L) / pqf_dwDegreesPerSecond);

    /* do step */
    testmgr_PqfStatus.pqfs_rms_iread = pPqfStep->step_rms_ilim * 7 / 10;
    testmgr_PqfStatus.pqfs_peak_iread = pPqfStep->step_peak_ilim * 7 / 10;
    
    test_dwDegreesUsedInStep += dwDegrees;
    if (test_dwDegreesUsedInStep >= dwStepDegrees) 
	{
        test_stepi++;
        if (test_stepi == GetListItemCnt()) 
		{	
			// We are done 
            if (m_bRepeatTest) 
			{
                SimulateStatusUpdate(TRUE);
                test_status = TEST_STATUS_TESTING;
                test_stepnum = 1;
                test_stepi = 0;
                test_dwDegreesUsedInStep = 0L;
            }
            else
                PqfDlgTestCompleted();
        }
        else 
		{
            test_dwDegreesUsedInStep = 0L;
            test_stepnum = test_stepi + 1;
        }
    }

    testrun_bExecuting = FALSE;
}

//////////////////////////////////////////////////////////////////////
void CPqfMainDlg::Qualify()
{
	if (m_bQualifying) return;

	HWND hDlg = GetHWND();
	if (testmgr_bSimulate) 
	{
		UtilMessageBox(hDlg, "Inrush current qualification is not available in the simulation mode.", "Not Available", MB_OK | MB_ICONINFORMATION);
		return;
	}
	
	if (testmgr_eut_status EQ TESTMGR_EUT_OFF) 
	{
		UtilMessageBox(hDlg, "EUT power is OFF; qualification requires power to be ON.\015\012To turn on EUT power, press button in EUT section at lower right,\015\012then cycle the EUT power button on the E100", common_errcaption, MB_OK | MB_ICONINFORMATION);
		return;
	}
	else if (testmgr_eut_status EQ TESTMGR_EUT_RDY) 
	{
		UtilMessageBox(hDlg, "EUT power is RDY but not yet ON; qualification requires power to be ON.\015\012To turn on EUT power from the RDY state, cycle the EUT power\015\012button on the E100", common_errcaption, MB_OK | MB_ICONINFORMATION);
		return;
	}

	m_bQualifying = TRUE;

	SetDlgItemText(hDlg, IDC_INRUSH_POS, common_nullstr);
	SetDlgItemText(hDlg, IDC_INRUSH_NEG, common_nullstr);

	HWND hParent = GetParent();
	HWND hWaitDlg = CreateDialog(GetInst(), MAKEINTRESOURCE(IDD_QUALIFYING), hParent, (DLGPROC) DefWindowProc);
	if (hWaitDlg)
	{
		EnableWindow(hParent, FALSE);
		ShowWindow(hWaitDlg, SW_SHOW);
		UpdateWindow(hWaitDlg);
	}

	CEcatApp::BeginWaitCursor();
	
	nRunMode = RUN_MODE;	// so we won't accidently exit
	EnableControls(TRUE);
	EnableWindow(GetDlgItem(IDD_PQF_RUN), FALSE);
	EnableWindow(GetDlgItem(IDD_PQF_STOP), FALSE);

	LONG lPeak = SendMessage (testmgr_hWndComm, KT_PQF_QUALIFY, 0, 0L);

	EnableControls(FALSE);
	EnableWindow(GetDlgItem(IDD_PQF_RUN), TRUE);
	nRunMode = IDLE_MODE;

	CEcatApp::EndWaitCursor();

	if (hWaitDlg)
	{
		EnableWindow(hParent, TRUE);
		DestroyWindow(hWaitDlg);
		UpdateWindow(hParent);
	}

	if (lPeak != -1L) 
	{
		WORD pospeak = LOWORD(lPeak);
		WORD negpeak = HIWORD(lPeak);
		Int2Text(IDC_INRUSH_POS, pospeak);
		Int2Text(IDC_INRUSH_NEG, negpeak);
	}
	else
		MessageBox(hDlg, "Unable to qualify", common_errcaption, MB_OK | MB_ICONWARNING);

	m_bQualifying = FALSE;

	RedrawWindow(GetDlgItem(IDC_INRUSH_POS));
	RedrawWindow(GetDlgItem(IDC_INRUSH_NEG));
}

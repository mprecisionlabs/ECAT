//////////////////////////////////////////////////////////////////////
// PqfMainWnd.cpp: implementation of the CPqfMainWnd class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "PqfMainWnd.h"
#include "PqfMainDlg.h"
#include "..\core\about.h"
#include "..\core\globals.h"
#include "common.h"
#include "revstr.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CPqfMainWnd::CPqfMainWnd()
{

}

//////////////////////////////////////////////////////////////////////
CPqfMainWnd::~CPqfMainWnd()
{

}

//////////////////////////////////////////////////////////////////////
BOOL CPqfMainWnd::DoExit()
{
	return CEcatMainWnd::DoExit();
}

//////////////////////////////////////////////////////////////////////
int CPqfMainWnd::OnCreate(HWND hWnd, LPCREATESTRUCT lpCS)
{
	// Some special initialization to make the app backward compatible with the old code
	hWnd_pqf = hWnd;

    // Reset status to zero if in simulation mode
	if (testmgr_bSimulate) 
	{
        testmgr_bDebug = FALSE;
		testmgr_PqfStatus.pqfs_nIlock = 0;
		testmgr_PqfStatus.pqfs_rms_iread = 0;
		testmgr_PqfStatus.pqfs_peak_iread = 0;
		testmgr_PqfStatus.pqfs_step_number = 0;
		testmgr_PqfStatus.pqfs_eut_status = TESTMGR_EUT_OFF;
		testmgr_PqfStatus.pqfs_line_voltage = 0;
		testmgr_PqfStatus.pqfs_line_freq = 0;
		testmgr_PqfStatus.pqfs_error = 0;
    }

	m_pDlg = new CPqfMainDlg();
	if (!m_pDlg) return -1;
	
	if (!m_pDlg->Create(IDD_PQFDLG, hWnd))
	{
		delete m_pDlg;
		m_pDlg = NULL;
		return -1;
	}

	return 0;
}

//////////////////////////////////////////////////////////////////////
// NOTE: Return 0 if the message has been processed, 1 for further 
//		 processing by the system
//////////////////////////////////////////////////////////////////////
int CPqfMainWnd::OnCmdMsg(UINT uiNotify, UINT uiCtrlId, HWND hwndCtl)
{
	HMENU hMenu;
	switch (uiCtrlId)
	{
	case IDM_ABOUT:
		// We have to handle IDM_ABOUT here since the REVSTR is different
		show_about(GetInst(), GetHWND(), REVSTR);
		break;

    case IDM_PQF_GRAPH_ENABLEZOOMIN:
        ModifyMenu(GetMenu(GetHWND()), (WPARAM) uiCtrlId, MF_BYCOMMAND | MF_STRING, IDM_PQF_GRAPH_DISABLEZOOMIN, "Cancel Zoom &In");
        PqfGraphEnableLeftMouseZooming(GetGraphHWND(), TRUE);
        break; /* IDM_PQF_GRAPH_ENABLEZOOMIN */

    case IDM_PQF_GRAPH_DISABLEZOOMIN:
        ModifyMenu(GetMenu(GetHWND()), (WPARAM) uiCtrlId, MF_BYCOMMAND | MF_STRING, IDM_PQF_GRAPH_ENABLEZOOMIN, "Zoom &In");
        PqfGraphEnableLeftMouseZooming(GetGraphHWND(), FALSE);
        break; /* IDM_PQF_GRAPH_DISABLEZOOMIN */

    case IDM_PQF_GRAPH_ZOOMOUT:
		hMenu = GetMenu(GetHWND());
        PostMessage (GetGraphHWND(), WM_LBUTTONDBLCLK, 0, 0L);
        break; /* IDM_PQF_GRAPH_ZOOMOUT */

	case IDM_PQF_QUALIFICATION:
		if (m_pDlg)
			SendMessage(m_pDlg->GetHWND(), WM_COMMAND, IDC_BUT_QUALIFY, 0);
		break;

	default:
		return CEcatMainWnd::OnCmdMsg(uiNotify, uiCtrlId, hwndCtl);
	}

	return 0;
}

//////////////////////////////////////////////////////////////////////
LRESULT CALLBACK CPqfMainWnd::LocalWndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	// Local window procedure
	switch (msg) 
	{
		case WM_INITMENU:
			EnableMenuItem((HMENU) wParam, IDM_PQF_GRAPH_ZOOMOUT, 
				(IsWindowEnabled(GetDlgItem(GetGraphHWND(), IDD_PQFGRAPH_SCROLL))) ? MF_ENABLED : MF_GRAYED);
			break; /* WM_INITMENU */

		default:
			return CEcatMainWnd::LocalWndProc(hWnd, msg, wParam, lParam);
   }

   return 0;
}



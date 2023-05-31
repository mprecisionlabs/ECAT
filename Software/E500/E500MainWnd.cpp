//////////////////////////////////////////////////////////////////////
// E500MainWnd.cpp: implementation of the CE500MainWnd class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "e500.h"
#include "E500App.h"
#include "E500MainWnd.h"
#include "E500MainDlg.h"
#include "revstr.h"
#include "..\core\globals.h"
#include "..\core\about.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CE500MainWnd::CE500MainWnd()
{
}

//////////////////////////////////////////////////////////////////////
CE500MainWnd::~CE500MainWnd()
{
}

//////////////////////////////////////////////////////////////////////
int CE500MainWnd::OnCreate(HWND hWnd, LPCREATESTRUCT lpCS)
{
	m_pDlg = new CE500MainDlg();
	if (!m_pDlg) return -1;
	
	if (!m_pDlg->Create(IDD_E500DLG, hWnd))
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
int CE500MainWnd::OnCmdMsg(UINT uiNotify, UINT uiCtrlId, HWND hwndCtl)
{
	// Handle all menu notifications here 
	switch (uiCtrlId)
	{
	case IDM_ABOUT:
		// We have to handle IDM_ABOUT here since the REVSTR is different
		show_about(GetInst(), GetHWND(), REVSTR);
		break;

	default:
		return CEcatMainWnd::OnCmdMsg(uiNotify, uiCtrlId, hwndCtl);
	}

	return 0;
}



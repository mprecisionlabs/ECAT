// MultiStateBtn.h: interface for the CMultiStateBtn class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MULTISTATEBTN_H__E9531BCC_5E79_11D6_B321_00B0D0DE3525__INCLUDED_)
#define AFX_MULTISTATEBTN_H__E9531BCC_5E79_11D6_B321_00B0D0DE3525__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define MAX_BTN_STATES			10

class CMultiStateBtn  
{
public:
	CMultiStateBtn();
	virtual ~CMultiStateBtn();

public:
	BOOL Subclass(HWND hParent, UINT uiId, HINSTANCE hInst, UINT* pImageIDs, UINT uiImgCnt = 1);
	int GetState() {return m_iState;}

private:
	HWND m_hWnd;
	int m_iState;
	WNDPROC m_oldProc;
	HBITMAP m_arrBmps[MAX_BTN_STATES];
	HBITMAP m_arrBmpsDis[MAX_BTN_STATES];

private:
	void SetState(int iState);
	static LRESULT CALLBACK CommonBtnProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	virtual LRESULT CALLBACK BtnProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	void OnDraw(LPDRAWITEMSTRUCT lpDis);
	int GetValidStateCnt();
};

#endif // !defined(AFX_MULTISTATEBTN_H__E9531BCC_5E79_11D6_B321_00B0D0DE3525__INCLUDED_)

//////////////////////////////////////////////////////////////////////
// MultiStateBtn.cpp: implementation of the CMultiStateBtn class.
//
//////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "MultiStateBtn.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CMultiStateBtn::CMultiStateBtn()
{
	m_hWnd = NULL;
	m_iState = 0;
	m_oldProc = NULL;
	memset((PBYTE) m_arrBmps, 0x00, sizeof(m_arrBmps));
	memset((PBYTE) m_arrBmpsDis, 0x00, sizeof(m_arrBmpsDis));
}

//////////////////////////////////////////////////////////////////////
CMultiStateBtn::~CMultiStateBtn()
{
	for (int i = 0; i < GetValidStateCnt(); i++)
	{
		DeleteObject(m_arrBmps[i]);
		DeleteObject(m_arrBmpsDis[i]);
	}
}

//////////////////////////////////////////////////////////////////////
int CMultiStateBtn::GetValidStateCnt()
{
    int i;

	for (i = 0; i < MAX_BTN_STATES; i++)
	{
		if (m_arrBmps[i] == NULL)
			break;
	}

	return i;
}

//////////////////////////////////////////////////////////////////////
LRESULT CMultiStateBtn::CommonBtnProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	CMultiStateBtn* lpBtn = (CMultiStateBtn*) GetWindowLong(hWnd, GWL_USERDATA);
	if (lpBtn != NULL) 
		return (lpBtn->BtnProc(hWnd, message, wParam, lParam));
	
	return 0;
}

//////////////////////////////////////////////////////////////////////
static void S2C(HWND hParent, LPRECT pRect)
{
	if ((hParent == NULL) || (pRect == NULL))
		return;

	ScreenToClient(hParent, (LPPOINT) pRect);
	ScreenToClient(hParent, (LPPOINT) ((LPPOINT)pRect + 1));
}

//////////////////////////////////////////////////////////////////////
BOOL CMultiStateBtn::Subclass(HWND hParent, UINT uiId, HINSTANCE hInst, UINT* pImageIDs, UINT uiImgCnt/* = 1*/)
{
	// See if we are already subclassed
	if (m_hWnd != NULL)
		return FALSE;

	// Make sure there are some sensible values passed
	if ((pImageIDs == NULL) || (uiImgCnt == 0))
		return FALSE;

	// Get the control's handle
	if (!::IsWindow(hParent))
		return FALSE;

	m_hWnd = GetDlgItem(hParent, uiId);
	if (m_hWnd == NULL)
		return FALSE;

	// Create the images
	for (int i = 0; i < (int) uiImgCnt; i++)
	{
		//m_arrBmps[i] = LoadBitmap(hInst, MAKEINTRESOURCE(pImageIDs[i]));
		m_arrBmps[i] = (HBITMAP) LoadImage(hInst, MAKEINTRESOURCE(pImageIDs[i]), IMAGE_BITMAP, 0, 0, LR_LOADMAP3DCOLORS);
		m_arrBmpsDis[i] = (HBITMAP) LoadImage(hInst, MAKEINTRESOURCE(pImageIDs[i]), IMAGE_BITMAP, 0, 0, LR_MONOCHROME);
	}

	// Remove any unwanted flags and set owner draw
	DWORD dwStyle = GetWindowLong(m_hWnd, GWL_STYLE);
	dwStyle &= ~0x0000FFFF;
	dwStyle |= BS_OWNERDRAW;
	::SetWindowLong(m_hWnd, GWL_STYLE, dwStyle);

	// Set the pointer to 'this'
	::SetWindowLong(m_hWnd, GWL_USERDATA, (LONG) this);

	// Adjust button size to match that of the bitmap
	BITMAP bmpInfo;
	GetObject(m_arrBmps[0], sizeof(bmpInfo), &bmpInfo);
	RECT rc;
	GetWindowRect(m_hWnd, &rc);
	S2C(hParent, &rc);
	MoveWindow(m_hWnd, rc.left, rc.top, bmpInfo.bmWidth + 2, bmpInfo.bmHeight + 2, FALSE);
	
	// Hook our proc to the specified control
	m_oldProc = (WNDPROC) ::SetWindowLong(m_hWnd, GWL_WNDPROC, (LONG) CommonBtnProc);
	SetState(0);

	return TRUE;
}

//////////////////////////////////////////////////////////////////////
void CMultiStateBtn::SetState(int iState)
{
	if (m_hWnd == NULL)
		return;

	if ((iState < 0) || (iState > (GetValidStateCnt() - 1)))
		return;

	m_iState = iState;

	RECT rc;
	GetClientRect(m_hWnd, &rc);
	InvalidateRect(m_hWnd, &rc, TRUE);
	UpdateWindow(m_hWnd);
}

//////////////////////////////////////////////////////////////////////
LRESULT CMultiStateBtn::BtnProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_DRAWITEM:
		// Call our draw item method
		if ((m_hWnd != NULL) && ((LONG) wParam == GetWindowLong(m_hWnd, GWL_ID)))
			OnDraw((LPDRAWITEMSTRUCT) lParam);
		break;

	case BM_SETCHECK:
		SetState((int) wParam);
		return TRUE;

	default:
		break;		
	}

	return m_oldProc(hWnd, message, wParam, lParam);
}

//////////////////////////////////////////////////////////////////////
static void FillSolidRect(HDC hDC, LPRECT lpRect, COLORREF clr)
{
	COLORREF colOld = ::SetBkColor(hDC, clr);
	::ExtTextOut(hDC, 0, 0, ETO_OPAQUE, lpRect, NULL, 0, NULL);
	::SetBkColor(hDC, colOld);
}

//////////////////////////////////////////////////////////////////////
static void FillSolidRect(HDC hDC, int x, int y, int cx, int cy, COLORREF clr)
{
	RECT rect;
	rect.left = x;
	rect.top = y;
	rect.right = x + cx;
	rect.bottom = y + cy;
	FillSolidRect(hDC, &rect, clr);
}

//////////////////////////////////////////////////////////////////////
static void Draw3dRect(HDC hDC, int x, int y, int cx, int cy, COLORREF clrTopLeft, COLORREF clrBottomRight)
{
	FillSolidRect(hDC, x, y, cx - 1, 1, clrTopLeft);
	FillSolidRect(hDC, x, y, 1, cy - 1, clrTopLeft);
	FillSolidRect(hDC, x + cx, y, -1, cy, clrBottomRight);
	FillSolidRect(hDC, x, y + cy, cx, -1, clrBottomRight);
}

//////////////////////////////////////////////////////////////////////
static void Draw3dRect(HDC hDC, LPRECT lpRect, COLORREF clrTopLeft, COLORREF clrBottomRight)
{
	Draw3dRect(hDC, lpRect->left, lpRect->top, lpRect->right - lpRect->left,
		lpRect->bottom - lpRect->top, clrTopLeft, clrBottomRight);
}

//////////////////////////////////////////////////////////////////////
static void InflateRect(RECT& rc, int h, int v)
{
	rc.left -= h;
	rc.right += h;
	rc.top -= v;
	rc.bottom += v;
}

//////////////////////////////////////////////////////////////////////
void CMultiStateBtn::OnDraw(LPDRAWITEMSTRUCT lpDis)
{
	HDC hDC = lpDis->hDC;

	RECT rc = lpDis->rcItem;
	FillRect(hDC, &rc, (HBRUSH) (COLOR_BTNFACE + 1));

	// Now draw the button depending on the state we are in
	BOOL bDown = (lpDis->itemState & ODS_SELECTED);
	COLORREF clrTopLeft = (bDown ? GetSysColor(COLOR_3DSHADOW) : GetSysColor(COLOR_3DHILIGHT)); 
	COLORREF clrBottomRight = (bDown ? GetSysColor(COLOR_3DHILIGHT) : GetSysColor(COLOR_3DSHADOW));
	Draw3dRect(lpDis->hDC, &rc, clrTopLeft, clrBottomRight);

	// Draw the image if not out of range
	BOOL bEnabled = IsWindowEnabled(m_hWnd);
	if ((m_iState >= 0) && (m_iState < GetValidStateCnt()))
	{
		RECT rc1 = rc;
		InflateRect(rc1, -1, -1);
		if (bDown)
		{
			rc1.left++;
			rc1.top++;
		}

		DrawState(
			lpDis->hDC,
			NULL,
			NULL,
			(LPARAM) (bEnabled ? m_arrBmps[m_iState] : m_arrBmpsDis[m_iState]),
			0,
			rc1.left,
			rc1.top,
			0,
			0,
			DST_BITMAP | (bEnabled ? DSS_NORMAL : DSS_DISABLED));
	}

	if (lpDis->itemState & ODS_FOCUS)
	{
		InflateRect(rc, -1, -1);
		DrawFocusRect(hDC, &rc);
	}
}

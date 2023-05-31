#include "stdafx.h"
#include "dlgutil.h"

STATIC UINT dlg_IddCovered[10];
STATIC BOOL dlg_IddVisible[10];
STATIC BOOL dlg_IddEnabled[10];
STATIC INTEGER dlg_covered_count = 0;
STATIC SHORT dlg_popcount = 0;
STATIC HWND dlg_hWndpopup = NULL;

VOID DlgCoverIdd (HWND hDlg, UINT idd)
{
    if (dlg_covered_count >= 10) return;
    if (!GetDlgItem (hDlg, idd)) return;
    dlg_IddCovered[dlg_covered_count] = idd;
    dlg_IddVisible[dlg_covered_count] = IsWindowVisible (GetDlgItem (hDlg, idd));
    dlg_IddEnabled[dlg_covered_count] = IsWindowEnabled (GetDlgItem (hDlg, idd));
    dlg_covered_count++;
} /* DlgCoverIdd */

VOID DlgCoverControls (HWND hDlg, BOOL bCover)
{
    INTEGER i;
    for (i=0; i<dlg_covered_count; i++) {
        if (bCover)
            DlgShowControl (hDlg, dlg_IddCovered[i], FALSE);
        else {
            if (dlg_IddEnabled[i])
                EnableWindow (GetDlgItem (hDlg, dlg_IddCovered[i]), TRUE);
            if (dlg_IddVisible[i])
                ShowWindow (GetDlgItem (hDlg, dlg_IddCovered[i]), SW_SHOWNORMAL);
        }
    }
    if (!bCover)
        dlg_covered_count = 0;
} /* DlgCoverControls */

VOID DlgClearCover (HWND hDlg)
{
    dlg_covered_count = 0;
} /* DlgClearCover */

VOID DlgShowControl (HWND hDlg, UINT idd, BOOL bShow)
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
} /* DlgShowControl */

VOID DlgPaintControl (HWND hDlg, UINT idd)
{
    HWND hCtrl;

    hCtrl = GetDlgItem (hDlg, idd);
    if (!hCtrl)
        return;
    InvalidateRect (hCtrl, NULL, TRUE);
    UpdateWindow (hCtrl);
} /* DlgPaintControl */

BOOL DlgAnyPopup()
{
    if (dlg_popcount)
        return (TRUE);
    return (FALSE);
} /* DlgAnyPopup */

BOOL DlgCheckPopup (HWND hDlg, UINT idd_popup, UINT idd_query, WPARAM wParam, BOOL bListBox)
{
    HWND hCtrl;
    BOOL retval;
    STATIC BOOL bInUse = FALSE;

    if (bInUse) return (FALSE);
    if (!dlg_popcount) return (FALSE);
    hCtrl = GetDlgItem (hDlg, idd_popup);
    if (!hCtrl)
        return (FALSE);
    bInUse = TRUE;
    retval = FALSE;
    if ((wParam != idd_popup) AND
        IsWindowEnabled (hCtrl) ) {
        DlgPopup (hDlg, idd_popup, idd_query, bListBox, FALSE);
        UpdateWindow (hDlg);
        retval = TRUE;
    }
    bInUse = FALSE;
    return (retval);
} /* DlgCheckPopup */

VOID DlgPopup (HWND hDlg, UINT idd_popup, UINT idd_query, BOOL bListBox, BOOL bPopup)
{
    HWND hCtrl;

    hCtrl = GetDlgItem (hDlg, idd_popup);
    if (!hCtrl)
        return;
    if (bPopup) {
        DlgCoverControls (hDlg, TRUE);
        EnableWindow (hCtrl, TRUE);
/*
        ShowWindow (hCtrl, SW_SHOWNORMAL);
        UpdateWindow (hCtrl);
        BringWindowToTop (hCtrl);
*/
        SetWindowPos (hCtrl, HWND_TOP, 
           0, 0, 0, 0,
           SWP_NOMOVE | SWP_NOSIZE | SWP_SHOWWINDOW);

        SetFocus (hCtrl);
        ShowWindow (GetDlgItem (hDlg, idd_query), SW_HIDE);
        UpdateWindow (GetDlgItem (hDlg, idd_query));
        dlg_popcount=1;
        dlg_hWndpopup = hCtrl; /* only valid if popcount is 1 */
    }
    else {
        ShowWindow (GetDlgItem (hDlg, idd_query), SW_SHOWNORMAL);
        UpdateWindow (GetDlgItem (hDlg, idd_query));
        SetFocus (GetDlgItem (hDlg, idd_query));
        DlgCoverControls (hDlg, FALSE);
        ShowWindow (hCtrl, SW_HIDE);
        EnableWindow (hCtrl, FALSE);
        InvalidateRect (hCtrl, NULL, TRUE);
        UpdateWindow (hCtrl);
        if (bListBox)
            SendMessage (hCtrl, LB_RESETCONTENT, 0, 0L);
        dlg_popcount=0;
    }
} /* DlgPopup */

LONG DlgScroll (HWND hDlg, WPARAM wParam, LONG lParam, LONG val, LONG minval, LONG maxval,
    LONG line, LONG page)
{
    LONG wIDD = GetWindowWord ((HWND) lParam, GWL_ID);
    switch (LOWORD(wParam)) {
        case SB_PAGEDOWN:
            val+=page;
            break;
        case SB_LINEDOWN:
            val+=line;
            break;
        case SB_PAGEUP:
            val-=page;
            break;
        case SB_LINEUP:
            val-=line;
            break;
         case SB_TOP:
            val = minval;
            break;
        case SB_BOTTOM:
            val = maxval;
            break;
        case SB_THUMBTRACK:
            val = HIWORD(wParam);
            break;
        case SB_THUMBPOSITION:
            val = HIWORD(wParam);
            break;
    } /* voltage scroll command */
    if (val < minval)
        val = minval;
    if (val > maxval)
        val = maxval;

    if (wParam != SB_THUMBTRACK)
        SetScrollPos  (GetDlgItem (hDlg, wIDD), SB_CTL, val, TRUE);
    return (val);
} /* DlgScroll */

VOID DlgSetItemTextNOW (HWND hDlg, UINT idd, LPSTR text)
{
    SetDlgItemText (hDlg, idd, text);
    UpdateWindow (GetDlgItem (hDlg, idd));
} /* DlgSetItemTextNOW */

BOOL DlgFlipRadio (HWND hDlg, INTEGER idd_radio)
{
    BOOL bRadio;
    bRadio = !IsDlgButtonChecked (hDlg, idd_radio);
    SendDlgItemMessage (hDlg, idd_radio, BM_SETCHECK, bRadio, 0L);
    return (bRadio);
} /* DlgFlipRadio */

INTEGER DlgFindCheckedRadioButton (HWND hDlg, INTEGER idd_start, INTEGER idd_end)
{
    INTEGER idd;

    for (idd=idd_start; idd<=idd_end; idd++) {
        if (!IsWindow (GetDlgItem (hDlg, idd)))
            continue;
        if (IsDlgButtonChecked (hDlg, idd))
            return (idd);
    }
    return (0);
} /* DlgFindCheckedRadioButton */

VOID DlgListBoxDataSet (HWND hWndListBox, INTEGER nListItem, DWORD dwData)
{
    SendMessage (hWndListBox, LB_SETITEMDATA, nListItem, dwData);
} /* DlgListBoxDataSet */

DWORD DlgListBoxDataGet (HWND hWndListBox, INTEGER nListItem)
{
    DWORD dwData;

    dwData = SendMessage (hWndListBox, LB_GETITEMDATA, nListItem, 0L);
    return (dwData);
} /* DlgListBoxDataGet */

INTEGER DlgListBoxDataSel (HWND hWndListBox, DWORD dwData)
{
    INTEGER nItems, nIndex;
    DWORD dwItemData;

    nItems = SendMessage (hWndListBox, LB_GETCOUNT, 0, 0L);
    for (nIndex=0; nIndex<nItems; nIndex++) {
        dwItemData = SendMessage (hWndListBox, LB_GETITEMDATA, nIndex, 0L);
        if (dwItemData EQ dwData)
            break;
    }
    if (nIndex >= nItems)
        nIndex = -1;
    SendMessage (hWndListBox, LB_SETCURSEL, nIndex, 0L);
    return (nIndex);
} /* DlgListBoxDataSel */

INTEGER DlgListBoxDataFind (HWND hWndListBox, DWORD dwData)
{
    INTEGER nItems, nIndex;
    DWORD dwItemData;

    nItems = SendMessage (hWndListBox, LB_GETCOUNT, 0, 0L);
    for (nIndex=0; nIndex<nItems; nIndex++) {
        dwItemData = SendMessage (hWndListBox, LB_GETITEMDATA, nIndex, 0L);
        if (dwItemData EQ dwData)
            break;
    }
    if (nIndex >= nItems)
        nIndex = -1;
    return (nIndex);
} /* DlgListBoxDataFind */

#ifndef DLGUTIL_DEFINED
#define DLGUTIL_DEFINED YES

VOID DlgShowControl (HWND hDlg, UINT idd, BOOL bShow);
VOID DlgPaintControl (HWND hDlg, UINT idd);
BOOL DlgAnyPopup(void);
BOOL DlgCheckPopup (HWND hDlg, UINT idd_popup, UINT idd_query, WPARAM wParam, BOOL bListBox);
VOID DlgPopup (HWND hDlg, UINT idd_popup, UINT idd_query, BOOL bListBox, BOOL bPopup);
VOID DlgClearCover (HWND hDlg);
VOID DlgCoverIdd (HWND hDlg, WORD idd);
VOID DlgCoverControls (HWND hDlg, BOOL bCover);
LONG DlgScroll (HWND hDlg, WPARAM wParam, LONG lParam, LONG val, LONG minval, LONG maxval, LONG line, LONG page);
VOID DlgSetItemTextNOW (HWND hDlg, UINT idd, LPSTR text);
BOOL DlgFlipRadio (HWND hDlg, INTEGER idd_radio);
INTEGER DlgFindCheckedRadioButton (HWND hDlg, INTEGER idd_start, INTEGER idd_end);
VOID DlgListBoxDataSet (HWND hWndListBox, INTEGER nListItem, DWORD dwData);
DWORD DlgListBoxDataGet (HWND hWndListBox, INTEGER nListItem);
INTEGER DlgListBoxDataSel (HWND hWndListBox, DWORD dwData);
INTEGER DlgListBoxDataFind (HWND hWndListBox, DWORD dwData);

#undef DLGUTIL_EXPORT
#endif /* DLGUTIL_DEFINED */

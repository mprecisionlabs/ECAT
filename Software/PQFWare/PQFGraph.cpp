#include "stdafx.h"
#include "common.h"
#include "..\core\dlgutil.h"
#include "..\core\kt_util2.h"

STATIC HWND hWndScroll;

#define MAX_SCANS				3
#define SCAN_FULL				0
#define SCAN_FIXED				1
#define SCAN_ZOOM				2

#define SCROLL_DEGREES			0
#define SCROLL_STEPS			1

// Our pointer to the extra bytes reserved through RegisterClass()
#define GWL_GRAPH				0

typedef STRUCTURE tagSCAN
{
    BOOL	scan_bOnStack;
    LONG	scan_nPoints;
	POINT*	scan_hPoints;
    LONG	scan_nViewLeftStep;
    LONG	scan_nViewRightStep;
    DWORD	scan_dwViewLeftDegrees; /* # of degrees at left of display */
    DWORD	scan_dwViewRightDegrees; /* # of degrees at right of display */
    DWORD	scan_dwViewTotDegrees; /* right - left + 1 */
    DWORD	scan_dwNumerator;
    DWORD	scan_dwDenominator;
    LONG	scan_nCalcLeftStep; /* leftmost step visible */
    LONG	scan_nCalcRightStep;  /* rightmost step visible */
    DWORD	scan_dwCalcLeftDegrees; /* leftmost degrees calculated */
    DWORD	scan_dwCalcRightDegrees; /* rightmost degrees calculated */
    DWORD	scan_dwCalcTotDegrees; /* right - left + 1 */
    LONG	scan_xViewStart;
    LONG	scan_xViewEnd;
    LONG	scan_yViewStart;
    LONG	scan_yViewEnd;
    LONG	scan_yViewMult;
    LONG	scan_nScrollRange; /* -1 if scroll not needed */
    LONG	scan_nScrollPage;
    LONG	scan_nScrollLine;
    LONG	scan_nThickPen;
    LONG	scan_nThinPen;

	// Initialize it to zeros
	tagSCAN::tagSCAN() {memset(this, 0, sizeof(tagSCAN));}
} SCAN, FAR * LPSCAN;

typedef STRUCTURE tagGRAPH
{
    HWND	graph_hWndParent;
    HWND	graph_hWnd; /* hWnd of child */
    BOOL	graph_bTestInProgress;

    BOOL	graph_bAux;

    LONG	graph_nSteps;
	DWORD*	graph_hDegrees;
    DWORD	graph_dwTotDegrees; /* total # of degrees in list */

    LONG	graph_nScanner; /* which scan is being used */
    LONG	graph_nMaxScanPoints;
    SCAN	graph_scan[3];

    LONG	graph_nPickStep; /* highlighted step */
    LONG	graph_nScrollOption;
    BOOL	graph_bEnableLeftMouseZooming;
    BOOL	graph_bZooming; /* has rbutton down or has selected zooming in menu */

	// Initialize it to zeros
	tagGRAPH::tagGRAPH() {memset(this, 0, sizeof(tagGRAPH));}
} GRAPH, FAR * LPGRAPH;

#define XVIEW_MIN(_rect) ((_rect.left))
#define XVIEW_MAX(_rect) ((_rect.right)-(_rect.left))

#define MAX_FIXEDVIEW_DEGREES 30000000


VOID prepDC (LPGRAPH lpGraph, LPSCAN lpScan, HDC hDC);
DWORD ConvertXtoStepRange (LPGRAPH lpGraph, LONG x);
VOID UpdateScan (HWND hWnd, LPGRAPH lpGraph);
VOID CreateScan (HWND hWnd, LPGRAPH lpGraph, DWORD dwLeftDegrees, DWORD dwRightDegrees);

VOID PqfGraphPickStep (HWND hWnd, LONG stepnum)
{
    LONG oldstepnum;
    LONG pointnum;
    LPGRAPH lpGraph;
    LPSCAN lpScan;
    LPPOINT lpPoints;

	lpGraph = (LPGRAPH) GetWindowLong(hWnd, GWL_GRAPH);
	if (!lpGraph) return;
    if ((stepnum < 0) OR (stepnum >= lpGraph->graph_nSteps)) {
        lpGraph->graph_nPickStep = -1;
        InvalidateRect (lpGraph->graph_hWnd, NULL, TRUE);
        return;
    }
    oldstepnum = stepnum;
    lpGraph->graph_nPickStep = stepnum;

    if (lpGraph->graph_nScanner EQ SCAN_FULL) {
        InvalidateRect (lpGraph->graph_hWnd, NULL, TRUE);
        return; /* don't need to scroll selected step into view */
    }

    lpScan = (LPSCAN) &(lpGraph->graph_scan[lpGraph->graph_nScanner]);
    if ((stepnum < lpScan->scan_nCalcLeftStep) OR (stepnum > lpScan->scan_nCalcRightStep)) {
/* aaaa redo calculations to include the nPickStep ? */
/* for now, just don't do anything                   */
        if ((oldstepnum >= lpScan->scan_nCalcLeftStep) AND (oldstepnum <= lpScan->scan_nCalcRightStep))
        InvalidateRect (lpGraph->graph_hWnd, NULL, TRUE);
        return; /* don't need to scroll selected step into view */
    }

    lpPoints = lpScan->scan_hPoints;
    pointnum = (stepnum-lpScan->scan_nCalcLeftStep) * 2 + 1; /* point of new level */
    if (lpPoints[pointnum+1].x < lpScan->scan_xViewStart) {
        lpScan->scan_xViewEnd -= (lpScan->scan_xViewStart-lpPoints[pointnum].x);
        lpScan->scan_xViewStart = lpPoints[pointnum].x;
        if (lpScan->scan_nScrollRange)
            SetScrollPos (GetDlgItem (hWnd, IDD_PQFGRAPH_SCROLL), SB_CTL, lpScan->scan_xViewStart, TRUE);
    }
    else if (lpPoints[pointnum].x > lpScan->scan_xViewEnd) {
        lpScan->scan_xViewStart += (lpPoints[pointnum+1].x-lpScan->scan_xViewEnd);
        lpScan->scan_xViewEnd = lpPoints[pointnum+1].x;
        if (lpScan->scan_nScrollRange)
            SetScrollPos (GetDlgItem (hWnd, IDD_PQFGRAPH_SCROLL), SB_CTL, lpScan->scan_xViewStart, TRUE);
    }
    InvalidateRect (lpGraph->graph_hWnd, NULL, TRUE);
} /* PqfGraphPickStep */

VOID PqfGraphTestInProgress (HWND hWnd, BOOL bInProgress)
{
    LPGRAPH lpGraph;
    LONG stepnum;

	lpGraph = (LPGRAPH) GetWindowLong(hWnd, GWL_GRAPH);
	if (!lpGraph) return;
    lpGraph->graph_bTestInProgress = bInProgress;
    stepnum = lpGraph->graph_nPickStep;
    EnableWindow (lpGraph->graph_hWnd, !bInProgress);

    PqfGraphPickStep (hWnd, stepnum);
} /* PqfGraphTestInProgress */

VOID PqfGraphEnableLeftMouseZooming (HWND hWnd, BOOL bEnable)
{
    LPGRAPH lpGraph;

	lpGraph = (LPGRAPH) GetWindowLong(hWnd, GWL_GRAPH);
	if (!lpGraph) return;
    lpGraph->graph_bEnableLeftMouseZooming = bEnable;
} /* PqfGraphEnableLeftMouseZooming */


LONG PqfGraphWndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    PAINTSTRUCT ps;
    LONG x, y, cy, nScanner;
    LONG wIDD;
    RECT rect;
    LPGRAPH lpGraph;
    LPSCAN lpScan;
    HBITMAP hBitmap;
    HDC hDC;

	// Get global instance handle
	HINSTANCE hInst = GetInst();

    switch (msg) {
        case WM_CREATE:
			// Take care of the allocating graph tructure
			lpGraph = new GRAPH;
			if (lpGraph == NULL) return -1;
            SetWindowLong(hWnd, GWL_GRAPH, (LONG) lpGraph);
            lpGraph->graph_hWndParent = GetParent (hWnd);
            lpGraph->graph_nPickStep = -1;
            lpGraph->graph_scan[SCAN_FULL].scan_yViewMult = 1;
            lpGraph->graph_scan[SCAN_FULL].scan_bOnStack = TRUE;
            y = 2;
            cy = 142;
            lpGraph->graph_hWnd = CreateWindow (
					"Graph", 
					"Graph",
                    WS_CHILD | WS_VISIBLE,
                    56, y, 486, cy,
                    hWnd, 
					(HMENU) IDD_PQFGRAPH_DISPLAY, 
					hInst, 
					(LPVOID) lpGraph			// Pass GRAPH pointer to the Graph window in CREATESTRUCT
					);
            GetClientRect (lpGraph->graph_hWnd, &rect);
            lpGraph->graph_scan[SCAN_FULL].scan_xViewStart = XVIEW_MIN(rect);
            lpGraph->graph_scan[SCAN_FULL].scan_xViewEnd = XVIEW_MAX(rect);
            lpGraph->graph_scan[SCAN_FULL].scan_yViewStart = -4;
            lpGraph->graph_scan[SCAN_FULL].scan_yViewEnd = 105;
            GetClientRect (hWnd, &rect);
            hWndScroll = CreateWindow (
					"scrollbar", 
					"",
                    SBS_HORZ | WS_CHILD | WS_VISIBLE,
                    0, y+cy+2, rect.right, 14,
                    hWnd, 
					(HMENU) IDD_PQFGRAPH_SCROLL, 
					hInst, 
					NULL);
			EnableWindow(hWndScroll, FALSE);
            break; /* WM_CREATE */

        case WM_PAINT:
            hDC = BeginPaint (hWnd, (LPPAINTSTRUCT)&ps);

			lpGraph = (LPGRAPH) GetWindowLong(hWnd, GWL_GRAPH);
            hBitmap = LoadBitmap (hInst, MAKEINTRESOURCE((lpGraph->graph_bAux) ? IDB_FRM_AUX : IDB_FRM_TOTAL));

            UtilDrawBitmap (hDC, 0, 0, hBitmap);
            DeleteObject (hBitmap);
            EndPaint (hWnd, (LPPAINTSTRUCT)&ps);
            
            return (FALSE);
            break; /* WM_PAINT */

        case WM_RBUTTONDBLCLK:
/*
for debugging purposes:
			lpGraph = (LPGRAPH) GetWindowLong(hWnd, GWL_GRAPH);
            UpdateScan (hWnd, lpGraph);
            InvalidateRect (lpGraph->graph_hWnd, (LPRECT)NULL, TRUE);
            break;
*/
        case WM_LBUTTONDBLCLK:
			lpGraph = (LPGRAPH) GetWindowLong(hWnd, GWL_GRAPH);
            if (lpGraph->graph_nScanner > SCAN_FIXED)
                lpGraph->graph_scan[lpGraph->graph_nScanner].scan_bOnStack = FALSE;
            while (lpGraph->graph_nScanner > SCAN_FULL) {
                lpGraph->graph_nScanner--;
                if (lpGraph->graph_scan[lpGraph->graph_nScanner].scan_bOnStack)
                    break;
            }
            lpGraph->graph_scan[lpGraph->graph_nScanner].scan_bOnStack = TRUE;
            UpdateScan (hWnd, lpGraph);
            InvalidateRect (lpGraph->graph_hWnd, (LPRECT)NULL, TRUE);
            break; /* WM_*BUTTONDBLCLK */

        case WM_HSCROLL:
			wIDD = GetWindowWord((HWND) lParam, GWL_ID);
            if (wIDD != IDD_PQFGRAPH_SCROLL) return (FALSE);

            common_bUpdating = TRUE;
   			lpGraph = (LPGRAPH) GetWindowLong(hWnd, GWL_GRAPH);
			lpScan = (LPSCAN) &(lpGraph->graph_scan[lpGraph->graph_nScanner]);
            x = lpScan->scan_xViewStart;
            lpScan->scan_xViewStart = DlgScroll (hWnd, wParam, lParam, x, 0, lpScan->scan_nScrollRange, lpScan->scan_nScrollLine, lpScan->scan_nScrollPage);
            if (lpScan->scan_xViewStart != x) {
                lpScan->scan_xViewEnd = lpScan->scan_xViewStart + (LONG) lpScan->scan_dwNumerator;
                InvalidateRect (lpGraph->graph_hWnd, (LPRECT)NULL, TRUE);
            }
			/* aaaaaaaa not dealing with scrolling out of calculated range */
            common_bUpdating = FALSE;
            break; /* WM_HSCROLL */

        case WM_DESTROY:
			// Clean up the graph structure
	   		lpGraph = (LPGRAPH) GetWindowLong(hWnd, GWL_GRAPH);
            for (nScanner=0; nScanner<MAX_SCANS; nScanner++) {
                if (lpGraph->graph_scan[nScanner].scan_hPoints) {
                    delete [] lpGraph->graph_scan[nScanner].scan_hPoints;
					lpGraph->graph_scan[nScanner].scan_hPoints = NULL;
			        lpGraph->graph_scan[nScanner].scan_nPoints = 0;
                }
            }

            if (lpGraph->graph_hDegrees)
			{
				delete [] lpGraph->graph_hDegrees;
				lpGraph->graph_hDegrees = NULL;
			}

			delete lpGraph;
            SetWindowLong(hWnd, GWL_GRAPH, NULL);
            break; /* WM_DESTROY */
		
        default:
            return (DefWindowProc(hWnd, msg, wParam, lParam));
    } /* switch (msg) */
    return (NULL);
} /* PqfGraphWndProc */

LONG GraphWndProc (HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    PAINTSTRUCT ps;
    RECT rect;
    POINT point;
    HDC hDC;
    LPGRAPH lpGraph;
    LPSCAN lpScan;
    LPPOINT lpPoints;
    INTEGER pointnum, y, ychg, line;
    DOUBLE dwLeftDegrees, dwRightDegrees;
    HPEN hOldPen;
    HRGN hRgn1, hRgn2, hRgn3;
	static int firstX;
	static RECT oldRect;

    switch (msg) {
        case WM_CREATE:
			lpGraph = ((LPGRAPH)((LPCREATESTRUCT) lParam)->lpCreateParams);
            SetWindowLong(hWnd, GWL_GRAPH, (LONG) lpGraph);
            break; /* WM_CREATE */

        case WM_PAINT:
            hDC = BeginPaint (hWnd, (LPPAINTSTRUCT)&ps);
   			lpGraph = (LPGRAPH) GetWindowLong(hWnd, GWL_GRAPH);
            GetClientRect (hWnd, &rect);

            lpScan = (LPSCAN) &(lpGraph->graph_scan[lpGraph->graph_nScanner]);
            if (lpScan->scan_hPoints)
                lpPoints = lpScan->scan_hPoints;

            prepDC (lpGraph, lpScan, hDC);
            SetBkMode(hDC, TRANSPARENT);

            /* draw in yellow step-marker */
            if (lpGraph->graph_nSteps AND
                (lpGraph->graph_nPickStep >= lpScan->scan_nCalcLeftStep) AND
                (lpGraph->graph_nPickStep <= lpScan->scan_nCalcRightStep)) {
                RECT rectStep;
                pointnum = (lpGraph->graph_nPickStep-lpScan->scan_nCalcLeftStep) * 2 + 1;
                rectStep.top = lpScan->scan_yViewStart - 2;
                rectStep.bottom = lpScan->scan_yViewEnd + 2;
                rectStep.left = lpPoints[pointnum].x;
                rectStep.right = lpPoints[pointnum+1].x+1;
                if (lpGraph->graph_bTestInProgress)
                    FillRect (hDC, &rectStep, obj_hbr_palegreen);
                else
                    FillRect (hDC, &rectStep, obj_hbr_yellow);
            }

            /* draw in grid lines */
            hOldPen = (HPEN) SelectObject(hDC,CreatePen(PS_DOT,lpScan->scan_nThinPen,RGB_liteblue));
            ychg = 10 * lpScan->scan_yViewMult;
            for (line=0; line<=10; line++) {
                y = line * ychg;
                MoveToEx(hDC,lpScan->scan_xViewStart,y,NULL);
                LineTo(hDC,lpScan->scan_xViewEnd+1,y);
            }

            /* now do 100 % line */
            DeleteObject (SelectObject(hDC,CreatePen(PS_SOLID,lpScan->scan_nThickPen,RGB_olivegreen)));
            MoveToEx(hDC,lpScan->scan_xViewStart,7*ychg,NULL); /* 100 % */
            LineTo(hDC,lpScan->scan_xViewEnd+1,7*ychg);

            if (lpScan->scan_hPoints) {
                DeleteObject (SelectObject(hDC,CreatePen(PS_SOLID,lpScan->scan_nThickPen,RGB_litered)));
                Polyline(hDC,lpPoints,lpScan->scan_nPoints);
            }

#ifdef SKIP
            points[0].x = 0;        points[0].y = 50;
            points[1].x = 30;       points[1].y = 50;
            points[2].x = 30;       points[2].y = 80;
            points[3].x = 70;       points[3].y = 80;
            points[4].x = 70;       points[4].y = 10;
            points[5].x = 100;      points[5].y = 10;
            DeleteObject (SelectObject(hDC,CreatePen(PS_SOLID,2,RGB_darkblue)));
            Polyline(hDC,(LPPOINT) &points,5);
#endif

            DeleteObject (SelectObject (hDC, hOldPen));
            RestoreDC(hDC,-1);
            EndPaint (hWnd, (LPPAINTSTRUCT)&ps);
            
            return (FALSE);
            break; /* WM_PAINT */

        case WM_LBUTTONDOWN:
   			lpGraph = (LPGRAPH) GetWindowLong(hWnd, GWL_GRAPH);
            lpScan = (LPSCAN) &(lpGraph->graph_scan[lpGraph->graph_nScanner]);
            if (!lpGraph->graph_bZooming AND !lpGraph->graph_bEnableLeftMouseZooming) 
			{ 
				/* not zooming, so want to select the step under mouse */
                if (!lpScan->scan_nPoints) 
                    break;

                hDC = GetDC (hWnd);
                prepDC (lpGraph, lpScan, hDC);
                DPtoLP(hDC,(LPPOINT)&lParam,1);
                ReleaseDC (hWnd, hDC);
                point.x = LOWORD (lParam);
                lpPoints = lpScan->scan_hPoints;
                for (pointnum=2; pointnum<lpScan->scan_nPoints; pointnum+=2) 
				{
                    if (point.x < lpPoints[pointnum].x)
                        break;
                }

                //nid = GetWindowLong(GetParent(hWnd), GWL_ID);
                if (pointnum >= lpScan->scan_nPoints) /* de-select */
                    lpGraph->graph_nPickStep = -1;
                else
                    lpGraph->graph_nPickStep = (pointnum-2)/2;
				SendMessage(lpGraph->graph_hWndParent, WM_COMMAND, MAKEWPARAM(IDD_PQF_GRAPH, LBN_SELCHANGE), (LPARAM) lpGraph->graph_nPickStep);
                InvalidateRect (hWnd, (LPRECT)NULL, TRUE);
                break;
            }

            if ((lpGraph->graph_bZooming EQ (BOOL) WM_RBUTTONDOWN) OR
                (lpScan->scan_nPoints <= 1)) 
                break;

            lpGraph->graph_bZooming = (BOOL) msg; /* to trace right vs. left */
            GetClientRect (hWnd, &rect);
            firstX = LOWORD(lParam);
            SetRect(&oldRect,firstX,rect.top,firstX,rect.bottom);
            SetCapture (hWnd);
            break; /* WM_LBUTTONDOWN */

        case WM_RBUTTONDOWN:
			lpGraph = (LPGRAPH) GetWindowLong(hWnd, GWL_GRAPH);
			lpScan = (LPSCAN) &(lpGraph->graph_scan[lpGraph->graph_nScanner]);
            if ((lpGraph->graph_bZooming EQ WM_LBUTTONDOWN) AND
                (GetCursor() != obj_hCursor_cross)) { /* haven't pressed lbutton yet */
                if (lpScan->scan_nPoints <= 1) {
                    break;
                }
            }
            else if ((lpGraph->graph_bZooming) OR
                (lpScan->scan_nPoints <= 1)) {
                break;
            }
            SetCursor (obj_hCursor_cross);
            lpGraph->graph_bZooming = (BOOL) msg; /* to trace right vs. left */
            GetClientRect (hWnd, &rect);
            firstX = LOWORD(lParam);
            SetRect(&oldRect,firstX,rect.top,firstX,rect.bottom);
            SetCapture (hWnd);
            break; /* WM_RBUTTONDOWN */

        case WM_MOUSEMOVE:
			lpGraph = (LPGRAPH) GetWindowLong(hWnd, GWL_GRAPH);
            if (lpGraph->graph_bZooming) {
                RECT clientrect;
                GetClientRect (hWnd, &clientrect);
                rect = clientrect;
                point.x = (LONG) LOWORD(lParam);
                point.y = (LONG) HIWORD(lParam);
                point.x = kmax(point.x, clientrect.left);
                point.x = kmin(point.x, clientrect.right-1);
                if (!PtInRect (&clientrect, point)) {
                    rect.left = rect.right = firstX;
                }
                else {
                    rect.left=kmin(point.x,firstX);
                    rect.left=kmax(rect.left,clientrect.left);
                    rect.right=kmax(point.x,firstX);
                    rect.right=kmin(rect.right,clientrect.right);
                    if (point.x > rect.left)
                        rect.right++;
                }
                IntersectRect (&rect, &rect, &clientrect);
                hRgn1=CreateRectRgnIndirect(&oldRect);
                hRgn2=CreateRectRgnIndirect(&rect);
                hRgn3=CreateRectRgnIndirect(&rect);
                CombineRgn(hRgn3,hRgn1,hRgn2,RGN_XOR);
                hDC=GetDC(hWnd);
                InvertRgn(hDC,hRgn3);
                ReleaseDC(hWnd,hDC);
                DeleteObject(hRgn1);
                DeleteObject(hRgn2);
                DeleteObject(hRgn3);
                oldRect=rect;
            }
            else if (lpGraph->graph_bEnableLeftMouseZooming) {
                if (GetCursor() != obj_hCursor_cross)
                    SetCursor (obj_hCursor_cross);
            }
            else if (GetCursor() != obj_hCursor_arrow)
                SetCursor (obj_hCursor_arrow);
            break; /* WM_MOUSEMOVE */

        case WM_LBUTTONUP:
        case WM_RBUTTONUP:
			lpGraph = (LPGRAPH) GetWindowLong(hWnd, GWL_GRAPH);
            lpScan = (LPSCAN) &(lpGraph->graph_scan[lpGraph->graph_nScanner]);
            /* ALERT ALERT this is assuming WM_LBUTTONDOWN continues to be WM_LBUTTONUP-1 */
            if (lpGraph->graph_bZooming != (BOOL) (msg-1)) {
                break;
            }
            hDC=GetDC(hWnd);
            rect = oldRect;
            if (oldRect.left >= oldRect.right) {
                InvertRect(hDC,&oldRect);
                MessageBeep(NULL);
            }
            else {
                prepDC (lpGraph, lpScan, hDC);
                DPtoLP(hDC,(LPPOINT)&rect,2);

                dwLeftDegrees = ((DOUBLE) rect.left * (DOUBLE) lpScan->scan_dwDenominator) / (DOUBLE) lpScan->scan_dwNumerator;
                dwLeftDegrees += lpScan->scan_dwCalcLeftDegrees;
                dwRightDegrees = ((DOUBLE) rect.right * (DOUBLE) lpScan->scan_dwDenominator) / (DOUBLE) lpScan->scan_dwNumerator;
                dwRightDegrees += lpScan->scan_dwCalcLeftDegrees;
                if ((dwRightDegrees - dwLeftDegrees) < 720L) {
                    ReleaseDC (hWnd, hDC);
                    hDC=GetDC(hWnd);
                    InvertRect(hDC,&oldRect);
                    MessageBeep(NULL);
                }
                else {
                    for (lpGraph->graph_nScanner=SCAN_FIXED+1; lpGraph->graph_nScanner<MAX_SCANS; lpGraph->graph_nScanner++)
                        if (!lpGraph->graph_scan[lpGraph->graph_nScanner].scan_bOnStack)
                            break;
                    if (lpGraph->graph_nScanner >= MAX_SCANS)
                        lpGraph->graph_nScanner = MAX_SCANS - 1;
                    CreateScan (GetParent (hWnd), lpGraph, (LONG) dwLeftDegrees, (LONG) dwRightDegrees);
                    UpdateScan (GetParent (hWnd), lpGraph);
                    InvalidateRect (hWnd, (LPRECT)NULL, TRUE);
                    if (lpGraph->graph_bEnableLeftMouseZooming)
                        PostMessage (hWnd_pqf, WM_COMMAND, IDM_PQF_GRAPH_DISABLEZOOMIN, 0L);
                }
            }
            lpGraph->graph_bZooming = FALSE;
            SetCursor (obj_hCursor_arrow);
            ReleaseDC (hWnd, hDC);
            ReleaseCapture();
            break; /* WM_*BUTTONUP */

        default:
            return (DefWindowProc(hWnd, msg, wParam, lParam));
    } /* switch (msg) */
    return (NULL);
} /* GraphWndProc */

#ifdef VIEWING
DWORD ConvertXtoStepRange (LPGRAPH lpGraph, LONG x)
{
    LPPOINT lpPoints;
    LONG pointnum;
    DWORD retval;

    if (!lpScan->scan_nPoints)
        return (0L);
    lpPoints = lpScan->scan_hPoints;
    for (pointnum=2; pointnum<lpScan->scan_nPoints; pointnum+=2) {
        if (x < lpPoints[pointnum].x)
            break;
    }
    if (pointnum >= lpScan->scan_nPoints)
        retval = 0L;
    else
        retval = MAKELONG (lpPoints[pointnum-1].x, lpPoints[pointnum].x);
    return (retval);
} /* ConvertXtoStepRange */
#endif

STATIC VOID prepDC (LPGRAPH lpGraph, LPSCAN lpScan, HDC hDC)
{
	RECT rect;
    LONG yViewStart, yViewEnd;

    GetClientRect (lpGraph->graph_hWnd, &rect);
    SetMapMode(hDC,MM_ANISOTROPIC);
    yViewStart = -4; yViewEnd = 105;
    SetWindowOrgEx(hDC,lpScan->scan_xViewStart,yViewEnd,NULL);
    SetWindowExtEx(hDC,lpScan->scan_xViewEnd-lpScan->scan_xViewStart+1,yViewStart-yViewEnd-1,NULL);
    SetViewportExtEx(hDC,rect.right,rect.bottom,NULL);
    SetViewportOrgEx(hDC,0,0,NULL);
} /* prepDC */

STATIC VOID CreateScan (HWND hWnd, LPGRAPH lpGraph, DWORD dwLeftDegrees, DWORD dwRightDegrees)
{
    LPSCAN lpScan;

    lpScan = (LPSCAN) &(lpGraph->graph_scan[lpGraph->graph_nScanner]);
    lpScan->scan_bOnStack = TRUE;

    /*
        need to generate:
            dwViewLeftDegrees, dwViewRightDegrees and dwViewTotDegrees
    */
    lpScan->scan_dwViewLeftDegrees = dwLeftDegrees;
    lpScan->scan_dwViewRightDegrees = dwRightDegrees;
    lpScan->scan_dwViewTotDegrees = dwRightDegrees - dwLeftDegrees + 1L;
} /* CreateScan */

STATIC VOID UpdateScan (HWND hWnd, LPGRAPH lpGraph)
{
    LONG   pointnum, stepnum, stealpoint;
    RECT    clientrect;
    LPSCAN  lpScan, lpFullScan;
    LPPOINT lpScanPoints, lpFullPoints;
    LPDWORD lpDegrees;

    if (lpGraph->graph_nScanner EQ SCAN_FULL) {
		EnableWindow(GetDlgItem(hWnd, IDD_PQFGRAPH_SCROLL), FALSE);
        return;
    }

    lpScan = (LPSCAN) &(lpGraph->graph_scan[lpGraph->graph_nScanner]);
    
	if (lpScan->scan_hPoints) 
		delete[] lpScan->scan_hPoints;
    lpScan->scan_hPoints = new POINT[lpGraph->graph_nMaxScanPoints];
    lpScanPoints = lpScan->scan_hPoints;

    lpFullScan = (LPSCAN) &(lpGraph->graph_scan[SCAN_FULL]);
    lpFullPoints = lpFullScan->scan_hPoints;
    lpDegrees = lpGraph->graph_hDegrees;

    /* the only known facts at this point are: */
    /*        dwViewLeftDegrees, dwViewRightDegrees and dwViewTotDegrees */
    /*        and all SCAN_FULL data                                     */

    /* so first figure out rightmost VISIBLE (viewed) step */
    /* loop towards RIGHT until rightmost degrees of step is */
    /* out of viewer (or just at right edge of view).   */
    for (stepnum=0; stepnum<lpGraph->graph_nSteps; stepnum++)
        if (lpDegrees[stepnum] >= lpScan->scan_dwViewRightDegrees)
            break;
    if (stepnum >= lpGraph->graph_nSteps)
        stepnum--;
    lpScan->scan_nViewRightStep = stepnum;

    /* now figure out leftmost VISIBLE (viewed) step */
    /* loop towards LEFT until rightmost degrees of step is */
    /* out of viewer...and then use the FOLLOWING step   */
    for (; stepnum>=0; stepnum--)
        if (lpDegrees[stepnum] < lpScan->scan_dwViewLeftDegrees)
            break;
    stepnum++;
    lpScan->scan_nViewLeftStep = stepnum;

    GetClientRect (lpGraph->graph_hWnd, &clientrect);
    lpScan->scan_dwNumerator = (DWORD) (XVIEW_MAX(clientrect) - XVIEW_MIN(clientrect));
    lpScan->scan_dwDenominator = lpScan->scan_dwViewTotDegrees - 1L;

    /* now determine how far in each direction we can calculate */
    /* for scrolling purposes                                   */
    if (lpScan->scan_dwViewTotDegrees >= lpFullScan->scan_dwViewTotDegrees) {
        lpScan->scan_dwCalcTotDegrees = lpFullScan->scan_dwViewTotDegrees;
        lpScan->scan_dwCalcLeftDegrees = 0L;
        lpScan->scan_dwCalcRightDegrees = lpScan->scan_dwCalcTotDegrees-1L;
        lpScan->scan_nCalcLeftStep = 0;
        lpScan->scan_nCalcRightStep = lpGraph->graph_nSteps - 1;
    } /* fixed view, total present curve is visible */
    else {
        lpScan->scan_dwCalcTotDegrees = (DWORD) ((32000.0 * (DOUBLE) lpScan->scan_dwDenominator) / (DOUBLE) lpScan->scan_dwNumerator);
        lpScan->scan_dwCalcTotDegrees = kmin (lpScan->scan_dwCalcTotDegrees, lpFullScan->scan_dwViewTotDegrees);
        if (lpScan->scan_dwViewLeftDegrees < ((lpScan->scan_dwCalcTotDegrees-lpScan->scan_dwViewTotDegrees)/2L)) {
            /* not enough space on left side */
            lpScan->scan_dwCalcLeftDegrees = 0L;
            lpScan->scan_dwCalcRightDegrees = lpScan->scan_dwCalcTotDegrees-1L;
            for (stepnum=0; stepnum<lpGraph->graph_nSteps; stepnum++)
                if (lpDegrees[stepnum] >= lpScan->scan_dwCalcRightDegrees)
                    break;
            if (stepnum >= lpGraph->graph_nSteps)
                stepnum--;
            lpScan->scan_nCalcLeftStep = 0;
            lpScan->scan_nCalcRightStep = stepnum;
        }
        else if ((lpGraph->graph_dwTotDegrees - lpScan->scan_dwViewTotDegrees) < ((lpScan->scan_dwCalcTotDegrees-lpScan->scan_dwViewTotDegrees)/2L)) {
            /* not enough space on right side */
            lpScan->scan_dwCalcLeftDegrees = lpGraph->graph_dwTotDegrees - lpScan->scan_dwCalcTotDegrees;
            lpScan->scan_dwCalcRightDegrees = lpGraph->graph_dwTotDegrees - 1L;
            for (stepnum=lpGraph->graph_nSteps-1; stepnum>=0; stepnum--)
                if (lpDegrees[stepnum] < lpScan->scan_dwCalcLeftDegrees)
                    break;
            stepnum++;
            lpScan->scan_nCalcLeftStep = stepnum;
            lpScan->scan_nCalcRightStep = lpGraph->graph_nSteps-1;
        }
        else {
            lpScan->scan_dwCalcLeftDegrees = lpScan->scan_dwViewLeftDegrees - ((lpScan->scan_dwCalcTotDegrees-lpScan->scan_dwViewTotDegrees)/2L);
            lpScan->scan_dwCalcRightDegrees = lpScan->scan_dwCalcLeftDegrees + lpScan->scan_dwCalcTotDegrees - 1L;
            for (stepnum=lpGraph->graph_nSteps-1; stepnum>=0; stepnum--)
                if (lpDegrees[stepnum] < lpScan->scan_dwCalcLeftDegrees)
                    break;
            stepnum++;
            lpScan->scan_nCalcLeftStep = stepnum;
            for (stepnum=0; stepnum<lpGraph->graph_nSteps; stepnum++)
                if (lpDegrees[stepnum] >= lpScan->scan_dwCalcRightDegrees)
                    break;
            if (stepnum >= lpGraph->graph_nSteps)
                stepnum--;
            lpScan->scan_nCalcRightStep = stepnum;
        }
    } /* view is not larger than whole curve */

    /*
        we now have more of the scan structure filled out; the total
        info is now:
              dwViewLeftDegrees, dwViewRightDegrees and dwViewTotDegrees
              nViewLeftStep, nViewRightStep
              dwCalcTotDegrees, dwCalcLeftDegrees, dwCalcRightDegrees
              nCalcLeftStep, nCalcRightStep
              dwNumerator, dwDenominator,
        still need to determine:
              nPoints, hPoints,
              xViewStart, xViewEnd, yViewStart, yViewEnd, yViewMult,
              nScrollRange, nScrollPage, nScrollLine,
              nThickPen, nThinPen
    */

    lpScan->scan_xViewStart = (LONG) (
             ((DOUBLE) (lpScan->scan_dwViewLeftDegrees - lpScan->scan_dwCalcLeftDegrees) *
                (DOUBLE) lpScan->scan_dwNumerator ) / (DOUBLE) lpScan->scan_dwDenominator);
    lpScan->scan_xViewEnd = lpScan->scan_xViewStart + (LONG) lpScan->scan_dwNumerator;
    lpScan->scan_nPoints = 1 + (2 * (lpScan->scan_nCalcRightStep - lpScan->scan_nCalcLeftStep + 1));

    lpScan->scan_yViewMult = 1;
    lpScan->scan_yViewStart = -4;
    lpScan->scan_yViewEnd = 105;
    lpScan->scan_nThickPen = 2;
    lpScan->scan_nThinPen = 1;

#ifdef SKIP
    nRange = (LONG) (((DOUBLE) lpScan->scan_dwViewTotDegrees * (DOUBLE) lpScan->scan_dwNumerator) / (DOUBLE) lpScan->scan_dwDenominator);
    if (nRange < 5000) {
        lpScan->scan_yViewMult = 1;
        lpScan->scan_yViewStart = -4;
        lpScan->scan_yViewEnd = 105;
        lpScan->scan_nThickPen = 2;
        lpScan->scan_nThinPen = 1;
    }
    else if (nRange < 12000) {
        lpScan->scan_yViewMult = 10;
        lpScan->scan_yViewStart = -40;
        lpScan->scan_yViewEnd = 1050;
        lpScan->scan_nThickPen = 20;
        lpScan->scan_nThinPen = 10;
    }
    else {
        lpScan->scan_yViewMult = 100;
        lpScan->scan_yViewStart = -400;
        lpScan->scan_yViewEnd = 10500;
        lpScan->scan_nThickPen = 200;
        lpScan->scan_nThinPen = 100;
    }
#endif

    lpScanPoints[0].x = 0;
    stealpoint = lpScan->scan_nCalcLeftStep*2+1;
    for (stepnum=lpScan->scan_nCalcLeftStep, pointnum=1; stepnum<=lpScan->scan_nCalcRightStep; stepnum++) {
        lpScanPoints[pointnum].y = lpFullPoints[stealpoint].y;
        lpScanPoints[pointnum].x = lpScanPoints[pointnum-1].x;
        pointnum++; stealpoint++;
        lpScanPoints[pointnum].y = lpScanPoints[pointnum-1].y;
        lpScanPoints[pointnum].x = (INTEGER) ((((DOUBLE) lpDegrees[stepnum]-lpScan->scan_dwCalcLeftDegrees) * lpScan->scan_dwNumerator) / (DOUBLE) lpScan->scan_dwDenominator);
        pointnum++; stealpoint++;
    }
    /* redo last point in case lpDegrees was to the right of CalcRightDegrees,   */
    /* which will happen when the cutoff is in the MIDDLE of the rightmost step, */
    /* which in fact is going to be the usual case.                              */
    lpScanPoints[pointnum-1].x = (INTEGER) ((((DOUBLE) lpScan->scan_dwCalcRightDegrees-lpScan->scan_dwCalcLeftDegrees) * lpScan->scan_dwNumerator) / (DOUBLE) lpScan->scan_dwDenominator);
    lpScanPoints[0].y = lpScanPoints[1].y;

    if ((lpScan->scan_dwCalcLeftDegrees  >= lpScan->scan_dwViewLeftDegrees) AND
        (lpScan->scan_dwCalcRightDegrees <= lpScan->scan_dwViewRightDegrees)) {
		EnableWindow(GetDlgItem(hWnd, IDD_PQFGRAPH_SCROLL), FALSE);
        lpScan->scan_nScrollRange = -1;
    }
    else {
        if (lpGraph->graph_nScrollOption EQ SCROLL_DEGREES) {
            lpScan->scan_nScrollPage = (lpScan->scan_xViewEnd - lpScan->scan_xViewStart) / 2;
            lpScan->scan_nScrollLine = (lpScan->scan_xViewEnd - lpScan->scan_xViewStart) / 10;
            lpScan->scan_nScrollRange = (LONG) (((DOUBLE) (lpScan->scan_dwCalcRightDegrees - lpScan->scan_dwCalcLeftDegrees - lpScan->scan_dwViewTotDegrees) * lpScan->scan_dwNumerator) / ((DOUBLE) lpScan->scan_dwDenominator) );
            lpScan->scan_nScrollRange -= (lpScan->scan_xViewEnd - lpScan->scan_xViewStart);
            SetScrollRange (GetDlgItem (hWnd, IDD_PQFGRAPH_SCROLL), SB_CTL, 0, lpScan->scan_nScrollRange, FALSE);
            SetScrollPos (GetDlgItem (hWnd, IDD_PQFGRAPH_SCROLL), SB_CTL, lpScan->scan_xViewStart, TRUE);
			EnableWindow(GetDlgItem(hWnd, IDD_PQFGRAPH_SCROLL), TRUE);
        }
        else { /* by step */
/* aaaaaaaaa not supported yet if ever */
			EnableWindow(GetDlgItem(hWnd, IDD_PQFGRAPH_SCROLL), FALSE);
        }
    } /* if doing fixed width view */
} /* UpdateScan */


DWORD PqfSetGraph(HWND hMainDlg, LONG nPickStep)
{
    LONG   pointnum, stepnum, nScanner;
    RECT    clientrect;
    LPGRAPH lpGraph;
    LPSCAN  lpScan;
    LPPOINT lpPoints;
    LPDWORD lpDegrees;
    DWORD   dwTotDegrees;
	DOUBLE d1, d2, d3;
	INTEGER n1;

	HWND hWnd = GetDlgItem(hMainDlg, IDD_PQF_GRAPH);
	lpGraph = (LPGRAPH) GetWindowLong(hWnd, GWL_GRAPH);
    lpGraph->graph_bAux = FALSE;

    for (nScanner = 0; nScanner < MAX_SCANS; nScanner++) 
	{
        if (lpGraph->graph_scan[nScanner].scan_hPoints) 
		{
            delete [] lpGraph->graph_scan[nScanner].scan_hPoints;
            lpGraph->graph_scan[nScanner].scan_hPoints = NULL;
            lpGraph->graph_scan[nScanner].scan_nPoints = 0;
        }
    }

    if (lpGraph->graph_hDegrees) 
	{
		delete [] lpGraph->graph_hDegrees;
        lpGraph->graph_hDegrees =  NULL;
    }

    lpGraph->graph_nSteps = SendDlgItemMessage(hMainDlg, IDD_PQF_LIST, LVM_GETITEMCOUNT, 0, 0);
    lpGraph->graph_nMaxScanPoints = 1 + (2 * lpGraph->graph_nSteps);

    GetClientRect(lpGraph->graph_hWnd, &clientrect);
    if (lpGraph->graph_nSteps EQ 0) 
	{
        lpGraph->graph_nPickStep = -1;
        InvalidateRect (lpGraph->graph_hWnd, NULL, TRUE);
        return (0L);
    }
    lpGraph->graph_nPickStep = nPickStep;

	// Assemble list of steps
	LPPQFSTEP* pPqfStepList = new LPPQFSTEP[lpGraph->graph_nSteps];
	if (pPqfStepList == NULL)
		return 0;
	for (stepnum = 0; stepnum < lpGraph->graph_nSteps; stepnum++)
	{
		LVITEM lvItem;
		memset(&lvItem, 0, sizeof(lvItem));
		lvItem.mask = LVIF_PARAM;
		lvItem.iItem = stepnum;
		SendDlgItemMessage(hMainDlg, IDD_PQF_LIST, LVM_GETITEM, 0, (LPARAM) &lvItem);
		pPqfStepList[stepnum] = (LPPQFSTEP) lvItem.lParam;
	}

    lpGraph->graph_hDegrees = new DWORD[lpGraph->graph_nSteps];
    lpDegrees = lpGraph->graph_hDegrees;
    
	lpGraph->graph_dwTotDegrees = 0L;
	for (stepnum = 0; stepnum < lpGraph->graph_nSteps; stepnum++)
	{
		LPPQFSTEP pPqfStep = pPqfStepList[stepnum];
		if (pPqfStep == NULL)
			continue;

        switch (pPqfStep->step_duration_units) 
		{
            case PQF_DURATION_UNITS_MIN:
                lpDegrees[stepnum] = (DWORD) pPqfStep->step_duration * pqf_dwDegreesPerSecond;
                break;
            case PQF_DURATION_UNITS_SEC:
                lpDegrees[stepnum] = ((DWORD) pPqfStep->step_duration) * (pqf_dwDegreesPerSecond / 100L);
                break;
            case PQF_DURATION_UNITS_CYC:
                lpDegrees[stepnum] = (DWORD) pPqfStep->step_duration * PQF_DEGREES_PER_CYCLE;
                break;
        } // switch on duration units 

		if (stepnum != (lpGraph->graph_nSteps - 1))
			lpDegrees[stepnum] += ((360 + pPqfStepList[stepnum + 1]->step_start - pPqfStep->step_start) % 360);
        lpDegrees[stepnum] = kmax(lpDegrees[stepnum],1L);
        lpGraph->graph_dwTotDegrees += lpDegrees[stepnum];
        lpDegrees[stepnum] = lpGraph->graph_dwTotDegrees;
	}
    lpGraph->graph_dwTotDegrees++;

    /* first calculate normal full-view scan */
    lpScan = (LPSCAN) &(lpGraph->graph_scan[SCAN_FULL]);
    lpScan->scan_nPoints = 1 + (2 * lpGraph->graph_nSteps);
    lpScan->scan_hPoints = new POINT[lpScan->scan_nPoints];

    lpPoints = lpScan->scan_hPoints;
    lpScan->scan_dwNumerator = (DWORD) (XVIEW_MAX(clientrect) - XVIEW_MIN(clientrect));
    lpScan->scan_dwDenominator = lpGraph->graph_dwTotDegrees - 1L;

    lpPoints[0].x = 0;
	//for (stepnum=0, pointnum=1, stepi=lpStepList->step_next; (stepi>0) AND (stepi<=lpStepListhdr->hdr_numsteps); stepi=lpStepList[stepi].step_next, stepnum++) 
	for (stepnum = 0, pointnum = 1; stepnum < lpGraph->graph_nSteps; stepnum++) 
	{
        lpPoints[pointnum].y = 10 * pPqfStepList[stepnum]->step_level;
        lpPoints[pointnum].x = lpPoints[pointnum-1].x;
        pointnum++;
        lpPoints[pointnum].y = lpPoints[pointnum-1].y;
		d1 = (DOUBLE) lpDegrees[stepnum];
		d2 = (DOUBLE) lpScan->scan_dwNumerator;
		d1 = d1 * d2;
		d3 = (DOUBLE) lpScan->scan_dwDenominator;
		d1 = d1 / d3;
		n1 = (INTEGER) d1;
        lpPoints[pointnum].x = (INTEGER) (((DOUBLE) lpDegrees[stepnum] * lpScan->scan_dwNumerator) / (DOUBLE) lpScan->scan_dwDenominator);
        pointnum++;
    }
    lpPoints[0].y = lpPoints[1].y;

    lpScan->scan_nViewLeftStep = 0;
    lpScan->scan_nViewRightStep = lpGraph->graph_nSteps - 1;
    lpScan->scan_dwViewLeftDegrees = 0L;
    lpScan->scan_dwViewRightDegrees = lpGraph->graph_dwTotDegrees - 1L;
    lpScan->scan_dwViewTotDegrees = lpGraph->graph_dwTotDegrees;
    lpScan->scan_nCalcLeftStep = lpScan->scan_nViewLeftStep;
    lpScan->scan_nCalcRightStep = lpScan->scan_nViewRightStep;
    lpScan->scan_dwCalcLeftDegrees = lpScan->scan_dwViewLeftDegrees;
    lpScan->scan_dwCalcRightDegrees = lpScan->scan_dwViewRightDegrees;
    lpScan->scan_dwCalcTotDegrees = lpScan->scan_dwViewTotDegrees;

    lpScan->scan_xViewStart = XVIEW_MIN(clientrect);
    lpScan->scan_xViewEnd = XVIEW_MAX(clientrect);
    lpScan->scan_yViewStart = -4;
    lpScan->scan_yViewEnd = 105;
    lpScan->scan_yViewMult = 1;
    lpScan->scan_nScrollRange = -1;
    lpScan->scan_nScrollPage = 0;
    lpScan->scan_nScrollLine = 0;
    lpScan->scan_nThickPen = 2;
    lpScan->scan_nThinPen = 1;

    UpdateScan (hWnd, lpGraph);

    InvalidateRect (lpGraph->graph_hWnd, NULL, TRUE);
    dwTotDegrees = lpGraph->graph_dwTotDegrees;

	if (pPqfStepList != NULL)
		delete [] pPqfStepList;

    return (dwTotDegrees);
} /* PqfSetGraph */



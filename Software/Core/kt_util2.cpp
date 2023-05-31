
#include "stdafx.h"
#include "kt_util2.h"
#include <direct.h>
#include <errno.h>

STATIC BOOL util_ctlcolor_enable = TRUE;
STATIC CHARACTER *util_warncaption   = "WARNING";
STATIC CHARACTER *util_errcaption    = "ERROR";
STATIC CHARACTER *util_fatalcaption = "PROGRAM ERROR";
STATIC CHARACTER *util_msgbox = "UtilMessageBox";
STATIC CHARACTER util_buffer[501];
STATIC INTEGER util_wm_msgbox = 0;

VOID UtilInit (WORD wm_msgbox)
{
    util_wm_msgbox = wm_msgbox;
} /* UtilInit */

VOID UtilEndDialog (HWND hWnd, INTEGER nResult, LPHANDLE lphButtonList)
{
    EndDialog (hWnd, nResult);
} /* UtilEndDialog */

BOOL UtilDestroyDialog (HWND hWnd, LPHANDLE lphButtonList)
{
    BOOL retval;
    retval = DestroyWindow (hWnd);
    return (retval);
} /* UtilDestroyDialog */

BOOL UtilSetMenuName (HINSTANCE hInst, HWND hWnd, LPSTR lpszMenu)
{
    HMENU hMenu, hOldMenu;
    BOOL bSetMenu;

    if (!hInst OR !hWnd)
        return (FALSE);
    hOldMenu = GetMenu (hWnd);
    if (lpszMenu AND lpszMenu[0])
        hMenu = LoadMenu (hInst, lpszMenu);
    else
        hMenu = NULL;
    bSetMenu = SetMenu (hWnd, hMenu);
    if (hOldMenu)
        DestroyMenu (hOldMenu);
    return (bSetMenu);
} /* UtilSetMenuName */

VOID UtilEnableCtlColor (BOOL bEnable)
{
    util_ctlcolor_enable = bEnable;
} /* UtilEnableCtlColor */

HBRUSH UtilCtlColor (HWND hWnd, HDC hDC, DWORD text_color,
        DWORD textback_color, HBRUSH hBackBrush)
{
    STATIC POINT point;

    SetTextColor (hDC, text_color);
    if (!util_ctlcolor_enable)
        return (hBackBrush);
    SetBkColor (hDC, textback_color);
    UnrealizeObject (hBackBrush);
    point.x = point.y = 0;
    ClientToScreen (hWnd, &point);
    SetBrushOrgEx (hDC, point.x, point.y, NULL);
    return (hBackBrush);
} /* UtilCtlColor */

INTEGER UtilMessageBox (HWND hWnd, LPSTR message, LPSTR caption, WORD wMcode)
{
    INTEGER nid;
    HWND hWndParent;

    for (hWndParent = hWnd; GetWindowLong (hWndParent, GWL_HWNDPARENT); hWndParent = (HWND) GetWindowLong (hWndParent, GWL_HWNDPARENT))
        ;
    if (hWndParent AND util_wm_msgbox)
        SendMessage (hWndParent, util_wm_msgbox, TRUE, (LPARAM) hWnd);
    nid = MessageBox (hWnd, message, caption, wMcode);
    if (hWndParent AND util_wm_msgbox)
        SendMessage (hWndParent, util_wm_msgbox, FALSE, (LPARAM) hWnd);
    return (nid);
} /* UtilMessageBox */

BOOL UtilMessageTab (HANDLE hInst, HWND hWnd, WORD wStrnum, LPSTR caption, WORD wMcode)
{
    INTEGER retval;

    LoadString ((HINSTANCE) hInst, wStrnum, (LPSTR) util_buffer, 500);
    retval = UtilMessageBox (hWnd, util_buffer, caption, wMcode);
    return (retval);
} /* UtilMessageTab */

BOOL UtilMessageTab2 (HANDLE hInst, HWND hWnd, WORD wStrnum1, WORD wStrnum2, LPSTR caption, WORD wMcode)
{
    STATIC CHARACTER buftab[1002];
    INTEGER retval;

    LoadString ((HINSTANCE) hInst, wStrnum1, (LPSTR) buftab, 500);
    LoadString ((HINSTANCE) hInst, wStrnum2, (LPSTR) util_buffer, 500);
    strcat (buftab, util_buffer);
    retval = UtilMessageBox (hWnd, buftab, caption, wMcode);
    return (retval);
} /* UtilMessageTab2 */

LPSTR UtilLoadString (HANDLE hInst, HWND hWnd, WORD wStrnum, LPSTR buftab, INTEGER maxlen)
{
    INTEGER len;

    len = LoadString ((HINSTANCE) hInst, wStrnum, (LPSTR) buftab, maxlen);
    if (len <= 0)
        return (0L);
    return (buftab);
} /* UtilLoadString */

LPSTR UtilLoadStrings (HANDLE hInst, HWND hWnd, WORD wStrnum, INTEGER strcnt, LPSTR buftab, INTEGER maxlen)
{
    INTEGER len, totlen;

    totlen = 0;
    while (strcnt) {
        len = LoadString ((HINSTANCE) hInst, wStrnum, (LPSTR) (buftab+totlen), maxlen-totlen);
        totlen += len;
        wStrnum++;
        strcnt-- ;
    }
    if (totlen EQ 0)
        return (0L);
    return (buftab);
} /* UtilLoadStrings */

INTEGER UtilStripName (LPSTR name)
{
    INTEGER len, cutoff, i, j;
    BOOL bMove;

    len = strlen (name);
    i = j = cutoff = 0;
    bMove = FALSE;
    while (i < len) {
        if (bMove) {
            name[j] = name[i];
            j++;
            if (!isspace(name[i]))
                cutoff = j;
        }
        else if (!isspace(name[i])) {
            bMove = TRUE;
            continue;
        }
        i++ ;
    }
    name[cutoff] = '\0';
    return (cutoff);
} /* UtilStripName */


INTEGER UtilCompressName (LPSTR name)
{
    INTEGER len, i, j;

    len = strlen (name);
    for (i=0, j=0; i < len; i++) {
        if (isspace(name[i]))
            continue;
        name[j] = name[i];
        j++;
    }
    name[j] = '\0';
    return (j);
} /* UtilCompressName */


/* called after sprintf %e, to strip extraneous 0's */
INTEGER UtilStripExp (LPSTR text)
{
    INTEGER i, e, p, len;

    len = strlen (text);
    e = p = (-1);
    for (i=0; i<len; i++) {
        if (text[i] EQ '.') p=i;
        if ((text[i] EQ 'e') OR (text[i] EQ 'E')) {
            e=i;
            break;
        }
    }
    if ((e < 0) OR (p < 0))
        return (-1);

    for (i=e-1; i>(p+1); i--)
        if (text[i] != '0') break;
    p = i;
    p++;
    text[p] = text[e];   /* exponent letter, e or E */
    p++;
    text[p] = text[e+1]; /* exponent sign */
    for (i=e+2; i<len; i++)
        if (text[i] != '0') break;
    for (; i<len; i++) {
        p++;
        text[p] = text[i];
    }
    if ((text[p] EQ '+') OR (text[p] EQ '-')) {
        p++;
        text[p] = '0';
    }
    text[p+1] = '\0';
    return (p+1);
} /* UtilStripExp */

INTEGER UtilGetInt (LPSTR buffer)
{
    INTEGER endnum, n;
    LPSTR s;

    n = 0;
    endnum = FALSE;
    s = buffer;
    while (*s) {
        if (!isspace (*s)) break;
        s++ ;
    }
    if (!*s) return (-1);
    while (*s) {
        if (isdigit (*s)) {
            if (endnum) return (-1);
            n = (n * 10) + *s - '0';
        }
        else if (isspace(*s)) endnum = TRUE;
        else return (-1);
        s++;
    }
    return (n);
} /* UtilGetInt */

INTEGER UtilGetSignedInt (LPSTR buffer, BOOL FAR * lpbOK)
{
    INTEGER n, polarity;

    UtilStripName (buffer);
    polarity = 1;
    if (buffer[0] EQ '-') {
        polarity = (-1);
        buffer++ ;
    }
    else if (buffer[0] EQ '+')
        buffer++ ;

    n = UtilGetInt (buffer);
    if (n < 0) {
        *lpbOK = FALSE;
        return (0);
    }
    if (polarity < 0)
        n = (-n);
    *lpbOK = TRUE;
    return (n);
} /* UtilGetSignedInt */

LONG UtilGetLong (LPSTR buffer)
{
    INTEGER endnum;
    LONG n;
    LPSTR s;

    n = 0L;
    endnum = FALSE;
    s = buffer;
    while (*s) {
        if (!isspace (*s)) break;
        s++ ;
    }
    while (*s) {
        if (isdigit (*s)) {
            if (endnum) return (-1L);
            n = (n * 10L) + (LONG) *s - (LONG) '0';
        }
        else if (isspace(*s)) endnum = TRUE;
        else return (-1L);
        s++;
    }
    return (n);
} /* UtilGetLong */

LONG UtilGetLong100 (LPSTR buffer)
{
    STATIC INTEGER numdig[2];
    INTEGER i;
    LONG value;
    INTEGER sign;
    INTEGER nomore, nogo;
    CHARACTER ch;

    i = numdig[0] = numdig[1] = 0;
    value = 0L;
    nomore = nogo = FALSE;
    while ((ch=buffer[i]) != '\0') {
        if (!isspace(ch)) break;
        i++;
    }
    if (ch EQ '\0') {
        return (0L);
    }

    sign = 1;
    if (ch EQ '-') {
        sign = -1;
        i++;
    }
    else if (ch EQ '+')
        i++;

    while ((ch=buffer[i]) != '\0') {
        if (!isspace(ch)) break;
        i++;
    }
    if (ch EQ '\0') {
        return (0L);
    }

    while ((ch=buffer[i]) != '\0') {
        i++;
        if (isspace(ch)) { nomore = TRUE; continue; }
        if (nomore) { nogo = TRUE; break; }
        if (isdigit(ch)) {
            numdig[0]++;
            value = (value*10L) + (LONG) (ch - '0');
            continue;
        }
        if (ch EQ '.') break;
        nogo = TRUE; break;
    }

    while ((!nogo) AND ((ch=buffer[i]) != '\0')) {
        i++;
        if (isspace(ch)) { nomore = TRUE; continue; }
        if (nomore) { nogo = TRUE; break; }
        if (isdigit(ch)) {
            numdig[1]++;
            if (numdig[1] > 2) {
                nogo = TRUE; break;
            }
            value = (value*10L) + (LONG) (ch - '0');
            continue;
        }
        nogo = TRUE; break;
    }
    if (nogo) {
        return (-1L);
    }
    while (numdig[1] < 2) {
        value = value * 10L;
        numdig[1]++ ;
    }
    value = value * sign;
    return (value);
} /* UtilGetLong100 */

VOID UtilDumpValue100 (LONG value, LPSTR buffer, INTEGER fieldlen, BOOL bStrip, BOOL bFixed, INTEGER qdec)
{
    INTEGER i, v, sign;
    BOOL rnum;

    for (i=0; i<fieldlen; i++)
        buffer[i] = ' ';

    i = fieldlen;
    sign = 1;
    if (value < 0) {
        sign = -1;
        value = kabs(value);
    }

    buffer[i] = '\0';
    i--;
    rnum = FALSE;
    v = (SHORT) (value % 10L);
    if (v OR bFixed) {
        buffer[i] = (CHARACTER) ('0' + v);
        i--;
        value = value - v;
        rnum = TRUE;
        if (qdec < 2) {
            rnum = FALSE;
            i++ ;
        }
    }
    v = (SHORT) (value % 100L);
    if (v OR rnum OR ((qdec<2) AND bFixed)) {
        buffer[i] = (CHARACTER) ('0' + v/10);
        i--;
        value = value - v;
        rnum = TRUE;
    }
    if (rnum) {
        buffer[i] = '.';
        i--;
    }
    value = value / 100L;
    while (i >= 0) {
        v = (SHORT) (value % 10L);
        buffer[i] = (CHARACTER) ('0' + v);
        i--;
        value = value / 10L;
        if (value EQ 0L) break;
    }
    if (sign EQ -1)
        buffer[i] = '-';
    if (bStrip)
        UtilStripName (buffer);
} /* UtilDumpValue100 */

VOID UtilRectScreenToClient (HWND hWnd, LPRECT lprect)
{
    STATIC POINT point;

    point.x = lprect->left;
    point.y = lprect->top;
    ScreenToClient (hWnd, &point);
    lprect->left = point.x;
    lprect->top = point.y;

    point.x = lprect->right;
    point.y = lprect->bottom;
    ScreenToClient (hWnd, &point);
    lprect->right = point.x;
    lprect->bottom = point.y;
} /* UtilRectScreenToClient */


/************************************************
 *    --- VOID FAR PASCAL UtilDrawBitmap ---
 *
 ************************************************/
VOID UtilDrawBitmap (HDC hDC, short xStart, short yStart, HBITMAP hBitmap)
{
    STATIC BITMAP bm;
    HDC hdcMem;
    STATIC POINT ptSize, ptOrg;

    hdcMem = CreateCompatibleDC (hDC);
    SelectObject (hdcMem, hBitmap);
    SetMapMode (hdcMem, GetMapMode (hDC));
    GetObject (hBitmap, sizeof(BITMAP), (LPSTR)&bm);

    ptSize.x = bm.bmWidth;
    ptSize.y = bm.bmHeight;
    DPtoLP (hDC, &ptSize, 1);

    ptOrg.x = 0;
    ptOrg.y = 0;
    DPtoLP (hdcMem, &ptOrg, 1);

    BitBlt (hDC, xStart, yStart, ptSize.x, ptSize.y, hdcMem, ptOrg.x, ptOrg.y, SRCCOPY);
    DeleteDC (hdcMem);
} /* DrawBitMap*/


VOID UtilMakePathList (HWND hWnd, LPSTR pathlist, LPSTR path, LPSTR sub1, LPSTR sub2, LPSTR sub3, LPSTR ext)
{
    STATIC CHARACTER *path3_format = "%s\\%s\\%s\\%s\\*.%s";
    STATIC CHARACTER *path2_format = "%s\\%s\\%s\\*.%s";
    STATIC CHARACTER *path1_format = "%s\\%s\\*.%s";
    STATIC CHARACTER buffer[10];

    if (ext)
        strcpy (buffer, ext);
    else {
        buffer[0] = '*';
        buffer[1] = '\0';
    }
    if (sub1 AND sub2 AND sub3) {
        wsprintf (pathlist, (LPSTR) path3_format, path, sub1, sub2, sub3, (LPSTR) buffer);
    }
    else if (sub1 AND sub2) {
        wsprintf (pathlist, (LPSTR) path2_format, path, sub1, sub2, (LPSTR) buffer);
    }
    else if (sub1) {
        wsprintf (pathlist, (LPSTR) path1_format, path, sub1, (LPSTR) buffer);
    }
} /* UtilMakePathList */

INTEGER UtilDirDir2ListBox (HWND hWndListBox, LPSTR pathlist, LPSTR match)
{
    STATIC CHARACTER *curdir = "[..]";
    STATIC CHARACTER buffer[50];
    INTEGER nitems, len, matchlen;
    WORD wSel;

    SendMessage (hWndListBox, LB_RESETCONTENT, 0, 0L);
    SendMessage (hWndListBox, LB_DIR, DDL_DIRECTORY, (LONG) ((LPSTR) pathlist));
    nitems = (WORD) SendMessage (hWndListBox, LB_GETCOUNT, 0, 0L);
    for (wSel=0; wSel<nitems; wSel++) {
        SendMessage (hWndListBox, LB_GETTEXT, wSel, (LONG) ((LPSTR) buffer));
        if (strcmp (curdir, buffer))
            continue;
        SendMessage (hWndListBox, LB_DELETESTRING, wSel, 0L);
        nitems--;
        break;
    }
    if (!match OR !match[0]) {
        if (nitems>0)
            SendMessage (hWndListBox, LB_SETCURSEL, 0, 0L);
        return (nitems);
    }
    matchlen = strlen (match);
    for (wSel=0; wSel<nitems; wSel++) {
        len = (WORD) SendMessage (hWndListBox, LB_GETTEXTLEN, wSel, 0L);
        if ((len-2) != matchlen)
            continue;
        SendMessage (hWndListBox, LB_GETTEXT, wSel, (LONG) ((LPSTR) buffer));
        if (_strnicmp (match, buffer+1, matchlen))
            continue;
        break;
    }
    if (wSel >= nitems) wSel = 0;
    SendMessage (hWndListBox, LB_SETCURSEL, wSel, 0L);
    return (nitems);
} /* UtilDirDir2ListBox */

BOOL UtilCreateDirectoryChain (HWND hWnd, CHARACTER drivech, LPSTR dirpath)
{
    STATIC CHARACTER path[_MAX_PATH+1];
    INTEGER err, i, len;
    CHARACTER ch;

    strcpy (path, dirpath);
    path[0] = drivech;
    len = strlen (path);
    i = 0;
    for (; i<len; i++)
        if (path[i] EQ '\\') break; /* skip past a:\ */
    while (TRUE) {
        for (i++; i<len; i++)
            if (path[i] EQ '\\') break; /* get to a:\keytek\ */
        ch = path[i]; /* could be NULL if i is len */
        path[i] = '\0';
        err = _mkdir (path);
        path[i] = ch;
        if (err AND (errno EQ ENOENT))
            return (FALSE);
        if (i >= len)
            break;
    }
    return (TRUE);
} /* UtilCreateDirectoryChain */

BOOL UtilCheckFilename (HWND hWnd, LPSTR filename)
{
    INTEGER len, i;

    len = UtilStripName (filename);
    if (len EQ 0)
        return (FALSE);
    for (i=0; i<len; i++) {
        if (filename[i] EQ ' ')
            return (FALSE);
        if (filename[i] EQ '.')
            return (FALSE);
        if (filename[i] EQ '/')
            return (FALSE);
        if (filename[i] EQ '\\')
            return (FALSE);
        if (filename[i] EQ '+')
            return (FALSE);
        if (filename[i] EQ '\042')
            return (FALSE);
        if (filename[i] EQ '\133') /* [ */
            return (FALSE);
        if (filename[i] EQ '\135') /* ] */
            return (FALSE);
        if (filename[i] EQ '<')
            return (FALSE);
        if (filename[i] EQ '>')
            return (FALSE);
        if (filename[i] EQ '=')
            return (FALSE);
        if (filename[i] EQ ';')
            return (FALSE);
        if (filename[i] EQ ',')
            return (FALSE);
        if (filename[i] EQ ':')
            return (FALSE);
        if (filename[i] EQ '|')
            return (FALSE);
        if (filename[i] EQ '*')
            return (FALSE);
        if (filename[i] EQ '?')
            return (FALSE);
        if (filename[i] EQ '^')
            return (FALSE);
    }
    return (TRUE);
} /* UtilCheckFilename */

BOOL UtilCheckFilespec (HWND hWnd, LPSTR filespec)
{
    INTEGER len, i, elen;

    len = UtilStripName (filespec);
    if (len EQ 0)
        return (FALSE);
    elen = -1;
    for (i=0; i<len; i++) {
        if (filespec[i] EQ '.') {
            if (i EQ 0) /* no name, just the extension part */
                return (FALSE);
            if (elen >= 0) /* already found the dot */
                return (FALSE);
            elen = 0;
            continue;
        }
        if (filespec[i] EQ ' ')
            return (FALSE);
        if (filespec[i] EQ '/')
            return (FALSE);
        if (filespec[i] EQ '\\')
            return (FALSE);
        if (filespec[i] EQ '+')
            return (FALSE);
        if (filespec[i] EQ '\042')
            return (FALSE);
        if (filespec[i] EQ '\133') /* [ */
            return (FALSE);
        if (filespec[i] EQ '\135') /* ] */
            return (FALSE);
        if (filespec[i] EQ '<')
            return (FALSE);
        if (filespec[i] EQ '>')
            return (FALSE);
        if (filespec[i] EQ '=')
            return (FALSE);
        if (filespec[i] EQ ';')
            return (FALSE);
        if (filespec[i] EQ ',')
            return (FALSE);
        if (filespec[i] EQ ':')
            return (FALSE);
        if (filespec[i] EQ '|')
            return (FALSE);
        if (filespec[i] EQ '*')
            return (FALSE);
        if (filespec[i] EQ '?')
            return (FALSE);
        if (filespec[i] EQ '^')
            return (FALSE);
        if (elen >= 0) {
            elen++;
            if (elen > 3)
                return (FALSE);
        }
    } /* for each char in filename up until the extension begins */

    return (TRUE);
} /* UtilCheckFilespec */

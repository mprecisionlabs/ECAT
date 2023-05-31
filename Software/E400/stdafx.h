// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

#if !defined(_STDAFX_H__E400__)
#define _STDAFX_H__E400__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define VC_EXTRALEAN		// Exclude rarely-used stuff from Windows headers

#define _WIN32_IE 0x0400

#include <windows.h>
#include <windowsx.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strstream>
#include <commctrl.h>
#pragma comment(lib, "Comctl32.lib")

// Common stuff
#include "..\core\kt_util.h"		// common helper functions/handles/strutures.

// Local stuff
#include "revstr.h"
#include "resource.h"

using namespace std;

#endif // !defined(_STDAFX_H__E400__)

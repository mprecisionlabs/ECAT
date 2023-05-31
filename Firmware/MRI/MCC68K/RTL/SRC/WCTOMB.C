#include "lib_top.h"


#if EXCLUDE_wctomb
    #pragma option -Qws			/* Prevent compiler warning */
#else
 

/****************************************************************************/
/* THIS INFORMATION IS PROPRIETARY TO                                       */
/* MICROTEC RESEARCH, INC.                                                  */
/*--------------------------------------------------------------------------*/
/* Copyright (c) 1991, 1992 Microtec Research, Inc.                         */
/* All rights reserved                                                      */
/****************************************************************************/
/*
;The software source code contained in this file and in the related
;header files is the property of Microtec Research, Inc. and may only be
;used under the terms and conditions defined in the Microtec Research
;license agreement. You may modify this code as needed but you must retain
;the Microtec Research, Inc. copyright notices, including this statement.
;Some restrictions may apply when using this software with non-Microtec
;Research software.  In all cases, Microtec Research, Inc. reserves all rights.
*/


/************************************************************************/
/*	wctomb (char *s, wchar_t wchar)					*/
/*		s - destination address of multibyte character		*/
/*		wchar - source wide character				*/
/*									*/
/*	History --							*/
/*		07/15/91 jpc -	Written					*/
/************************************************************************/
/*	%W% %G%	*/

#include <stdlib.h>

int wctomb (char *s, wchar_t wchar)
{
    if (s == NULL)	/* indicate multi-byte chars do not have state- */
	return 0;	/* dependent encodings */

    *s = wchar;		/* simply copy wchar to *s */

    return 1;		/* Otherwise always return 1 */
}
 
#endif /* EXCLUDE_wctomb */
 

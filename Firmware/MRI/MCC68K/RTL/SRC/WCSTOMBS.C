#include "lib_top.h"


#if EXCLUDE_wcstombs
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
/*	wcstombs (char *s, const wchar_t *pwcs, size_t n)		*/
/*		s - destination address of multibyte character		*/
/*		wchar - source wide character				*/
/*		n - maximum number of elements to be translated		*/
/*									*/
/*	returns: (size_t) -1 if an illegal wide char was found,		*/
/*		otherwise the number of elements translated		*/
/*									*/
/*	History --							*/
/*		07/15/91 jpc -	Written					*/
/************************************************************************/
/*	%W% %G%	*/

#include <stdlib.h>

size_t wcstombs (register char *s, register const wchar_t *pwcs, size_t n)
{
register size_t	count;

    count = n;				/* use count in computations */

    while (count) {
	if ((*s = *pwcs) == '\0') break;	/* stop if null found */
	--count;
	++s;
	++pwcs;
	}

    return n - count;
}
 
#endif /* EXCLUDE_wcstombs */
 

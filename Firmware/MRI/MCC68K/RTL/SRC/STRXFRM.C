#include "lib_top.h"


#if EXCLUDE_strxfrm
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
/*	size_t strxfrm (char *s1, const char *s2, size_t n);		*/
/*		s1 - address of destination string			*/
/*		s2 - address of source string				*/
/*		n - max number of bytes to be transferred to s1		*/
/*									*/
/*	returns: length of the tranformed string			*/
/*									*/
/*	History --							*/
/*		07/17/91 jpc -	Written					*/
/************************************************************************/
/*	%W% %G%	*/

#include <string.h>

size_t strxfrm (char *s1, register const char *s2, size_t n)
{
register size_t strSize;

    strSize = strlen (s2);		/* Get size of source */

    if (n <= strSize)			/* if dest is too small */
	return strSize;			/* simply return size of source */

    strcpy (s1, s2);			/* Simply move s2 -> s1 */

    return strSize;			/* and return # chars moved minus */
					/* null */
}
 
#endif /* EXCLUDE_strxfrm */
 

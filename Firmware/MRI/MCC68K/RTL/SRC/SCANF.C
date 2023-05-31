#include "lib_top.h"


#if EXCLUDE_scanf
    #pragma option -Qws			/* Prevent compiler warning */
#else

/****************************************************************************/
/* THIS INFORMATION IS PROPRIETARY TO                                       */
/* MICROTEC RESEARCH, INC.                                                  */
/*--------------------------------------------------------------------------*/
/* Copyright (c) 1987, 1988, 1989, 1991, 1992 Microtec Research, Inc.       */
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


/*	%W% %G%	*/

/************************************************************************/
/* NAME                         S C A N F                               */
/*                                                                      */
/* FUNCTION     scanf - scan formatted input from standard input        */
/*                                                                      */
/* SYNOPSIS     scanf (fmt, va_alist)                                   */
/*              char*   fmt - scan format string pointer                */
/*              va_dcl                                                  */     
/*                                                                      */
/* RETURN                                                               */
/*                                                                      */
/* DESCRIPTION                                                          */
/*                                                                      */
/* MODIFICATION HISTORY                                                 */
/*                                                                      */
/*      MCC86 C Library  - Copyright 1986, Hunter & Ready Inc.          */
/*	 8/04/89 jpc	Changed _scanf to _scanf			*/
/*	 9/16/89 jpc	Added mrilib.h for easy build of '881 stuff	*/
/*	11/06/91 jpc	removed special case for non ANSI		*/
/************************************************************************/
#include <stdio.h>
#include "mrilib.h"

#include <stdarg.h>

extern void _sfef();

int scanf(const char *fmt, ...)
{
    int ret;
    va_list pvar;

#if ARGS_BY_REGISTER
    va_start (pvar, (char *) fmt);
#else
    va_start (pvar, fmt);
#endif
    ret = _scanf(fgetc, ungetc, (char *) stdin, (char *) fmt, pvar);
    va_end(pvar);
    return ret;
}

#endif /* EXCLUDE_scanf */


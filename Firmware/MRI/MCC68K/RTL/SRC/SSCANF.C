#include "lib_top.h"


#if EXCLUDE_sscanf
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
/* NAME                         S S C A N F                             */
/*                                                                      */
/* FUNCTION    sscanf - scan formatted input from a sting               */ 
/*                                                                      */
/* SYNOPSIS    sscanf(str, fmt, va_alist)                               */
/*                                                                      */
/* RETURN                                                               */
/*                                                                      */
/* DESCRIPTION                                                          */
/*                                                                      */
/* MODIFICATION HISTORY                                                 */
/*                                                                      */
/*      MCC86 C Library  - Copyright 1986, Hunter & Ready Inc.          */
/*	 9/16/89 jpc	Added mrilib.h for easy '881 build		*/
/*	 6/19/90 jpc	Removed stdio.h to get rid of _iob struct	*/
/*	11/06/91 jpc	Removed special cases for non ANSI		*/
/************************************************************************/

#include "mrilib.h"

#include <stdarg.h>

extern void _sfef();

int sscanf(const char *str, const char *fmt, ...)
{
    int ret;
    va_list pvar;

#if ARGS_BY_REGISTER
    va_start (pvar, (char *) fmt);
#else
    va_start (pvar, fmt);
#endif
    ret = _scanf((int (*)())0, (int (*)())0, (char *) str, (char *) fmt, pvar);
    va_end(pvar);
    return ret;
}

#endif /* EXCLUDE_sscanf */


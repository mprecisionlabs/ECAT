#include "lib_top.h"


#if EXCLUDE_fscanf
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

/*	History								    */
/*	 9/16/89 jpc	Added mrilib for easy build of '881 stuff	    */
/*	11/06/91 jpc	Removed special case for non ANSI		    */
/****************************************************************************/

/* %Z%  %M%  %I%  %G% */

/* scan formatted input from stream
 *
 * Copyright 1986, Hunter & Ready Inc.
 */

#include <stdio.h>
#include "mrilib.h"

extern void _sfef();

#include <stdarg.h>

int fscanf(FILE *fp, const char *fmt, ...)
{
    int ret;
    va_list pvar;

#if ARGS_BY_REGISTER
    va_start (pvar, (char *) fmt);
#else
    va_start (pvar, fmt);
#endif
    ret = _scanf(fgetc, ungetc, (char *) fp, (char *) fmt, pvar);
    va_end(pvar);
    return ret;
}

#endif /* EXCLUDE_fscanf */


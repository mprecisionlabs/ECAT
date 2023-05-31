#include "lib_top.h"


#if EXCLUDE_fprintf
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


/* %W% %G% */

/************************************************************************
 *  fprintf.c
 *		FPRINTF
 *
 *	int  fprintf (format [,arg]...)
 *
 *	fprintf places output on the named output stream.
 *	It returns the number of characters output, or a negative value if
 *	an output error was encountered.
 *
 *  written by H. Yuen  12/12/86
 *
 * revision history for fprintf.c:
 *	27-Jun-88 kjk:	this module split out from printf.c
 *	09/24/88 jpc:	added __STDC__ routine stolen from G32
 *      06/14/90 jpc:	converted to pass address of putc function to _mri_prt.
 *	11/19/91 jpc:	removed __STDC__ constructs
 *
 *************************************************************************/
/* (last mod 11/04/87  HY) */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "mrilib.h"

#undef  putc                    /* get rid of putc macro */

extern int _mri_prt();

#include <stdarg.h>

int fprintf (FILE *fp, const char *fmt, ...)
{
	int ret;
	va_list arg;

#if ARGS_BY_REGISTER
	va_start (arg, (char *) fmt);
#else
	va_start (arg, fmt);
#endif
	ret = _mri_prt (fp, (char *) fmt, arg, putc);
	va_end (arg);
	return ret;
}

#endif /* EXCLUDE_fprintf */


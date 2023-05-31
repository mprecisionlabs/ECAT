#include "lib_top.h"


#if EXCLUDE_sprintf
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

/************************************************************************
 *  sprintf.c
 *		SPRINTF
 *
 *	int  sprintf (cp,format [,arg]...)
 *
 *	sprintf places "output", followed by a null character (\0) in
 *	consecutive bytes starting at *cp; user must ensure that enough
 *	storage is available.
 *	It returns the number of characters output (excluding \0), or a
 *	negative value if an output error was encountered.
 *
 *  written by H. Yuen  12/12/86
 *
 *  08/16/89 jpc - added mrilib.h to aid in building 68k '881 versions
 *  09/24/89 jpc - added __STDC__ stuff from G32
 *  11/19/91 jpc - removed __STDC__ constructs
 *
 *************************************************************************/
/* (last mod 07/01/88  HY) */

#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include "mrilib.h"

extern int _mri_prt ();

static int transChar (int c, unsigned char **addr)
{
    *(*addr)++ = c;			/* put a char into the buffer */
    return c;				/* make transChar look like putc */
}

int sprintf(char *str, const char *fmt, ...)
{
	va_list arg;
	register int nc;

#if ARGS_BY_REGISTER
	va_start (arg, (char *) fmt);
#else
	va_start (arg, fmt);
#endif
	nc = _mri_prt (&str, (char *) fmt, arg, transChar);
	va_end(arg);
	*str = '\0';		/* Terminate string */
	return (nc);
}

#endif /* EXCLUDE_sprintf */


#include "lib_top.h"


#if EXCLUDE_vfprintf
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


/*	%W% %G%	*/

/************************************************************************
 *  vfprintf.c
 *		VFPRINTF
 *
 *	int  vfprintf (FILE *stream, const char *format, va_list arg);
 *
 *	vfprintf places output on the named output stream.
 *	It returns the number of characters output, or a negative value if
 *	an output error was encountered.
 *
 * Code stolen from fprintf.c on 02/21/91
 *
 * revision history for vfprintf.c:
 *
 *************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "mrilib.h"

#undef  putc                    /* get rid of putc macro */

extern int _mri_prt();

#include <stdarg.h>

int vfprintf (FILE *fp, const char *fmt, va_list arg) 
{
	return _mri_prt (fp, (char *) fmt, arg, putc);
}

#endif /* EXCLUDE_vfprintf */


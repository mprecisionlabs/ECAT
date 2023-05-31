#include "lib_top.h"


#if EXCLUDE_vsprintf
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
 *  vsprintf.c
 *		VSPRINTF
 *
 *	int  vsprintf (char *s, const char *format, va_list arg);
 *
 *	vsprintf places "output", followed by a null character (\0) in
 *	consecutive bytes starting at *s; user must ensure that enough
 *	storage is available.
 *	It returns the number of characters output (excluding \0), or a
 *	negative value if an output error was encountered.
 *
 *  Stolen from sprintf.c on 02/21/91
 *
 *************************************************************************/

#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include "mrilib.h"

extern int _mri_prt ();

static int transChar (int c, unsigned char **addr)

{
    *(*addr)++ = c;			/* put a char into the buffer */
    return c;				/* make transChar act like putc */
}

int vsprintf(char *str, const char *fmt, va_list arg)

{
int returnVal;

	returnVal = _mri_prt (&str, (char *) fmt, arg, transChar);
	*str = '\0';		/* Terminate string */
	return returnVal;
}

#endif /* EXCLUDE_vsprintf */


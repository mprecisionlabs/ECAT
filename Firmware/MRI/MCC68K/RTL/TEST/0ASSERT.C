#include "lib_top.h"


#pragma	-nx

/****************************************************************************/
/* THIS INFORMATION IS PROPRIETARY TO                                       */
/* MICROTEC RESEARCH, INC.                                                  */
/*--------------------------------------------------------------------------*/
/* Copyright (c) 1992 Microtec Research, Inc.				    */
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
/* NAME                      0 a s s e r t				*/ 
/*                                                                      */
/* FUNCTION     0assert - Test assert macro.				*/
/*                                                                      */
/* DESCRIPTION                                                          */
/*		0assert tests the assert function by forcing an assert	*/
/*		then intercepting the output of the _ASSERT function as	*/
/*		it attempts to call fprintf.				*/
/*		0assert also intercepts to call to abort from the 	*/
/*		_ASSERT function.  At this time it prints the results	*/
/*		of the test.						*/
/*                                                                      */
/* MODIFICATION HISTORY                                                 */
/*	12/22/92 jpc:	written.					*/
/*	12/13/93 jpc:	Upper case file name to allow to work on VMS 	*/
/*			and DOS.					*/
/*                                                                      */
/************************************************************************/

#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>

#if     _MCC88K || _MCCSP || _MCC29K || _MCC80 || _MCC960
    #define ARGS_BY_REGISTER    1
#endif

#if sun
    #include <memory.h>
    #undef NULL
    #define NULL ((void *)0)
#endif

#include "message.h"

static char format[] = "Assertion failed: (%s) file %s, line %d\n";

static int  assert_line;		/* assert_line == line # of assert */
static int  error;
static int  fprintf_flag;

main () {
	assert_line = __LINE__ + 1;
	assert (0);
}

#if sun				/* Sun header has no prototypes. */
    int fprintf (file, format_str)
    FILE *file;
    const char *format_str;
#else
    int fprintf (FILE *file, const char *format_str, ...)
#endif
{

va_list arg;
const char *temp_pnt, *scan_pnt;
int  line;

fprintf_flag++;			/* set flag to indicate routine was run */

#if ARGS_BY_REGISTER
va_start (arg, (char *) format_str);
#else
va_start (arg, format_str);
#endif

if (file != stderr) {
	printf ("Error:  file specified is not \"stderr\"\n");
	error++;
	}

if (strcmp (format, (char *) format_str)) {
	printf ("Error:  format string specified is incorrect\n");
	error++;
	}

if (strcmp (temp_pnt = va_arg (arg, const char *), "0")) {
	printf ("Error:  expression is incorrect - expected (0) "
		"actual (%s)\n", (char *) temp_pnt);
	error++;
	}

scan_pnt = temp_pnt = va_arg (arg, const char *);

for (scan_pnt--; *++scan_pnt; )
    *(char *)scan_pnt = toupper (*scan_pnt);

if ((strstr ((char *) temp_pnt, (char *) "0ASSERT.C") == NULL)) {
	printf ("Error:  file is incorrect - expected (0assert.c) "
		"actual (%s)\n", (char *) temp_pnt);
	error++;
	}

if ((line = va_arg (arg, int)) != assert_line) {
	printf ("Error:  line number is incorrect - expected (%d) "
		"actual (%d)\n", assert_line, line);
	error++;
	}

va_end (arg);

}
 
#if sun
    int abort ()
#else
    void abort (void)
#endif
{

if (! fprintf_flag)
	++error;	/* if fprintf not called -- it was an error */

if (error)
	printf (ERRORS);
else
	printf (NO_ERRORS);

exit (0);
}

#include "lib_top.h"


#if EXCLUDE_nonlputs
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
/* NAME                  _ N O _ N L _ P U T S				*/ 
/*                                                                      */
/* FUNCTION     _no_no_puts - copy string to console up to '/0'		*/
/*                                                                      */
/* SYNOPSIS     _no_nl_puts(s)						*/
/*              char *s - string to be copied                           */
/*                                                                      */
/* RETURN       0 if string empty. EOF if an error occurs.		*/
/*                                                                      */
/* DESCRIPTION                                                          */
/*                                                                      */
/* MODIFICATION HISTORY                                                 */
/*      Author - wb denniston 09/30/86                                  */
/*	09/14/89 jpc:	Removed writing of '\r' at end of string	*/
/*			Returned value of putc when '\n' is written	*/
/*	04/11/90 jpc:	added _MCC88K to #if's				*/
/*	01/16/91 jpc:   remove writing of \r				*/
/*	11/19/91 jpc:	removed __STDC__ constructs			*/
/*	12/22/92 jpc:	stole code from puts.c and modified for		*/
/*			nonlputs.					*/
/*                                                                      */
/************************************************************************/

#include <stdio.h>

int _no_nl_puts (register const char *str)
{
    register int ch;

    --str;				/* Allow for pre-increment in loop */
    while (ch = *++str)
	if ((putchar)(ch) == EOF)
		return (EOF);
    return 0;
}
 
#endif /* EXCLUDE_nonlputs */
 

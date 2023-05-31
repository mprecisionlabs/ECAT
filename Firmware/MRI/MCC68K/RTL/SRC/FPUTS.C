#include "lib_top.h"


#if EXCLUDE_fputs
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
/* NAME                        F P U T S                                */ 
/*                                                                      */
/* FUNCTION     fputs - copy string to file up to '/0'                  */
/*                                                                      */
/* SYNOPSIS     fputs(s,f)                                              */
/*              char *s - string to be copied                           */
/*              FILE *f - destination file pointer for string           */
/*                                                                      */
/* RETURN       Last character sent if ok. 0 if string empty. EOF if    */
/*              an error occurs.                                        */
/*                                                                      */
/* DESCRIPTION                                                          */
/*                                                                      */
/* MODIFICATION HISTORY                                                 */
/*      Author - wb denniston 09/30/86                                  */
/*	11/19/91 jpc:	removed __STDC__ construct			*/
/*                                                                      */
/************************************************************************/
#include <stdio.h>

int fputs (const char *str, FILE *file)
{
    register int ch;

    if (*str == '\0') return (0) ;
    while (ch = *str++)
	if (putc(ch,file) == EOF)
		return (EOF);
    return (ch);
}
 
#endif /* EXCLUDE_fputs */
 

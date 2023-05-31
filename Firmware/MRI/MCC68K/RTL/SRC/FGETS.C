#include "lib_top.h"


#if EXCLUDE_fgets
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
/* NAME                         F G E T S                               */
/*                                                                      */
/* FUNCTION     fgets - get a string from a file till maxlen, linefeed, */
/*                      or EOF occurs.                                  */
/*                                                                      */
/* SYNOPSIS     fgets (str, maxlen, file)                               */
/*              char    *str - buffer big enough for maxlen characters. */
/*              int     maxlen - largest number of characters to get    */
/*              FILE    *file - source of characters                    */
/*                                                                      */
/* RETURN       Pointer to string if OK. Else NULL.                     */
/*                                                                      */
/* DESCRIPTION  Fills buffer str with characters from file until        */
/* linefeed is detected, maxlen - 1 characters are read, or EOF is      */
/* detected. A '\0' is added to the string.                             */
/*                                                                      */
/* MODIFICATION HISTORY                                                 */
/*      Author - wb denniston 09/30/86                                  */
/*	11/19/91 jpc:	removed __STDC__ constructs			*/
/*                                                                      */
/************************************************************************/
#include <stdio.h>

char *fgets(char *str, int maxlen, FILE *file)
{
    register char *pstr = str;
    register int ch;

    while (--maxlen > 0 && (ch = getc(file)) != EOF) {
	*pstr++ = ch;
	if (ch == '\n') break;
    }
    *pstr = '\0';
    if (ch == EOF && pstr == str)	/* nothing has been read */
	return (NULL);
    return (str);
}
 
#endif /* EXCLUDE_fgets */
 

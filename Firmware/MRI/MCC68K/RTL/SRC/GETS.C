#include "lib_top.h"


#if EXCLUDE_gets
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

/* get string from standard input and strip the terminating new-line
 *	"str" is a pointer to where to put the string
 *	returns the argument "str", or zero upon end-of-file or error
 *
 * Copyright 1985, Hunter & Ready Inc.
 */

/****************************************************************************/
/* Revisions:								*/
/*	11/19/91 jpc:	removed __STDC__ constructs			*/
/****************************************************************************/

#include <stdio.h>

char *gets(char *str)
{
	FILE *si = stdin;
	char *ptr;
	int ch;
	ch = getc(si);
	if (ch == EOF)
		return 0;
	ptr = str;
	while (ch != '\n') {
		*ptr++ = ch;
		ch = getc(si);
		if (ch == EOF)
			break;
	}
	*ptr = '\0';
	return str;
}
 
#endif /* EXCLUDE_gets */
 

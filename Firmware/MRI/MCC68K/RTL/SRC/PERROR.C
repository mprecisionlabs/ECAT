#include "lib_top.h"


#if EXCLUDE_perror
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
 *  perror.c
 *
 *	Maps an errno to an error message string and write to stderr.
 *
 *  The error numbers are defined in <errorno.h> and also compatiable
 *  with UNIX System V.  This function calls strerror().
 *
 *  Written by H. Yuen  03/13/91
 *
 ************************************************************************/
/* (last mod 03/14/91  HY) */

#include <string.h>
extern	int errno;

void perror (const char *s)
{
	char *p;

	if (s && *s) {
	    write(2, (char *)s, strlen(s));
	    write(2, ": ", 2);
	}
	p = strerror(errno);
	write(2, p, strlen(p));
	write(2, "\n", 1);
}
 
#endif /* EXCLUDE_perror */
 

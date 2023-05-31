#include "lib_top.h"


#pragma	option	-nx

/************************************************************************/
/* THIS INFORMATION IS PROPRIETARY TO					*/
/* MICROTEC RESEARCH, INC.						*/
/* 2350 Mission College Blvd.						*/
/* Santa Clara, CA 95054						*/
/* USA									*/
/*----------------------------------------------------------------------*/
/* Copyright (c) 1991-1992  Microtec Research, Inc.			*/
/* ALL RIGHTS RESERVED							*/
/************************************************************************/
/*
;The software source code contained in this file and in the related
;header files is the property of Microtec Research, Inc. and may only be
;used under the terms and conditions defined in the Microtec Research
;license agreement. You may modify this code as needed but you must retain
;the Microtec Research, Inc. copyright notices, including this statement.
;Some restrictions may apply when using this software with non-Microtec
;Research software.  In all cases, Microtec Research, Inc. reserves all rights.
*/


/*
	Test routine for vfprintf.c
*/

/*	%W% %G%	*/

#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include "message.h"

tester (char *fmt, ...)
{
va_list args;
char array [256];

    memset (array, (int) 'a', sizeof (array));
    va_start (args, fmt);
    vsprintf (array, fmt, args);
    printf (array);			/* actually output string */
    va_end (args);
}
 
main ()
{
	tester ("%s %s %s %s %s %s %s%s",
		"This", "test", "has", "completed", "with", "no",
		"errors.", "\n");
}


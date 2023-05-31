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


/************************************************************************/
/*	Test for wctomb ()						*/
/************************************************************************/
/*	%W% %G%	*/

#include <stdlib.h>
#include <stdio.h>

#include "message.h"

int errorCount;
char string[] = "source";
char dest[] = "dest";

main ()
{
    if (wctomb (NULL, (wchar_t) 2) != 0) {
	puts ("Error: wctomb returned non-zero for s=NULL.");
	errorCount++;
	}

    if (wctomb (dest, (wchar_t) 's') != 1) {
	puts ("Error: wctomb did not return 1 in normal operation.");
	errorCount++;
	}
    if (*dest != 's') {
	puts ("Error: wctomb did not execute assignment to s.");
	errorCount++;
	}

    if (errorCount)
	printf (ERRORS);
    else
	printf (NO_ERRORS);
}
 

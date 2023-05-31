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
/*	Test for mblen ()						*/
/************************************************************************/
/*	%W% %G%	*/

#include <stdlib.h>
#include <stdio.h>

#include "message.h"

int errorCount;
char string[25];

main ()
{
    if (mblen (NULL, 2) != 0) {
	puts ("Error: mblen returned non-zero for s=NULL.");
	errorCount++;
	}

    if (mblen (string, 0) != -1) {
	puts ("Error: mblen did not return -1 when n=0.");
	errorCount;
	}

    if (mblen (string, 10) != 1) {
	puts ("Error: mblen did not return 1 in normal operation.");
	errorCount++;
	}

    if (errorCount)
	printf (ERRORS);
    else
	printf (NO_ERRORS);
}
 

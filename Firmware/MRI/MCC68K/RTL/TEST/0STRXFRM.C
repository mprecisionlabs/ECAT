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
/*	Test routine for strxfrm ()					*/
/************************************************************************/
/*	%W% %G%	*/

#include <string.h>
#include <stdio.h>

#include "message.h"

char source[] = "source";
char dest[] = "destination";
char array[25];

int errorCount;

main ()
{
/****************************************************************/
/*	Note: first test is example from ANSI spec.		*/
/****************************************************************/

    if (strxfrm (NULL, source, 0) != (sizeof(source) - 1)) {
	puts ("Error: strxfrm returns wrong size in test 1.");
	errorCount++;
	}

    strcpy (array, dest);			/* preset */
    if (strxfrm (array, source, 200) != (sizeof(source) - 1)) {
	puts ("Error: strxfrm returns wrong size in test 2.");
	errorCount++;
	}
    if (strcmp (array, source)) {
	puts ("Error: strxfrm failed to copy correctly in test 2.");
	errorCount++;
	}

    strcpy (array, dest);			/* preset */
    if (strxfrm (array, source, sizeof(source)) != (sizeof(source) - 1)) {
	puts ("Error: strxfrm returns wrong size in test 3.");
	errorCount++;
	}
    if (strcmp (array, source)) {
	puts ("Error: strxfrm failed to copy correctly in test 3.");
	errorCount++;
	}

    if (errorCount) {
	printf (ERRORS);
	return (1);
	}
    else {
	printf (NO_ERRORS);
	return (0);
	}
}
 

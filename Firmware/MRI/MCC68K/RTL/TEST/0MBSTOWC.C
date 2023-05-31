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
/*	Test for mbstowcs ()						*/
/************************************************************************/
/*	%W% %G%	*/

#include <stdlib.h>
#include <stdio.h>

#include "message.h"

int errorCount;
char string[] = "source";
char dest[] =   "destination";
char array[25];

main ()
{
    strcpy (array, dest);		/* preset */
    if (mbstowcs ((wchar_t *) array, string, 200) != (sizeof (string) - 1)) {
	puts("Error: mbstowcs returned incorrect conversion count for test 1.");
	errorCount++;
	}
    if (strcmp (array, "source")) {
	puts ("Error: mbstowcs transferred incorrectly in test 1.");
	errorCount++;
	}

    strcpy (array, dest);		/* preset */
    if (mbstowcs ((wchar_t *) array, string, (sizeof(string) -1))
	    != (sizeof (string) - 1)){
	puts("Error: mbstowcs returned incorrect conversion count for test 2.");
	errorCount++;
	}
    if (strcmp (array, "sourceation")) {
	puts ("Error: mbstowcs transferred incorrectly in test 2.");
	errorCount++;
	}

    if (errorCount)
	printf (ERRORS);
    else
	printf (NO_ERRORS);
}
 

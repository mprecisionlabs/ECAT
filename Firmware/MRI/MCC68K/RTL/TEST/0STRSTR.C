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
	Test routine for strstr.c
*/

/*	%W% %G%	*/

#include <string.h>
#include <stdio.h>

#include "message.h"

/*		        01234567890123456789012345		*/
static char alpha [] = "abcdefghijklmnopqrstuvwxyz";


main ()
{
int i;
int j;
int errorCount = 0;
char * returnVal;
char testString[40];

    for (i=0; i<24; i++)			/* 3 character test strings */
	{
	for (j=0; j<3; j++)			/* build test string */
	    testString[j] = alpha[i+j];

	testString[3] = 0;			/* Term string */

	if ((returnVal = strstr (alpha, testString)) != &alpha[i])
	    {
	    errorCount++;
	    printf ("Error: strstr 1: i = %d expected = %p actual = %p\n",
		i, &alpha[i], returnVal);
	    }
	}

    for (i=0; i<26; i++)			/* 1 character test strings */
	{
	testString[0] = alpha[i];

	testString[1] = 0;			/* Term string */

	if ((returnVal = strstr (alpha, testString)) != &alpha[i])
	    {
	    errorCount++;
	    printf ("Error: strstr 2: i = %d expected = %p actual = %p\n",
		i, &alpha[i], returnVal);
	    }
	}

    if ((returnVal = strstr (alpha, "test")) != (char *) NULL)
	{
	errorCount++;
	printf ("Error: strstr 3: expected = %p actual = %p\n",
	    (char *) NULL, returnVal);
	}

    if ((returnVal = strstr (alpha, "1")) != (char *) NULL)
	{
	errorCount++;
	printf ("Error: strstr 4: expected = %p actual = %p\n",
	    (char *) NULL, returnVal);
	}

    if (errorCount)
	printf (ERRORS);
    else
	printf (NO_ERRORS);
}
 

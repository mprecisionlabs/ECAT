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
	bsearch test routine
*/

/*	%W% %G%	*/

#include <stdio.h>

#ifdef sun
#include <search.h>
#define void char
#undef	NULL
#define NULL (char *) 0
#else
#include <stdlib.h>
#endif

#include "message.h"

const char * tests [17];		/* Prime # of elements */
	
const char  zero[3] = "00";
const char eighteen[3] = "18";
const char fortyone[4] = "041";

const char tests_2[17][3] = {
	"01",
	"02",
	"03",
	"04",
	"05",
	"06",
	"07",
	"08",
	"09",
	"10",
	"11",
	"12",
	"13",
	"14",
	"15",
	"16",
	"17" };


#ifdef sun
int testFunct (ptr1, ptr2)
char * ptr1, * ptr2;
#else
int testFunct (void * ptr1, void * ptr2)
#endif
{
return strcmp (*(void **) ptr1, *(void **) ptr2);
}

#ifdef sun
int testFunct_2 (ptr1, ptr2)
char * ptr1, * ptr2;
#else
int testFunct_2 (void * ptr1, void * ptr2)
#endif
{
return strcmp ((void *) ptr1, (void *) ptr2);
}

main ()
{
int i;
int errorCount = 0;
char ** returnVal;
char * returnVal2;

        tests[0] = "01";
	tests[1] = "02";
	tests[2] = "03";
	tests[3] = "04";
	tests[4] = "05";
	tests[5] = "06";
	tests[6] = "07";
	tests[7] = "08";
	tests[8] = "09";
	tests[9] = "10";
	tests[10] = "11";
	tests[11] = "12";
	tests[12] = "13";
	tests[13] = "14";
	tests[14] = "15";
	tests[15] = "16";
	tests[16] = "17";

for (i=0; i<17; i++)
    {
    if ((returnVal = (char **) bsearch
	    (&tests[i], tests, 17, sizeof (char *), testFunct)) != &tests[i])
	{
	printf ("Error: bsearch 1: i = %d expected = %p actual = %p\n",
	    i, &tests[i], returnVal);
	errorCount++;
	}
    }

if ((returnVal = (char **) bsearch
	    (&zero, tests, 17, sizeof (char *), testFunct)) != (char **) NULL)
	{
	printf ("Error: bsearch 2: expected = %p actual = %p\n",
	    NULL, returnVal);
	errorCount++;
	}

if ((returnVal = (char **) bsearch
	    (&eighteen, tests, 17, sizeof (char *), testFunct)) !=
	    (char **) NULL)
	{
	printf ("Error: bsearch 3: expected = %p actual = %p\n",
	    NULL, returnVal);
	errorCount++;
	}

if ((returnVal = (char **) bsearch
	    (&fortyone, tests, 17, sizeof (char *), testFunct)) !=
	    (char **) NULL)
	{
	printf ("Error: bsearch 4: expected = %p actual = %p\n",
	    NULL, returnVal);
	errorCount++;
	}

for (i=0; i<17; i++)
    {
    if ((returnVal2 = (char *) bsearch
	    (tests_2[i], tests_2, 17, tests_2[1] - tests_2[0], testFunct_2)) 
	    != tests_2[i])
	{
	printf ("Error: bsearch 5: i = %d expected = %p actual = %p\n",
	    i, tests_2[i], returnVal2);
	errorCount++;
	}
    }

if (errorCount)
    printf (ERRORS);
else
    printf (NO_ERRORS);
}
 

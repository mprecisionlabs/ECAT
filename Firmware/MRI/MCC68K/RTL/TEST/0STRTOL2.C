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


/*	%W% %G%	*/


#include <errno.h>
#include <limits.h>
#include <stdlib.h>
#include <stdio.h>

#include "message.h"

/*			   012345678901234567890123456		*/
char master[50] = "0123456789abcdefghijklmnopqrstuvwxyz\t";

long  masterResult[] = {
		1, 5, 11, 19, 29, 41, 55, 71, 89, 109, 131, 155, 181,
		209, 239, 271, 305, 341, 379, 419, 461, 505, 551, 599,
		649, 701, 755, 811, 869, 929, 991, 1055, 1121, 1189,
		1259 };

struct {
	char str[20];
	int	base;
	long	result;
	} strings [] = {
	{ "9999999999999x", 0, LONG_MAX},
	{ "0xdeadz", 0, 57005},
	{ "07654z", 0, 4012},
	{ "-9999999999999x", 0, LONG_MIN},
	{ "-0xdeadz", 0, -57005},
	{ "-07654z", 0, -4012} };

main ()
{
int i;
char * retChar;
long	tempLong;
int	errorFlag = 0;

for (i=0; i<35; i++) {
    if ((tempLong = strtol (&master[i], &retChar, i + 2)) != masterResult[i]) {
	errorFlag++;
	printf ("Error: strtol 1:");
	printf (" i = %d input = \"%s\" expected = %d actual = %d\n",
	    i, &master[i], masterResult[i], tempLong);
	}
    if (retChar != &master[i+2]) {
	errorFlag++;
	printf ("Error: strtol 2: bad address i = %d actual %p expected %p\n",
	    i, retChar, &master[i+2]);
	}
    }

for (i=0; i < (sizeof(strings) / sizeof(strings[0])); i++) {
    if ((tempLong = strtol (strings[i].str, &retChar, strings[i].base)) != 
		strings[i].result) {
	errorFlag++;
	printf ("Error: strtol 3: ");
	printf ("i = %d input = \"%s\" expected %d actual = %d\n",
	    i, strings[i].str, strings[i].result, tempLong);
	}
    if (retChar != (strings[i].str + strlen(strings[i].str)-1)) {
	errorFlag++;
	printf ("Error: strtol 2: bad address i = %d actual %p expected %p\n",
	    i, retChar, &master[i+2]);
	}
    }
if (errorFlag)
    printf (ERRORS);
else
    printf (NO_ERRORS);
}
 

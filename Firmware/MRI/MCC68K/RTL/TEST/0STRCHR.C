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
/*	Routine to test strchr ().					*/
/*	    Originally written to exercise the 88k assembly language	*/
/*	    strchr () routine						*/
/*									*/
/*	History:							*/
/*	    04/13/90 jpc - Written					*/
/*									*/
/************************************************************************/

/*	%W% %G%	*/

#include <string.h>
#include <stdio.h>

#include "message.h"

#define	OFFSET	1	/* make sure it works for odd addresses */

static char source[30];
static int error;
static int middle;

static void testIt (char match, char noMatch, int error);
static void setUp (int middle, char everyWhere, char find);

main ()
{
int i;

middle = sizeof(source)/2;
/* Setup */

setUp (middle, 0xff, 0xfe);
testIt (0xfe, 0xfd, 1);
testIt (0xfe, 0x1, 1);

setUp (middle, 0xff, 0x1);
testIt (0x1, 0xfe, 2);
testIt (0x1, 0x2, 2);

setUp (middle, 0x1, 0xff);
testIt (0xff, 0xfe, 3);
testIt (0xff, 0x2, 3);

setUp (middle, 0x1, 0x2);
testIt (0x2, 0xfe, 4);
testIt (0x2, 0x3, 4);

if (error)
    printf (ERRORS);
else
    printf (NO_ERRORS);
}

static void printError (int group, int localError)
{
printf ("Error condition found; group %d - error %d\n", group, localError);
error = 1;
}

static void testIt (char match, char noMatch, int error)
{
if (strchr(source + OFFSET, match) != (source + middle))
	printError (error, 1);
if (strchr(source + OFFSET, noMatch) != 0)
	printError (error, 2);
}

static void setUp (int middle, char everyWhere, char find)
{
memset (source, everyWhere, sizeof(source)-1);
source[sizeof(source)-1] = 0;
source[middle] = find; 
}
 

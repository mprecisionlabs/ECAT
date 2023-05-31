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



/*	Routine to test the setlocale routine */

/*	%W% %G%	*/

#include <locale.h>
#include <stdio.h>

#include "message.h"

static int errorCount;
static void error (int value);

main ()
{

if (strcmp (setlocale (5, NULL), "C"))			error (1);
if (strcmp (setlocale (5, ""  ), "C"))			error (2);
if (strcmp (setlocale (5, "C" ), "C"))			error (3);
if (setlocale (5, "barf") != NULL)			error (4);

if (errorCount)
	printf (ERRORS);
else
	printf (NO_ERRORS);
}

static void error (int value)
{
printf ("Error %d\n", value);
errorCount++;
}
 

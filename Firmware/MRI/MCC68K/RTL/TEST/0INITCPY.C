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
/*									*/
/*	This routine tests whether or not the "initdata" command	*/
/*	works with the linker.  The linker command file must be changed */
/*	and csys*.c must be recompiled with the _INITDATA switch turned	*/
/*	on								*/
/*									*/
/************************************************************************/

/*	%W% %G%	*/

#include <stdio.h>

#include "message.h"

int	var1;
int	var2 = 5;

main ()
{
int	errCount = 0;

if (var1 != 0) {
	printf ("Error #1\n");
	errCount++;
	}

if (var2 != 5) {			/* Error indicates that _initcopy () */
	printf ("Error #2\n");		/* may not have been called */
	errCount++;
	}

var2 = 100;				/* set var2 to something else */
_initcopy ();				/* set it back to original value */

if (var2 != 5) {
	printf ("Error #3\n");
	errCount++;
	}

if (errCount)
	printf (ERRORS);
else
	printf (NO_ERRORS);
}
 

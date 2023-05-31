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


/*	Routine to test div () and ldiv ()	*/

/*	%W% %G%	*/

#include <stdlib.h>
#include <stdio.h>
#include "message.h"

static int errorCount;

main ()
{
div_t	divReturn;
ldiv_t	ldivReturn;

divReturn = div (25, 4);
if (divReturn.quot != 6)	error (1);
if (divReturn.rem  != 1)	error (2);

divReturn = div (4, 25);
if (divReturn.quot != 0)	error (3);
if (divReturn.rem  != 4)	error (4);

#ifndef _MCC88K		/* cannot divide by negative on 88k */

divReturn = div (-25, 4);
if (divReturn.quot != -6)	error (5);
if (divReturn.rem  != -1)	error (6);

divReturn = div (-4, 25);
if (divReturn.quot != 0)	error (7);
if (divReturn.rem  != -4)	error (8);

divReturn = div (-25, -4);
if (divReturn.quot !=  6)	error (9);
if (divReturn.rem  != -1)	error (10);

divReturn = div (-4, -25);
if (divReturn.quot != 0)	error (11);
if (divReturn.rem  != -4)	error (12);

#endif



ldivReturn = ldiv (25, 4);
if (ldivReturn.quot != 6)	error(21);
if (ldivReturn.rem  != 1)	error(22);

ldivReturn = ldiv (4, 25);
if (ldivReturn.quot != 0)	error(23);
if (ldivReturn.rem  != 4)	error(24);

#ifndef	_MCC88K

ldivReturn = ldiv (-25, 4);
if (ldivReturn.quot != -6)	error(25);
if (ldivReturn.rem  != -1)	error(26);

ldivReturn = ldiv (-4, 25);
if (ldivReturn.quot != 0)	error(27);
if (ldivReturn.rem  != -4)	error(28);

ldivReturn = ldiv (-25, -4);
if (ldivReturn.quot !=  6)	error(29);
if (ldivReturn.rem  != -1)	error(210);

ldivReturn = ldiv (-4, -25);
if (ldivReturn.quot != 0)	error(211);
if (ldivReturn.rem  != -4)	error(212);

#endif

if (errorCount)
	printf (ERRORS);
else
	printf (NO_ERRORS);
}

error (int err)
{
printf ("Error #%d occurred\n", err);
errorCount++;
}
 

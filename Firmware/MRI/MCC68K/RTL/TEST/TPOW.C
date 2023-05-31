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
/*	Quick test to check computation of pow (-1., 3.0);		*/
/*	This was a problem reported by sqa on one of our product lines	*/
/*									*/
/*	history:							*/
/*		1/12/93 jpc -	Added test for pow (2., 5.) to insure	*/
/*				exact computation.			*/
/************************************************************************/

/*	%W% %G%	*/

#include <math.h>
#include <stdio.h>
#include "message.h"

main ()
{
double temp;
int    errorCnt = 0;

    if ((temp = pow (-1., 3.0)) != -1.) {
	printf ("Bad value returned by pow (-1., 3.0) : %f\n", temp);
	errorCnt++;
	}

    if ((temp = pow (2., 5.)) != 32.) {
        printf ("Bad value returned by pow (2., 5.) : %f\n", temp);
	errorCnt++;
        }

    if (errorCnt) {
	printf (ERRORS);
	exit (1);
	} 
    else {
	printf (NO_ERRORS);
	exit (0);
	}
}
 

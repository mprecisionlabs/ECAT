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
/*	This routine is intended to test pre-increment, pre-decrement,	*/
/*	post-increment, post-decrement with floating point numbers, both*/
/*	floats and doubles.  Also tested is conversions from longs to 	*/
/*	floats and doubles, and visa versa.				*/
/*									*/
/*	In most cases, the above functionality is implemented with	*/
/*	interface routines.  So this routine is a fair test of those	*/
/*	interface routines.						*/
/*									*/
/*	02/12/91 jpc:	Written						*/
/************************************************************************/

/*	%W% %G%	*/

/*
Change history:
	12/10/93 jpc -	Modification to support -Kq
*/

#include <stdio.h>

#include "message.h"

double conDbl1 = 5.;
double conDbl2 = 10.;
double conDbl3 = 20.;
double conDbl4 = 40.;

static int toLongD (double x)
{
return x;
}

static int toLongF (float x)
{
return x;
}

main ()
{
    double testDouble;
    float  testFloat;
    int	   errorCount = 0;
    int	   y, yy;
    unsigned long uLong;
	     long Long;
    unsigned short uShort;
	     

    testDouble = 5.;
    testFloat  = 5.;

    if (testDouble++ != 5.) {
	printf ("Error: test 1 testDouble = %g\n", testDouble);
	errorCount++;
	}

    if (testDouble != 6.) {
	printf ("Error: test 2 testDouble = %g\n", testDouble);
	errorCount++;
	}

    if (testDouble-- != 6.) {
	printf ("Error: test 3 testDouble = %g\n", testDouble);
	errorCount++;
	}

    if (testDouble != 5.) {
	printf ("Error: test 4 testDouble = %g\n", testDouble);
	errorCount++;
	}

    if (++testDouble != 6.) {
	printf ("Error: test 5 testDouble = %g\n", testDouble);
	errorCount++;
	}

    if (--testDouble != 5.) {
	printf ("Error: test 6 testDouble = %g\n", testDouble);
	errorCount++;
	}

/****/

    if (testFloat++ != 5.) {
	printf ("Error: test 7 testFloat = %g\n", testFloat);
	errorCount++;
	}

    if (testFloat != 6.) {
	printf ("Error: test 8 testFloat = %g\n", testFloat);
	errorCount++;
	}

    if (testFloat-- != 6.) {
	printf ("Error: test 9 testFloat = %g\n", testFloat);
	errorCount++;
	}

    if (testFloat != 5.) {
	printf ("Error: test  10 testFloat = %g\n", testFloat);
	errorCount++;
	}

    if (++testFloat != 6.) {
	printf ("Error: test 11 testFloat = %g\n", testFloat);
	errorCount++;
	}

    if (--testFloat != 5.) {
	printf ("Error: test 12 testFloat = %g\n", testFloat);
	errorCount++;
	}

/*********/

    testDouble = 8.;
    y = testDouble;
    yy = toLongD (testDouble);
    if ((y != 8) || (yy != 8)) {
	printf ("Error: test 13 y = %d  yy = %d\n", y, yy);
	errorCount++;
	}
    
    testFloat = 8.;
    y = testFloat;
    yy = toLongF (testFloat);
    if ((y != 8) || (yy != 8)) {
	printf ("Error: test 14 y = %d  yy = %d\n", y, yy);
	errorCount++;
	}
    
/**********/

     testFloat = 8.;
     testDouble = testFloat;
     if (testDouble != 8.) {
	printf ("Error: test 15 testDouble = %g\n", testDouble);
	errorCount++;
	}

    testDouble = 8.;
    testFloat = testDouble;
    if (testFloat != 8.) {
	printf ("Error: test 16 testFloat = %g\n", testFloat);
	errorCount++;
	}

/**********/

    Long = 8;
    testDouble = Long;
    if (testDouble != 8.) {
	printf ("Error: test 17 testDouble = %g\n", testDouble);
	errorCount++;
	}

    Long = -8;
    testDouble = Long;
    if (testDouble != -8.) {
	printf ("Error: test 18 testDouble = %g\n", testDouble);
	errorCount++;
	}

    Long = 8;
    testFloat = Long;
    if (testFloat != 8.) {
	printf ("Error: test 19 testFloat = %g\n", testFloat);
	errorCount++;
	}

    Long = -8;
    testFloat = Long;
    if (testFloat != -8.) {
	printf ("Error: test 20 testFloat = %g\n", testFloat);
	errorCount++;
	}

/**********/

    uLong = 8;
    testDouble = uLong;
    if (testDouble != 8.) {
	printf ("Error: test 21 testDouble = %g\n", testDouble);
	errorCount++;
	}

#if __OPTION_VALUE ("-Kq")

    uShort = -8;
    testDouble = uShort;
    if (testDouble != 0xfff8) {
	printf ("Error: test 22 testDouble = %g\n", testDouble);
	errorCount++;
	}

#else

    uLong = -8;
    testDouble = uLong;
    if (testDouble != 0xfffffff8) {
	printf ("Error: test 22 testDouble = %g\n", testDouble);
	errorCount++;
	}

#endif

    uLong = 8;
    testFloat = uLong;
    if (testFloat != 8.) {
	printf ("Error: test 23 testFloat = %g\n", testFloat);
	errorCount++;
	}

if (((conDbl1 * conDbl2) * (conDbl4/conDbl3)) != 100.){
        printf ("Error: test 24\n");
        errorCount++;
        }

if ((((float)conDbl1 * (float)conDbl2) * ((float)conDbl4/(float) conDbl3))
        != (float) 100.){
        printf ("Error:  test 25\n");
        errorCount++;
        }

/*	The following test will not work correctly if there is double
	or extended prececision hardware in the circuit */

#if !_68040 && !_FPU && !_MCC88K && !_MCC29K
    uLong = -30001;
    testFloat = uLong;
    if (testFloat != (0xffffffff-30000)) {
	printf ("Error: test 26 testFloat = %g\n", testFloat);
	errorCount++;
	}
#endif

/**********/

    if (errorCount)
	printf (ERRORS);
    else
	printf (NO_ERRORS);
}
 

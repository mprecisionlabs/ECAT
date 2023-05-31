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
/*	This routine simply tests the integer limits in <limits.h> to	*/
/*	insure that they are correct.					*/
/************************************************************************/
/*	%W% %G%	*/

#include <stdio.h>
#include <limits.h>
#include "message.h"

int	errorCount;

main ()
{
if (sizeof (int) == 2) {
    if (INT_MIN != (-32768)) {
	printf ("Error:  INT_MIN incorrect; value is %ld\n", (long) INT_MIN);
	errorCount++;
	}
    if (INT_MAX != 32767) {
	printf ("Error:  INT_MAX incorrect; value is %ld\n", (long) INT_MAX);
	errorCount++;
	}
    if (UINT_MAX != 65535) {
	printf ("Error:  UINT_MAX incorrect; value is %ld\n",
		(unsigned long) UINT_MAX);
	errorCount++;
	}
    }
else {
    if (INT_MIN != (-2147483648)) {
	printf ("Error:  INT_MIN incorrect; value is %ld\n", (long) INT_MIN);
	errorCount++;
	}
    if (INT_MAX != 2147483647) {
	printf ("Error:  INT_MAX incorrect; value is %ld\n", (long) INT_MAX);
	errorCount++;
	}
    if (UINT_MAX != 4294967295) {
	printf ("Error:  UINT_MAX incorrect; value is %ld\n",
		(unsigned long) UINT_MAX);
	errorCount++;
	}
    }

if (SCHAR_MIN != (-128L)) {
    printf ("Error:  SCHAR_MIN incorrect; value is %ld\n", (long) SCHAR_MIN);
    errorCount++;
    }

if (SCHAR_MAX != (127L)) {
    printf ("Error:  SCHAR_MAX incorrect; value is %ld\n", (long) SCHAR_MAX);
    errorCount++;
    }

if (UCHAR_MAX != (255L)) {
    printf ("Error:  UCHAR_MAX incorrect; value is %ld\n", (long) UCHAR_MAX);
    errorCount++;
    }

#if _CHAR_SIGNED
    if (CHAR_MAX != (127L)) {
	printf ("Error:  CHAR_MAX incorrect; value is %ld\n",(long) CHAR_MAX);
	errorCount++;
	}

    if (CHAR_MIN != (-128L)) {
	printf ("Error:  CHAR_MIN incorrect; value is %ld\n",(long) CHAR_MIN);
	errorCount++;
	}
#else
    if (CHAR_MAX != (255L)) {
	printf ("Error:  CHAR_MAX incorrect; value is %ld\n",(long) CHAR_MAX);
	errorCount++;
	}

    if (CHAR_MIN != (0L)) {
	printf ("Error:  CHAR_MIN incorrect; value is %ld\n",(long) CHAR_MIN);
	errorCount++;
	}
#endif

if (SHRT_MIN != (-32768L)) {
    printf ("Error:  SHRT_MIN incorrect; value is %ld\n", (long) SHRT_MIN);
    errorCount++;
    }

if (SHRT_MAX != (32767L)) {
    printf ("Error:  SHRT_MAX incorrect; value is %ld\n", (long) SHRT_MAX);
    errorCount++;
    }

if (USHRT_MAX != (65535L)) {
    printf ("Error:  USHRT_MAX incorrect; value is %ld\n", (long) USHRT_MAX);
    errorCount++;
    }

if (LONG_MIN != (-2147483647-1)) {
    printf ("Error:  LONG_MIN incorrect; value is %ld\n", (long) LONG_MIN);
    errorCount++;
    }

if (LONG_MAX != (2147483647)) {
    printf ("Error:  LONG_MAX incorrect; value is %ld\n", (long) LONG_MAX);
    errorCount++;
    }

if (ULONG_MAX != (4294967295)) {
    printf ("Error:  ULONG_MAX incorrect; value is %ld\n", (long) ULONG_MAX);
    errorCount++;
    }

if (sizeof (SCHAR_MIN) != sizeof(int)) {
    printf ("Error:  SCHAR_MIN incorrect size; size is %d\n",sizeof(SCHAR_MIN));
    errorCount++;
    }

if (sizeof (SCHAR_MAX) != sizeof(int)) {
    printf ("Error:  SCHAR_MAX incorrect size; size is %d\n",sizeof(SCHAR_MAX));
    errorCount++;
    }

if (sizeof (UCHAR_MAX) != sizeof(int)) {
    printf ("Error:  UCHAR_MAX incorrect size; size is %d\n",sizeof(UCHAR_MAX));
    errorCount++;
    }

if (sizeof (CHAR_MIN) != sizeof(int)) {
    printf ("Error:  CHAR_MIN incorrect size; size is %d\n", sizeof(CHAR_MIN));
    errorCount++;
    }

if (sizeof (CHAR_MAX) != sizeof(int)) {
    printf ("Error:  CHAR_MAX incorrect size; size is %d\n", sizeof(CHAR_MAX));
    errorCount++;
    }

if (sizeof (SHRT_MIN) != sizeof(int)) {
    printf ("Error:  SHRT_MIN incorrect size; size is %d\n", sizeof(SHRT_MIN));
    errorCount++;
    }

if (sizeof (SHRT_MAX) != sizeof(int)) {
    printf ("Error:  SHRT_MAX incorrect size; size is %d\n", sizeof(SHRT_MAX));
    errorCount++;
    }

if (sizeof (USHRT_MAX) != sizeof(int)) {
    printf ("Error:  USHRT_MAX incorrect size; size is %d\n",sizeof(USHRT_MAX));
    errorCount++;
    }

if (sizeof (LONG_MIN) != sizeof(long)) {
    printf ("Error:  LONG_MIN incorrect size; size is %d\n", sizeof(LONG_MIN));
    errorCount++;
    }

if (sizeof (LONG_MAX) != sizeof(long)) {
    printf ("Error:  LONG_MAX incorrect size; size is %d\n", sizeof(LONG_MAX));
    errorCount++;
    }

if (sizeof (ULONG_MAX) != sizeof(long)) {
    printf ("Error:  ULONG_MAX incorrect size; size is %d\n",sizeof(ULONG_MAX));
    errorCount++;
    }

/******/

if (sizeof (INT_MIN) != sizeof(int)) {
    printf ("Error:  INT_MIN incorrect size; size is %d\n", sizeof(INT_MIN));
    errorCount++;
    }

if (sizeof (INT_MAX) != sizeof(int)) {
    printf ("Error:  INT_MAX incorrect size; size is %d\n", sizeof(INT_MAX));
    errorCount++;
    }

if (sizeof (UINT_MAX) != sizeof(int)) {
    printf ("Error:  UINT_MAX incorrect size; size is %d\n", sizeof(UINT_MAX));
    errorCount++;
    }

if (errorCount) 
    printf (ERRORS);
else
    printf (NO_ERRORS);
}
 

#include "lib_top.h"


#if EXCLUDE_m_nint || _80960SB || _80960KB || _80960MC || (_MCCG32 && _FPU) \
	|| (_MCC86 && (! _FPU)) \
	|| (_MCC68K && _FPU) || (__OPTION_VALUE ("-Kq"))
    #pragma option -Qws			/* Prevent compiler warning */
#else

/*
 * COPYRIGHT  Hewlett-Packard Company  1987, 1988
 */

/************************************************************************/
/* THIS INFORMATION IS PROPRIETARY TO					*/
/* MICROTEC RESEARCH, INC.						*/
/*----------------------------------------------------------------------*/
/* Copyright (c) 1991, 1992 Microtec Research, Inc.                     */
/* All rights reserved							*/
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

#include "m_incl.h"
#include "op_reent.h"

 /*--------------------------------------------------------------------
     AUTHOR: Michael Fluegge		05/10/88

     This file contains the routines ceil() and floor().  ceil() returns
   the smallest integer which is greater than or equal to the argument.
   The returned value is in the form of a double.  floor() returns the
   largest integer which is less than or equal to the argument.
     Since the code is so similar for both of these routines, they both
   call one routine to do the work.  This routine is NearInt().  This
   routine returns an integer that is near to its argument.  The particular
   integer returned is determined by its flag argument.  The flag indicates
   whether to choose the integer nearer zero or farther from zero.	*/



/*--  NearInt()  ----------------------------------------------------
    If the argument is an integer then return it, else return a near
 integer.  If the flag 'AwayFrom0' is set then the integer which is
 closest to the argument yet farther from ZERO is returned (i.e. the
 smallest integer with a larger magnitude than the magnitude of the
 argument).  If the flag is clear, then the largest integer whose
 magnitude is smaller than the magnitude of the argument is returned.	*/
/*
Change history:
	11/03/93 - jpc  Converted "INTEL" and "MOTOROLA" to "_LITTLE_ENDIAN"
			and _BIG_ENDIAN, respectively; removed special handling
			of NaN's for little-endian machines -- all NaN's are
			handled the same now.
*/

static double
NearInt(arg, AwayFrom0)
double  arg;
int	AwayFrom0;
{
    STATIC realnumber		r, hold;
    STATIC register int	shftcnt;

    r.value = arg;

 /* Get a feel for the size of the number.  1075 == EXP_BIAS + # sig bits */
    shftcnt = 1075 - ((r.half.hi >> 20) & 0x7FF);  /* # bits to mask */

    if (shftcnt > 52) { 	/* Is magnitude less than 1.0? */
	if (AwayFrom0 && isNonZero(r))
	    return( (isNEG(r)) ? -ONE : ONE );

	r.half.hi &= 0x80000000;	/* Return signed zero */
	r.half.lo  = 0x0;
	return(r.value);
    }

    if (shftcnt < 1) { 		/* Is it huge?  No binary point in mantissa? */
	return( (isNAN(r)) ? _fp_error((UInt32) SIGNAN, r.value) : r.value );
    }


 /* 'shftcnt' tells us where the first fractional bit is located */

    hold.value = r.value;	/* Save the value for latter */
    if (shftcnt >= 32) {	/* binary point in upper 32 bits? */
	r.half.hi &= ALLONES << (shftcnt -= 32);   /* Discard fractional bits */
	r.half.lo = 0x0;

	if (AwayFrom0 && (hold.value != r.value))	/* Was arg a non-int? */
	    r.half.hi += (UInt32) 0x1 << shftcnt;	/* Add 1.0 */
    }
    else {			/* binary point in lower 32 bits */
	r.half.lo &= ALLONES << shftcnt;	/* Discard fractional bits */

	if (AwayFrom0 && hold.half.lo != r.half.lo) {	/* Was arg a non-int? */
	    register UInt32  temp;		/* Save value for carry test */

	    temp = r.half.lo;
	    r.half.lo += (UInt32) 0x1 << shftcnt;	/* Add 1.0 */
	    if (r.half.lo <= temp)			/* Carry? */
		++r.half.hi;
	}
    }

    return(r.value);
}  /* NearInt() */



double ceil (double arg)
{
    STATIC realnumber  r;

    r.value = arg;
    if (isNAN(r)) {
	return( _fp_error((UInt32) (FP_ceil | SIGNAN), r.value) );
    }

 /* A positive argument means to move away from zero */
    return( NearInt(r.value, (r.half.hi & 0x80000000) == 0x0) );
}


double floor (double arg)
{
    STATIC realnumber  r;

    r.value = arg;
    if (isNAN(r)) {
	return( _fp_error((UInt32) (FP_floor | SIGNAN), r.value) );
    }

 /* A negative argument means to move away from zero */
    return( NearInt(r.value, (r.half.hi & 0x80000000) != 0x0) );
}  /* floor() */

#endif /* EXCLUDE_m_nint */


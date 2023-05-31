#include "lib_top.h"


#if EXCLUDE_m_exp || _80960SB || _80960KB || _80960MC || (_MCCG32 && _FPU) \
	|| (_MCC68K && _FPU) \
	|| (_MCC86 && (! _FPU)) \
	|| (__OPTION_VALUE ("-Kq"))
    #pragma option -Qws			/* Prevent compiler warning */
#else

/*
 * @(SUBID) 05/10/88 /src2/nls/libraries/libm/s.exp.c 1.17
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


/*	%W% %G% */

/*
    DOUBLE PRECISION EXP

    Author: Mark McDowell
    Date:   November 27, 1984
    Source: Software Manual for the Elementary Functions
	by William James Cody, Jr. and William Waite (1980)

    This routine will work only for numbers in IEEE double precision
    floating point format. Of the numbers in this format, it checks for 
    and handles only normalized numbers and -0 and +0 -- not infinities, 
    denormalized numbers, nor NaN's.

*/
 /*--------------------------------------------------------------------
     Revised by Michael Fluegge 	05/10/88

       The code has been changed to follow more of the IEEE standards.
     Arguments of all types can now be accepted and will produce appropriate
     results.  No special code is required for denormalized numbers since
     they fall into the range of numbers which will return a value of 1.0
  */
/*
Change history:
	11/03/93 - jpc  Converted "INTEL" and "MOTOROLA" to "_LITTLE_ENDIAN"
			and _BIG_ENDIAN, respectively; removed special handling
			of NaN's for little-endian machines -- all NaN's are
			handled the same now.
*/

#if CPLUSPLUS_NAME_CONVERT
    #include "m_nf_fix.h"
#endif

#include "m_incl.h"
#include "op_reent.h"

#define MAX_EXP_ARG   (*((const double *) hex_max_exp_arg))
#define MIN_EXP_ARG   (*((const double *) hex_min_exp_arg))
#define EXP_EPS       (*((const double *) hex_exp_eps))
#define ONE_OVER_LN2  (*((const double *) hex_one_over_ln2))
#define LN2_OVER_TWO  (*((const double *) hex_ln2_over_two))
#define C1            (*((const double *) hex_c1))
#define C2            (*((const double *) hex_c2))
#define P0            (*((const double *) hex_p0))
#define P1            (*((const double *) hex_p1))
#define P2            (*((const double *) hex_p2))
#define Q0            (*((const double *) hex_q0))
#define Q1            (*((const double *) hex_q1))
#define Q2            (*((const double *) hex_q2))
#define Q3            (*((const double *) hex_q3))

static const UInt32
	hex_one_over_ln2[2] = HD_INIT(0x3FF71547,0x652B82FE),
	hex_ln2_over_two[2] = HD_INIT(0x3FD62E42,0xFEFA39EF),
	hex_max_exp_arg[2] =  HD_INIT(0x40862E42,0xFEFA39EE),
	hex_min_exp_arg[2] =  HD_INIT(0xC0874385,0x446D71C3),
	hex_exp_eps[2] = HD_INIT(0x3C900000,0x0),	/* 2**-54 */
	hex_p0[2] = HD_INIT(0x3FD00000,0x00000000),	/* .25 */
	hex_p1[2] = HD_INIT(0x3F7F074B,0xF22A12A6),	/* .00757532 */
	hex_p2[2] = HD_INIT(0x3F008B44,0x2AE6921E),	/* 3.15552e-5 */
	hex_q0[2] = HD_INIT(0x3FE00000,0x00000000),	/* .5 */
	hex_q1[2] = HD_INIT(0x3FAD1728,0x51DFD9FF),	/* .0568173 */
	hex_q2[2] = HD_INIT(0x3F44AF0C,0x5C28D4DF),	/* .000631219 */
	hex_q3[2] = HD_INIT(0x3EA93363,0x0CE50455),	/* 7.5104e-7 */
	hex_c1[2] = HD_INIT(0x3FE63000,0x00000000),	/* .693359 */
	hex_c2[2] = HD_INIT(0xBF2BD010,0x5C610CA8);	/* -0.000212194 */


double
exp(arg)
double	arg;
{
    STATIC register long    n;
    STATIC register UInt32  argsign;
    STATIC realnumber       t1, t2, g;
    STATIC double           z;


    t1.value = arg;

 /* Is the argument an NaN or an infinity? */
    if (ExpAll1sp(t1)) {
	if (SigNonZerop(t1)) {		/* NaN? */
	    return _fp_error((UInt32) (FP_exp | SIGNAN), t1.value);
	}
        else if (isNEG(t1)) 		/* Is the argument neg. inf.? */
 /* exp(+inf.) == +inf.;   exp(-inf.) == 0.0 */
	    return ZERO;

	return( _fp_error((UInt32) (FP_exp | OVERFLOW)) );	/* + inf */
    }

 /* Is the argument out of range? */
    if (t1.value > MAX_EXP_ARG)
	return( _fp_error((UInt32) (FP_exp | OVERFLOW)) );

    if (t1.value < MIN_EXP_ARG)
	return( _fp_error((UInt32) (FP_exp | UNDERFLOW), 100, ZERO, 1) );
		/*	Set the shift_count value to 100 so that there is
		    no doubt the value underflows to 0.0.  The round/sticky
		    variable is non-zero so that the resulting value will
		    be taken to be inexact.
		*/


    argsign = t1.half.hi & 0x80000000;	/* save sign of argument */
    t1.half.hi ^= argsign;		/* t1 = abs(arg) */

    if (t1.value < EXP_EPS) 
	return(ONE);		    /* for small values, answer is 1 */

 /* Check to see if we can save ourselves from doing some computations */

    if (t1.value > LN2_OVER_TWO) {	/* argument reduction needed? */
					/* yes: n = INTRND(t1 / ln(2)) */
	n = (long) (t1.value * ONE_OVER_LN2 + HALF);
	t2.value = (double) n;		/* t2 = FLOAT(n) */
	g.value = (t1.value - t2.value * C1) - t2.value * C2;	/* reduce */

 /*    Repair the sign since the computations have been done using
  *  the absolute value of the operand.  */

	g.half.hi ^= argsign;		/* adjust sign appropriately */
	if (argsign)
	    n = -n;		/* adjust multiple's sign */
    }
    else {			/* no reduction needed */
	g.value = arg;
	n = 0;			/* divides by log(2) zero times */
    }

    z = g.value * g.value;		/* calculate exp */
    t1.value = ((P2 * z + P1) * z + P0) * g.value;
    t2.value = ((Q3 * z + Q2) * z + Q1) * z + Q0;
    t1.value = HALF + (t1.value / (t2.value - t1.value));

 /* Compute the exponent of the answer */
    n += 1 + ((t1.half.hi & 0x7FF00000) >> 20);
    t1.half.hi &= 0x000FFFFF;		/* Discard all but the sig bits */

    if (n > 0) {		/* Is the result a normal number? */
	t1.half.hi |= n << 20;
	return(t1.value);
    }
    else			/* A denormal must be returned */
	return(
	    _fp_error((UInt32) (FP_exp | UNDERFLOW), (int) (1 - n), t1.value, 0)
	);
}  /* exp() */

#endif /* EXCLUDE_m_exp */


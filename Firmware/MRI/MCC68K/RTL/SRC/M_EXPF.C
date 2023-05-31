#include "lib_top.h"


#if EXCLUDE_m_expf || _80960SB || _80960KB || _80960MC || (_MCCG32 && _FPU) \
	|| (_MCC68K && _FPU) \
	|| (_MCC86 && (! _FPU)) \
	|| (!__OPTION_VALUE ("-Kq"))
    #pragma option -Qws			/* Prevent compiler warning */
#else
 
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


/* %W% %G% */

/*
    SINGLE PRECISION EXP

    Author: Mark McDowell
    Date:   November 27, 1984
    Source: Software Manual for the Elementary Functions
	by William James Cody, Jr. and William Waite (1980)

    This routine will work only for numbers in IEEE single precision
    floating point format. Of the numbers in this format, it checks for 
    and handles only normalized numbers and -0 and +0 -- not infinities, 
    denormalized numbers, nor NaN's.

*/
 /*--------------------------------------------------------------------
     Revised by Michael Fluegge 	07/13/87
   
       The code has been changed to follow more of the IEEE standards.
     Arguments of all types can now be accepted and will produce appropriate
     results.  No special code is required for denormalized numbers since
     they fall into the range of numbers which will return a value of 1.0
  */

/*
NOTE:
	This routine is designed to be used only when the -Kq switch is turned
on. When this is done, doubles are treated as though they are only 32-bit long. 
And as such, floats and doubles are the same.
*/

/*
Change history:
	11/03/93 - jpc  Converted "INTEL" and "MOTOROLA" to "_LITTLE_ENDIAN"
			and _BIG_ENDIAN, respectively; removed special handling
			of NaN's for little-endian machines -- all NaN's are
			handled the same now.
	11/12/93 - jpc	Converted to function with -Kq switch
			floats converted to doubles
*/

#include "m_incl.h"
#include "op_reent.h"

#define EXP_EPS         0x33000000
#define LN2_OVER_TWO    0x3EB17218
#define	MAX_EXP_ARG	(*((const double *) &hex_max_exp_arg))
#define MIN_EXP_ARG	(*((const double *) &hex_min_exp_arg))
#define ONE_OVER_LN2    (*((const double *) &hex_one_over_ln2))
#define P0          	(*((const double *) &hex_p0))
#define P1          	(*((const double *) &hex_p1))
#define Q0          	(*((const double *) &hex_q0))
#define Q1          	(*((const double *) &hex_q1))
#define LN2         	(*((const double *) &hex_ln2))

static const UInt32	hex_max_exp_arg =  0x42B17218,
			hex_min_exp_arg =  0xC2CE8ECF,
			hex_one_over_ln2 = 0x3FB8AA3B,
			hex_p0 =           0x3E800000,
			hex_p1 =           0x3B885308,
			hex_q0 =           0x3F000000,
			hex_q1 =           0x3D4CBF5B,
			hex_ln2=	   0x3f317218;


double exp(double arg)
{
    STATIC register long     n;
    STATIC register UInt32   argsign;
    STATIC realnumberF       t1, t2, g;
    STATIC double             z;

    t1.value = arg;

 /* Is the argument an NaN or an infinity? */
    if (ExpAll1sFp(t1)) {
	if (SigNonZeroFp(t1))		/* NaN? */
	    return _fp_errorf((UInt32) (FP_expf | SIGNAN), t1.ivalue);
        else if (isNEGf(t1) && SigZeroFp(t1))	/* Is the argument neg. inf.? */
 /* exp(+inf.) == +inf.;   exp(-inf.) == 0.0 */
	    return ZEROf;

	return _fp_errorf((UInt32) (FP_expf | OVERFLOW));	/* + inf */
    }

 /* Is the argument out of range? */
    if (t1.value > MAX_EXP_ARG)		/* Overflow? */
	return( _fp_errorf((UInt32) (FP_expf | OVERFLOW)) );
	     
    if (t1.value < MIN_EXP_ARG)		/* Underflow? */
	return( _fp_errorf((UInt32) (FP_expf | UNDERFLOW),
				100, t1.ivalue = 0, 1) );
		/*	Set the shift_count value to 100 so that there is
		    no doubt the value underflows to 0.0.  The round/sticky
		    variable is non-zero so that the resulting value will
		    be taken to be inexact.
		*/


    argsign = t1.ivalue & 0x80000000;	/* save sign of argument */
    t1.ivalue ^= argsign;		/* t1 = abs(arg) */

 /* For small values, answer is 1.0 */
    if (t1.ivalue < EXP_EPS)
   	return(ONEf);

 /* Check to see if we can save ourselves from doing some computations */

    if (t1.ivalue > LN2_OVER_TWO) {     /* argument reduction needed? */
    					/* yes: n = INTRND(t1 / ln(2)) */
	n = (long) (t1.value * ONE_OVER_LN2 + HALFf);
	g.value = t1.value - n * LN2;	/* reduce */

 /*    Repair the sign since the computations have been done using
  *  the absolute value of the operand.		*/

	g.ivalue ^= argsign;        /* adjust sign appropriately */
	if (argsign) n = -n;        /* adjust multiple's sign */
    }
    else {                  /* no reduction needed */
	g.value = arg;
	n = 0;              /* divides by log(2) zero times */
    }

    z = g.value * g.value;		/* calculate exp */
    t1.value = (P1 * z + P0) * g.value;
    t2.value = Q1 * z + Q0;
    t1.value = HALFf + (t1.value / (t2.value - t1.value));

 /* Compute the exponent of the answer */
    n += 1 + ((t1.ivalue & 0x7F800000) >> 23);
    t1.ivalue &= 0x007FFFFF;		/* Discard all but the sig bits */

    if (n > 0) {		/* Result is a normal number? */
	t1.ivalue |= n << 23;
	return(t1.value);
    }
    else			/* A denormal must be returned */
	return( _fp_errorf((UInt32) (FP_expf | UNDERFLOW),
					1 - n, t1.ivalue, 0) );
}  /* expf() */
 
#endif /* EXCLUDE_m_expf */
 

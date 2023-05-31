#include "lib_top.h"


#if EXCLUDE_m_tan || _80960SB || _80960KB || _80960MC || (_MCCG32 && _FPU) \
	|| (_MCC68K && _FPU) \
	|| (_MCC86 && (! _FPU)) \
	|| (__OPTION_VALUE ("-Kq"))
    #pragma option -Qws			/* Prevent compiler warning */
#else

/*
 * @(SUBID) 05/10/88 /src2/nls/libraries/libm/s.tan.c 1.12
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

/*
    DOUBLE PRECISION TANGENT

    Author: Mark McDowell
    Date:   November 27, 1984
    Source: Software Manual for the Elementary Functions
	by William James Cody, Jr. and William Waite (1980)

    This routine will work only for numbers in IEEE double precision
    floating point format. Of the numbers in this format, it checks for 
    and handles only normalized numbers and -0 and +0 -- not infinities, 
    denormalized numbers, nor NaN's.

*/
/*---------------------------------------------------------------------
    Revised by Michael Fluegge		05/10/88

      All of the IEEE numbers are now handled.  NaNs are returned.
    Infinities cause an exception and an NaN is returned.  Denormals 
    will simply be returned after going through much of the 
    computation.	*/
/*
Change history:
	11/03/93 - jpc  Converted "INTEL" and "MOTOROLA" to "_LITTLE_ENDIAN"
			and _BIG_ENDIAN, respectively; removed special handling
			of NaN's for little-endian machines -- all NaN's are
			handled the same now.
*/

#include "m_incl.h"
#include "op_reent.h"

#define TAN_MAX      149078413.0	/* ~= INT(2**(53/2) * PI/2) */
#define TAN_EPS      (*((const double *) hex_tan_eps))
#define TWO_OVER_PI  (*((const double *) hex_two_over_pi))
#define PI_OVER_4    (*((const double *) hex_pi_over_4))
#define C1           (*((const double *) hex_c1))
#define C2           (*((const double *) hex_c2))
#define P1           (*((const double *) hex_p1))
#define P2           (*((const double *) hex_p2))
#define P3           (*((const double *) hex_p3))
#define Q1           (*((const double *) hex_q1))
#define Q2           (*((const double *) hex_q2))
#define Q3           (*((const double *) hex_q3))
#define Q4           (*((const double *) hex_q4))

static const UInt32
	hex_two_over_pi[2] = HD_INIT(0x3FE45F30,0x6DC9C883),
	hex_pi_over_4[2] = HD_INIT(0x3FE921FB,0x54442D18),
	hex_tan_eps[2] = HD_INIT(0x3E46A09E,0x667F3BCD),	/* 2**(53/2) */
	hex_c1[2] = HD_INIT(0x3FF92200,0x00000000),	/* 3217/2048 */
	hex_c2[2] = HD_INIT(0xBED2AEEF,0x4B9EE59E),
	hex_p1[2] = HD_INIT(0xBFC112B5,0xE54D0900),
	hex_p2[2] = HD_INIT(0x3F6C0E82,0xA63BAADF),
	hex_p3[2] = HD_INIT(0xBEF2BAB7,0x2EA2C724),
	hex_q1[2] = HD_INIT(0xBFDDDEB0,0x47FBD9D5),
	hex_q2[2] = HD_INIT(0x3F9A479E,0xA17E2159),
	hex_q3[2] = HD_INIT(0xBF346F64,0x99094841),
	hex_q4[2] = HD_INIT(0x3EA0B774,0xF07678E9);


double
tan(arg)
double	arg;
{
    STATIC realnumber		f, t1, t2;
    STATIC double		g;
    STATIC register UInt32	argsign;
    STATIC register int	odd_multiple;
    STATIC register long	multiple;


    t1.value = arg;

    if (isNAN(t1)) {
	return( _fp_error((UInt32) (FP_tan | SIGNAN), t1.value) );
    }

    argsign = t1.half.hi & 0x80000000;	/* argsign = sign of arg */
    t1.half.hi ^= argsign;		/* t1 = abs(arg) */

 /* Is the argument an infinity? */
    if (isINFINITY(t1))
	return( _fp_error((UInt32) (FP_tan | OPERROR)) );

 /* Is the loss of precision too bad? */
    if (t1.value > TAN_MAX)
	return( _fp_error((UInt32) (FP_tan | PLOSS)) );


    if (t1.value > PI_OVER_4) {         /* does argument need reduction? */
 /* multiple = INTRND(arg * 2/PI); 'multiple' is the number of PI/2's in arg */
	odd_multiple = 1 & (multiple = (long) (t1.value * TWO_OVER_PI + HALF));
	t2.value = (double) multiple;	/* t2 <- multiple */
	f.value = (t1.value - t2.value * C1) - t2.value * C2;	/* reduce */
	f.half.hi ^= argsign;           /* give it the correct sign */
	t1.value = f.value;
	t1.half.hi &= 0x7FFFFFFF;       /* t1 <- abs(reduced arg) */
    } 
    else {
	f.value = arg;		/* no reduction needed */
	odd_multiple = 0;	/* even multiple of PI/4 */
    }

    if (t1.value < TAN_EPS)		/* reduced argument significant? */
	return( (odd_multiple) ? (-ONE / f.value) : f.value );
    else {				/* calculate tangent */
	g = f.value * f.value;		/* g is the square of f */

 /* Compute the numerator and denominator of the result */
	t1.value = ((P3 * g + P2) * g + P1) * g * f.value + f.value;
	t2.value = (((Q4 * g + Q3) * g + Q2) * g + Q1) * g + ONE;

	if (odd_multiple) {		/* odd multiple of PI/4? */
	    t2.half.hi ^= 0x80000000;		/* Negate the result */
	    return(t2.value / t1.value);
	}
	else 
	    return(t1.value / t2.value);
    }
}  /* tan() */

#endif /* EXCLUDE_m_tan */


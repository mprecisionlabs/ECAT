#include "lib_top.h"


#if EXCLUDE_m_sinh || _80960SB || _80960KB || _80960MC || (_MCCG32 && _FPU) \
	|| (_MCC68K && _FPU) \
	|| (_MCC86 && (! _FPU)) \
	|| (__OPTION_VALUE ("-Kq"))
    #pragma option -Qws			/* Prevent compiler warning */
#else

/*
 * @(SUBID) 05/10/88 /src2/nls/libraries/libm/s.sinh.c 1.12
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
    DOUBLE PRECISION HYPERBOLIC SINE

    Author: Mark McDowell
    Date:   December 11, 1984
    Source: Software Manual for the Elementary Functions
	by William James Cody, Jr. and William Waite (1980)

    This routine will work only for numbers in IEEE double precision
    floating point format. Of the numbers in this format, it checks for 
    and handles only normalized numbers and -0 and +0 -- not infinities, 
    denormalized numbers, nor NaN's.

*/
/*--------------------------------------------------------------------
    Revised by Michael Fluegge		05/10/88

      The routine now handles all of the IEEE standard floating point
    numbers.  NaNs and infinities are simply returned.  There is no 
    special processing needed for the denormals.		*/
/*
Change history:
	11/03/93 - jpc  Converted "INTEL" and "MOTOROLA" to "_LITTLE_ENDIAN"
			and _BIG_ENDIAN, respectively; removed special handling
			of NaN's for little-endian machines -- all NaN's are
			handled the same now.
*/

#include "m_incl.h"
#include "op_reent.h"

#define SINH_EPS    (*((const double *) hex_sinh_eps))
#define P0          (*((const double *) hex_p0))
#define P1          (*((const double *) hex_p1))
#define P2          (*((const double *) hex_p2))
#define P3          (*((const double *) hex_p3))
#define Q0          (*((const double *) hex_q0))
#define Q1          (*((const double *) hex_q1))
#define Q2          (*((const double *) hex_q2))
#define YBAR        (*((const double *) hex_ybar))
#define LNV         (*((const double *) hex_lnv))
#define V2M1        (*((const double *) hex_v2m1))
#define WMAX        (*((const double *) hex_wmax))

static const UInt32
 /* ybar is the largest number s.t. exp(ybar) doesn't overflow and
            1.0/exp(ybar) doesn't underflow.		*/
	hex_ybar[2]     = HD_INIT(0x4086232B,0xDD7ABCD2),
	hex_sinh_eps[2] = HD_INIT(0x3E46A09E,0x667F3BCD),	/* 2**(53/2) */
	hex_p0[2]       = HD_INIT(0xC1157913,0x56533419),
	hex_p1[2]       = HD_INIT(0xC0C695C2,0xB6941490),
	hex_p2[2]       = HD_INIT(0xC0647841,0x6385BE4A),
	hex_p3[2]       = HD_INIT(0xBFE944E7,0xB86FC81B),
	hex_q0[2]       = HD_INIT(0xC1401ACE,0x80BE6713),
	hex_q1[2]       = HD_INIT(0x40E1A857,0x23B65EC7),
	hex_q2[2]       = HD_INIT(0xC0715BC3,0x81C97FF2),
	hex_lnv[2]      = HD_INIT(0x3FE62E60,0x00000000),
	hex_v2m1[2]     = HD_INIT(0x3EED0112,0xEB0202D6),
	hex_wmax[2]     = HD_INIT(0x40862E3C,0x85B28BDB);


double
sinh(arg) 
double	arg; 
{
    STATIC realnumber       y, f;
    STATIC register UInt32  argsign;

    y.value = arg;

    if (ExpAll1sp(y)) {			/* NaN or infinity? */
	if (isNAN(y)) {
	    return( _fp_error((UInt32) (FP_sinh | SIGNAN), y.value) );
	}
	return _fp_error((UInt32) (FP_sinh | OVERFLOW));
    }

    argsign = y.half.hi & 0x80000000;	/* save sign */
    y.half.hi ^= argsign;               /* y = abs(arg) */

    if (y.value <= ONE) {
	if (y.value < SINH_EPS)
	    return(arg);		/* sinh(x) = x for small x */

	f.value = arg * arg;            /* use polynomial for sinh */
	return(
	    arg + arg * 
	      (f.value * ( 		/* R(x^2) */
							/* P(x^2) */
		(((P3 * f.value + P2) * f.value + P1) * f.value + P0)
		/					/* Q(x^2) */
		(((f.value + Q2) * f.value + Q1) * f.value + Q0)
	      ))
	);
    }

    if (y.value <= YBAR) {
	f.value = exp(y.value);         /* sinh(y) = (exp(y)-1/exp(y))/2 */
	f.value -= ONE / f.value;
	f.half.hi -= 0x100000;		/* Multiply f by 0.5 */
	f.half.hi ^= argsign;           /* give proper sign */
	return(f.value);
    }

    y.value -= LNV;			/* y = y - (~= log(2)) */

    if (y.value > WMAX) {               /* argument too big? */
	y.value = _fp_error((UInt32) (FP_sinh | OVERFLOW));
	y.half.hi |= argsign;
	return(y.value);
    }

    f.value = exp(y.value);             /* calculate sinh near MAX */
    f.value += V2M1 * f.value;
    f.half.hi ^= argsign;               /* replace sign */
    return(f.value);
}  /* sinh() */

#endif /* EXCLUDE_m_sinh */


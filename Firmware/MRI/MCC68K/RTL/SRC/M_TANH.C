#include "lib_top.h"


#if EXCLUDE_m_tanh || _80960SB || _80960KB || _80960MC || (_MCCG32 && _FPU) \
	|| (_MCC68K && _FPU) \
	|| (_MCC86 && (! _FPU)) \
	|| (__OPTION_VALUE ("-Kq"))
    #pragma option -Qws			/* Prevent compiler warning */
#else

/*
 * @(SUBID) 05/10/88 /src2/nls/libraries/libm/s.tanh.c 1.13
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
    DOUBLE PRECISION HYPERBOLIC TANGENT

    Author: Mark McDowell
    Date:   December 10, 1984
    Source: Software Manual for the Elementary Functions
	by William James Cody, Jr. and William Waite (1980)

    This routine will work only for numbers in IEEE double precision
    floating point format. Of the numbers in this format, it checks for 
    and handles only normalized numbers and -0 and +0 -- not infinities, 
    denormalized numbers, nor NaN's.

*/
/*---------------------------------------------------------------------
    Revised by Michael Fluegge		05/10/88

      This routine required virtually zero changes in order to handle
    all of the IEEE floating-point number types.  The only change
    needed was a test for an NaN.		*/
/*
Change history:
	11/03/93 - jpc  Converted "INTEL" and "MOTOROLA" to "_LITTLE_ENDIAN"
			and _BIG_ENDIAN, respectively; removed special handling
			of NaN's for little-endian machines -- all NaN's are
			handled the same now.
*/

#include "m_incl.h"
#include "op_reent.h"


#define XBIG        (*((const double *) hex_xbig))
#define LOG3_OVER_2 (*((const double *) hex_log3_over_2))
#define TANH_EPS    (*((const double *) hex_tanh_eps))
#define P0          (*((const double *) hex_p0))
#define P1          (*((const double *) hex_p1))
#define P2          (*((const double *) hex_p2))
#define Q0          (*((const double *) hex_q0))
#define Q1          (*((const double *) hex_q1))
#define Q2          (*((const double *) hex_q2))

static const UInt32	hex_xbig[2] =        HD_INIT(0x40330FC1,0x931F09CA),
			hex_log3_over_2[2] = HD_INIT(0x3FE193EA,0x7AAD030B),
			hex_tanh_eps[2] =    HD_INIT(0x3E46A09E,0x667F3BCD),
			hex_p0[2] =          HD_INIT(0xC09935A5,0xC9BE1E18),
			hex_p1[2] =          HD_INIT(0xC058CE75,0xA1BA5CCC),
			hex_p2[2] =          HD_INIT(0xBFEEDC28,0xCEFBA77F),
			hex_q0[2] =          HD_INIT(0x40B2E83C,0x574E9693),
			hex_q1[2] =          HD_INIT(0x40A1738B,0x4D01F0F3),
			hex_q2[2] =          HD_INIT(0x405C2FA9,0xE1EBF7FA);


double
tanh(arg) 
double	arg;
{
    STATIC realnumber       f, g;
    STATIC register UInt32  argsign;


    f.value = arg;

    if (isNAN(f)) {
	return( _fp_error((UInt32) (FP_tanh | SIGNAN), f.value) );
    }

    argsign = f.half.hi & 0x80000000;	/* save sign */
    f.half.hi ^= argsign;               /* f = abs(arg) */

    if (f.value > XBIG) {               /* tanh of large value is +1 or -1 */
	f.value = ONE;
	f.half.hi |= argsign;           /* give proper sign */
	return(f.value);
    }

    if (f.value > LOG3_OVER_2) {	/* compare with log(3)/2 */
	f.half.hi += 0x100000;          /* f *= 2 */
	f.value = ONE / (exp(f.value) + ONE);
	f.value = HALF - f.value;
	f.half.hi += 0x100000;		/* f *= 2 */
	f.half.hi ^= argsign;           /* give proper sign */
	return(f.value);
    }

    if (f.value < TANH_EPS)
    	return(arg);		/* tanh(x) = x for small x */

    g.value = f.value * f.value;	/* calculate tanh */
    f.value += f.value * (
   /* R(g) */	(((P2 * g.value + P1) * g.value + P0) * g.value)   /* g*P(g) */
		/
		(((g.value + Q2) * g.value + Q1) * g.value + Q0)   /* Q(g) */
        );

    f.half.hi ^= argsign;		/* replace the sign */

    return(f.value);
}  /* tanh() */

#endif /* EXCLUDE_m_tanh */


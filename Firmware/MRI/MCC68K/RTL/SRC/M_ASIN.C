#include "lib_top.h"


#if EXCLUDE_m_asin || _80960SB || _80960KB || _80960MC || (_MCCG32 && _FPU) \
	|| (_MCC68K && _FPU) \
	|| (_MCC86 && (! _FPU)) \
	|| (__OPTION_VALUE ("-Kq"))
    #pragma option -Qws			/* Prevent compiler warning */
#else

/*
 * @(SUBID) 05/10/88 /src2/nls/libraries/libm/s.asin.c 1.16
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
    DOUBLE PRECISION ARCSINE

    Author: Mark McDowell
    Date:   December 13, 1984
    Source: Software Manual for the Elementary Functions
	by William James Cody, Jr. and William Waite (1980)

    This routine will work only for numbers in IEEE double precision
    floating point format. Of the numbers in this format, it checks for 
    and handles only normalized numbers and -0 and +0 -- not infinities, 
    denormalized numbers, nor NaN's.

*/
/*---------------------------------------------------------------------
    Revised by Michael Fluegge		05/10/88

      The changes for this routine were minimal.  More comments were
    added, the layout of the routine was changed to be consistent and
    a check was made for NaNs as arguments.  No special processing is
    needed for infinities or denormals.		*/
/*
Change history:
	11/03/93 - jpc  Converted "INTEL" and "MOTOROLA" to "_LITTLE_ENDIAN"
			and _BIG_ENDIAN, respectively; removed special handling
			of NaN's for little-endian machines -- all NaN's are
			handled the same now.
*/

#include "m_incl.h"
#include "op_reent.h"

#define ASIN_EPS   (*((const double *) hex_asin_eps))
#define PI_OVER_4  (*((const double *) hex_pi_over_4))
#define P1         (*((const double *) hex_p1))
#define P2         (*((const double *) hex_p2))
#define P3         (*((const double *) hex_p3))
#define P4         (*((const double *) hex_p4))
#define P5         (*((const double *) hex_p5))
#define Q0         (*((const double *) hex_q0))
#define Q1         (*((const double *) hex_q1))
#define Q2         (*((const double *) hex_q2))
#define Q3         (*((const double *) hex_q3))
#define Q4         (*((const double *) hex_q4))

static const UInt32
	hex_asin_eps[2] =  HD_INIT(0x3E46A09E,0x667F3BCD), /* 2**(53/2) */
	hex_pi_over_4[2] = HD_INIT(0x3FE921FB,0x54442D18),
	hex_p1[2] =        HD_INIT(0xC03B5E55,0xA83A0A62),
	hex_p2[2] =        HD_INIT(0x404C9AA7,0x360AD48A),
	hex_p3[2] =        HD_INIT(0xC043D82C,0xA9A6DA9F),
	hex_p4[2] =        HD_INIT(0x40244E17,0x64EC3927),
	hex_p5[2] =        HD_INIT(0xBFE64BBD,0xB5E61E65),
	hex_q0[2] =        HD_INIT(0xC06486C0,0x3E2B87CC),
	hex_q1[2] =        HD_INIT(0x407A124F,0x101EB843),
	hex_q2[2] =        HD_INIT(0xC077DDCE,0xFC56A848),
	hex_q3[2] =        HD_INIT(0x4062DE7C,0x96591C70),
	hex_q4[2] =        HD_INIT(0xC037D2E8,0x6EF9861F);


double
asin(arg)
double	arg;
{
    STATIC register int     i;
    STATIC register UInt32  argsign;
    STATIC realnumber       y, g;


    y.value = arg;

    if (isNAN(y)) {
	return( _fp_error((UInt32) (FP_asin | SIGNAN), y.value) );
    }

    argsign = y.half.hi & 0x80000000;		/* save sign */
    y.half.hi ^= argsign;			/* y = abs(arg) */

    if (y.value > HALF) {		/* compare with 1/2 */
	if (y.value > ONE)  		/* abs(arg) > 1 means error */
	    return( _fp_error((UInt32) (FP_asin | OPERROR)) );

	i = 1;				/* set i for abs(arg) > 1/2 */
 /* Step 8 */
#ifdef	hp64000
    /* NOTE: The unary plus prevents the constant folding of the two halfs */
	g.value = +(HALF - y.value) + HALF;	/* g = 1.0 - Y */
#else
	g.value = HALF - y.value;		/* g = 1.0 - Y */
	g.value += HALF;
#endif

	if (isNonZero(g)) {		/* Is it non-zero? */
	    g.half.hi -= 0x100000;		/* g = 0.5 * g */
	    y.value = sqrt(g.value);		/* Y = sqrt(g) */
	    y.half.hi += 0x80100000;		/* Y = -2.0 * Y */
	    goto step9;			/* evaluate polynomial */
	}
	else				/* Argument is a zero */
	    y.value = ZERO;
    } 
    else {
	i = 0;				/* set i for abs(arg) < 1/2 */
	if (y.value >= ASIN_EPS) {	/* significance? */
 /* Step 7 */
	    g.value = y.value * y.value;	/* calculate asin */
step9:		/* Step 9 */
	    y.value += y.value * (
			(((((P5 * g.value + P4) * g.value + 	/* P(g) */
			P3) * g.value + P2) * g.value + P1) * g.value)
		       /
			(((((g.value + Q4) * g.value + 		/* Q(g) */
			Q3) * g.value + Q2) * g.value + Q1) * g.value + Q0)
		       );
	}
    }

 /* Now adjust result (if necessary) by PI/2 */
    if (i)
	y.value = ((PI_OVER_4 + y.value) + PI_OVER_4);
    y.half.hi ^= argsign;	/* Complement the sign if the arg was neg. */

    return(y.value);
}  /* asin() */

#endif /* EXCLUDE_m_asin */


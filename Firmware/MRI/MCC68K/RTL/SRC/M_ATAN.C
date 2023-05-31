#include "lib_top.h"


#if EXCLUDE_m_atan || _80960SB || _80960KB || _80960MC || (_MCCG32 && _FPU) \
	|| (_MCC68K && _FPU) \
	|| (_MCC86 && (! _FPU)) \
	|| (__OPTION_VALUE ("-Kq"))
    #pragma option -Qws			/* Prevent compiler warning */
#else

/*
 * @(SUBID) 05/10/88 /src2/nls/libraries/libm/s.atan.c 1.17
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
    DOUBLE PRECISION ARCTANGENT

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

      The routine 'atan()' now handles all IEEE standard double numbers.
    The only changes which were needed to do this was to add checking
    for NaNs.				*/
/*
Change history:
	11/15/93 - jpc	Split atan2 into seperate module.
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

#define ATN_MAX          9007199254740992.0
#define PI               (*((const double *) hex_pi))
#define PI_OVER_2        (*((const double *) hex_pi_over_2))
#define SQRT3            (*((const double *) hex_sqrt3))
#define SQRT3_MINUS1     (*((const double *) hex_sqrt3_minus1))
#define TWO_MINUS_SQRT3  (*((const double *) hex_two_minus_sqrt3))
#define P0               (*((const double *) hex_p0))
#define P1               (*((const double *) hex_p1))
#define P2               (*((const double *) hex_p2))
#define P3               (*((const double *) hex_p3))
#define Q0               (*((const double *) hex_q0))
#define Q1               (*((const double *) hex_q1))
#define Q2               (*((const double *) hex_q2))
#define Q3               (*((const double *) hex_q3))
#define A1               (*((const double *) hex_a1))
#define A2               (*((const double *) hex_a2))
#define A3               (*((const double *) hex_a3))
#define ATN_EPS          (*((const double *) hex_atn_eps))

static const UInt32
	hex_pi[2] =              HD_INIT(0x400921FB,0x54442D18),
	hex_pi_over_2[2] =       HD_INIT(0x3FF921FB,0x54442D18),
	hex_sqrt3[2] =           HD_INIT(0x3FFBB67A,0xE8584CAA),
	hex_sqrt3_minus1[2] =    HD_INIT(0x3FE76CF5,0xD0B09955),
	hex_two_minus_sqrt3[2] = HD_INIT(0x3FD12614,0x5E9ECD56),
	hex_p0[2] = HD_INIT(0xC02B60A6,0x51061CE2),
	hex_p1[2] = HD_INIT(0xC034817F,0xB9E2BCCB),
	hex_p2[2] = HD_INIT(0xC020FD3F,0x5C8D6A63),
	hex_p3[2] = HD_INIT(0xBFEACD7A,0xD9B187BD),
	hex_q0[2] = HD_INIT(0x4044887C,0xBCC495A9),
	hex_q1[2] = HD_INIT(0x40558A12,0x040B6DA5),
	hex_q2[2] = HD_INIT(0x404DCA0A,0x320DA3D7),
	hex_q3[2] = HD_INIT(0x402E0C49,0xE14AC710),
	hex_a1[2] = HD_INIT(0x3FE0C152,0x382D7366),
	hex_a2[2] = HD_INIT(0x3FF921FB,0x54442D18),
	hex_a3[2] = HD_INIT(0x3FF0C152,0x382D7366),
	hex_atn_eps[2] = HD_INIT(0x3E46A09E,0x667F3BCD);	/* 2**(53/2) */


double
atan(arg)
double	arg;
{
    STATIC realnumber		f, g;
    STATIC register int	n;
    STATIC register UInt32	argsign;


    f.value = arg;

    if (isNAN(f)) {
	return( _fp_error((UInt32) (FP_atan | SIGNAN), f.value) );
    }

    argsign = f.half.hi & 0x80000000;		/* save sign */
    f.half.hi ^= argsign;                       /* f = abs(arg) */

    if (f.value >= ATN_MAX) {		/* for large arguments... */
	f.value = PI_OVER_2;		/* answer is PI/2... */
	f.half.hi |= argsign;		/* with sign of original argument */
	return(f.value);
    }

 /* Step 6 */
    if (f.value > ONE) {		/* check for 1st reduction */
	f.value = ONE / f.value;
	n = 2;
    } 
    else
	n = 0;

 /* Step 7 */
    if (f.value > TWO_MINUS_SQRT3) {	/* 2nd reduction */
 /* Step 8 */
	g.value = (SQRT3_MINUS1 * f.value) - HALF;
	g.value -= HALF;
	f.value = (g.value + f.value) / (SQRT3 + f.value);
	n++;
    }


 /* f is now less than 2 - sqrt(3) */
    g.value = f.value;
    g.half.hi &= 0x7FFFFFFF;		/* g = abs(f) */

 /* Step 10 */
    if (g.value >= ATN_EPS) {
 /* Step 11 */
	g.value = f.value * f.value;		/* calculate atan */
 /* Steps 13 and 14 */
	f.value += f.value * (((((P3 * g.value + P2) * g.value + P1) 
	    * g.value + P0) * g.value) / ((((g.value + Q3) 
	    * g.value + Q2) * g.value + Q1) * g.value + Q0));
    }

    if (n > 1)
	f.half.hi ^= 0x80000000;	/* change sign if necessary */

    switch (n) {			/* add an appropriate constant */
	case 0:			 break;	/* Don't need to add anything */
	case 1: f.value += A1;   break;
	case 2: f.value += A2;   break;
	case 3: f.value += A3;   break;
    }

    f.half.hi ^= argsign;		/* adjust sign */
    return(f.value);
}  /* atan() */

#endif /* EXCLUDE_m_atan */


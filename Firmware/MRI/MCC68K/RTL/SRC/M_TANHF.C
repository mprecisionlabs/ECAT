#include "lib_top.h"


#if EXCLUDE_m_tanhf || _80960SB || _80960KB || _80960MC || (_MCCG32 && _FPU) \
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
    SINGLE PRECISION HYPERBOLIC TANGENT

    Author: Mark McDowell
    Date:   December 19, 1984
    Source: Software Manual for the Elementary Functions
	by William James Cody, Jr. and William Waite (1980)

    This routine will work only for numbers in IEEE single precision
    floating point format. Of the numbers in this format, it checks for 
    and handles only normalized numbers and -0 and +0 -- not infinities, 
    denormalized numbers, nor NaN's.

*/
/*---------------------------------------------------------------------
    Revised by Michael Fluegge		07/13/87

      This routine required virtually zero changes in order to handle
    all of the IEEE floating-point number types.  The only change
    needed was a test for an NaN.			*/

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

#define XBIG        0x41102CB3
#define LOG3_OVER_2 0x3F0C9F54
#define TANH_EPS    0x39800000
#define P0          (*((const double *) &hex_p0))
#define P1          (*((const double *) &hex_p1))
#define Q0          (*((const double *) &hex_q0))

static const UInt32	hex_p0 = 0xBF52E2C6,
			hex_p1 = 0xBB7B11B2,
			hex_q0 = 0x401E2A1A;


double tanh(double arg)
{
    STATIC realnumberF		f, g;
    STATIC register UInt32	argsign;

    f.value = arg;

    if (isNANf(f))
	return( _fp_errorf((UInt32) (FP_tanhf | SIGNAN), f.ivalue) );

    argsign = f.ivalue & 0x80000000;	/* save sign */
    f.ivalue ^= argsign;                /* f = abs(arg) */

    if (f.ivalue > XBIG) {              /* tanh of large value is +1 or -1 */
	f.value = ONEf;
	f.ivalue |= argsign;            /* give proper sign */
	return(f.value);
    }

    if (f.ivalue > LOG3_OVER_2) {	/* compare with log(3)/2 */
	f.ivalue += 0x800000;		/* f *= 2 */
	f.value = HALFf - ONEf / (exp(f.value) + ONEf);
	f.ivalue += 0x800000;		/* f *= 2 */
	f.ivalue ^= argsign;            /* give proper sign */
	return(f.value);
    }

    if (f.ivalue < TANH_EPS) {
	f.ivalue ^= argsign;
	return(f.value);		/* tanh(x) = x for small x */
    }

    g.value = f.value * f.value;	/* calculate tanh */
    f.value += f.value * (
		((P1 * g.value + P0) * g.value)
    		/
		(g.value + Q0)
	);

    f.ivalue ^= argsign;                /* give proper sign */

    return(f.value);
}  /* tanhf() */
 
#endif /* EXCLUDE_m_tanhf */
 

#include "lib_top.h"


#if EXCLUDE_m_cosh || _80960SB || _80960KB || _80960MC || (_MCCG32 && _FPU) \
	|| (_MCC68K && _FPU) \
	|| (_MCC86 && (! _FPU)) \
	|| (__OPTION_VALUE ("-Kq"))
    #pragma option -Qws			/* Prevent compiler warning */
#else

/*
 * @(SUBID) 05/10/88 /src2/nls/libraries/libm/s.cosh.c 1.12
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
    DOUBLE PRECISION HYPERBOLIC COSINE

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
    numbers.  NaNs are simply returned.  The infinities cause positive 
    infinity to be returned.  There is no special processing needed for
    the denormals.		*/
/*
Change history:
	11/03/93 - jpc  Converted "INTEL" and "MOTOROLA" to "_LITTLE_ENDIAN"
			and _BIG_ENDIAN, respectively; removed special handling
			of NaN's for little-endian machines -- all NaN's are
			handled the same now.
*/

#include "m_incl.h"
#include "op_reent.h"

#define YBAR  (*((const double *) hex_ybar))
#define LNV   (*((const double *) hex_lnv))
#define V2M1  (*((const double *) hex_v2m1))
#define WMAX  (*((const double *) hex_wmax))

static const UInt32	hex_ybar[2] = HD_INIT(0x4086232B,0xDD7ABCD2),
			hex_lnv[2]  = HD_INIT(0x3FE62E60,0x00000000),
			hex_v2m1[2] = HD_INIT(0x3EED0112,0xEB0202D6),
			hex_wmax[2] = HD_INIT(0x40862E3C,0x85B28BDB);


double
cosh(arg) 
double	arg; 
{
    STATIC realnumber  y, f;


    y.value = arg;

    if (isNAN(y)) {
	return( _fp_error((UInt32) (FP_cosh | SIGNAN), y.value) );
    }

    y.half.hi &= 0x7FFFFFFF;		/* y = abs(arg) */

    if (y.value <= YBAR) {
	f.value = exp(y.value);         /* cosh(y) = (exp(y)+1/exp(y))/2 */
	f.value += ONE / f.value;
	f.half.hi -= 0x100000;		/* f /= 2 */
	return(f.value);
    }

    y.value -= LNV;			/* y = y - log(2) */

    if (y.value > WMAX)                 /* argument too big? */
	return( _fp_error((UInt32) (FP_cosh | OVERFLOW)) );

    f.value = exp(y.value);             /* calculate cosh near MAX */
    f.value += V2M1 * f.value;

    return(f.value);
}  /* cosh() */

#endif /* EXCLUDE_m_cosh */


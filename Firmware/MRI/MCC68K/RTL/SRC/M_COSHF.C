#include "lib_top.h"


#if EXCLUDE_m_coshf || _80960SB || _80960KB || _80960MC || (_MCCG32 && _FPU) \
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
    SINGLE PRECISION HYPERBOLIC COSINE

    Author: Mark McDowell
    Date:   December 19, 1984
    Source: Software Manual for the Elementary Functions
	by William James Cody, Jr. and William Waite (1980)

    This routine will work only for numbers in IEEE single precision
    floating point format. Of the numbers in this format, it checks for 
    and handles only normalized numbers and -0 and +0 -- not infinities, 
    denormalized numbers, nor NaN's.

*/
/*--------------------------------------------------------------------
    Revised by Michael Fluegge		07/13/87
  
      The routine now handles all of the IEEE standard floating point
    numbers.  NaNs are simply returned.  The infinities cause positive 
    infinity to be returned.  There is no special processing needed for
    the denormals.		*/

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

#define YBAR     0x42AEAC50
#define WMAX     0x42B1707A
#define HEX_ONE  0x3F800000

#define LNV     (*((const double *) &hex_lnv))
#define V2M1    (*((const double *) &hex_v2m1))

static const UInt32	hex_lnv  = 0x3F317300,
			hex_v2m1 = 0x37680897;


double cosh(double arg) 
{
    STATIC realnumberF       y, f;

    y.value = arg;

    if (isNANf(y))
	return( _fp_errorf((UInt32) (FP_coshf | SIGNAN), y.ivalue) );

    y.ivalue &= 0x7FFFFFFF;		/* y = abs(arg) */

    if (y.ivalue <= YBAR) {
	f.value = exp(y.value);		/* sinh(y) = (exp(y)-1/exp(y))/2 */
	f.value += ONEf / f.value;
	f.ivalue -= 0x00800000;		/* Multiply f by 0.5 */
	return(f.value);
    }

    y.value -= LNV;			/* y = y - (~= log(2)) */

    if (y.ivalue > WMAX)                /* argument too big? */
	return( _fp_errorf((UInt32) (FP_coshf | OVERFLOW)) );

    f.value = exp(y.value);		/* calculate sinh near MAX */
    f.value += V2M1 * f.value;

    return(f.value);
}  /* coshf() */
 
#endif /* EXCLUDE_m_coshf */
 

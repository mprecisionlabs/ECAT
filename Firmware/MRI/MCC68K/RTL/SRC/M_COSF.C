#include "lib_top.h"


#if EXCLUDE_m_cosf || _80960SB || _80960KB || _80960MC || (_MCCG32 && _FPU) \
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
    SINGLE PRECISION COSINE

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
  
      The routine now handles infinities and NaNs.  Infinities generate
    an NaN while NaN arguments are simply returned.
      No special checking is needed for the denormalized numbers.  The
    cos() of any denormalized value will be one also and it can be computed
    by using the normal code.		*/

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

#define COS_MAX     0x464905B8
#define COS_EPS     0x39800000
#define ONE_OVER_PI (*((const double *) &hex_one_over_pi))
#define PI_OVER_TWO (*((const double *) &hex_pi_over_two))
#define R1          (*((const double *) &hex_r1))
#define R2          (*((const double *) &hex_r2))
#define R3          (*((const double *) &hex_r3))
#define R4          (*((const double *) &hex_r4))
#define PI          (*((const double *) &hex_pi))

static const UInt32	hex_one_over_pi = 0x3EA2F983,
			hex_pi_over_two = 0x3FC90FDB,
			hex_r1 = 0xBE2AAAA4,
			hex_r2 = 0x3C08873E,
			hex_r3 = 0xB94FB222,
			hex_r4 = 0x362E9C5B,
			hex_pi = 0x40490FDB;


double cos(double arg)
{
    STATIC realnumberF      f, g;
    STATIC register UInt32  sgn;

    f.value = arg;

    if (isNANf(f))
	return( _fp_errorf((UInt32) (FP_cosf | SIGNAN), f.ivalue) );

 /* Ignore the sign of the argument */
    f.ivalue &= 0x7FFFFFFF;         /* cos(-x) = cos(x) */

 /* Is the argument an infinity? */
    if (isINFINITYf(f))
	return( _fp_errorf((UInt32) (FP_cosf | OPERROR)) );

 /* Can an accurate number be computed? */
    if (f.ivalue > COS_MAX)
	return( _fp_errorf((UInt32) (FP_cosf | PLOSS)) );


 /* The next two statements do Steps 2, 3, 6, 7 */
 /* divide it by PI and round */
    g.ivalue = (UInt32) ((f.value + PI_OVER_TWO) * ONE_OVER_PI + HALFf); 
    sgn = (g.ivalue & 1) ? 0x80000000 : 0;	/* sign of result */

 /* Step 9 */
    g.value = (double) g.ivalue - HALFf;		/* make into a float */

 /* Step 10 */
    f.value -= g.value * PI;			/* reduce */
    g.ivalue = f.ivalue & 0x7FFFFFFF;		/* g <- abs(f) */

    if (g.ivalue >= COS_EPS) {		/* reduced argument significant? */
 /* Steps 12 - 14 */
	g.value = f.value * f.value;	/* calculate cosine */
	f.value += f.value * ((((R4 * g.value + R3) * g.value + R2) 
	       * g.value + R1) * g.value);
    }

    f.ivalue ^= sgn;			/* set the sign appropriately */
    return(f.value);
}  /* cosf() */
 
#endif /* EXCLUDE_m_cosf */
 

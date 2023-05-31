#include "lib_top.h"


#if EXCLUDE_m_sinf || _80960SB || _80960KB || _80960MC || (_MCCG32 && _FPU) \
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
    SINGLE PRECISION SINE

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
      No special checking is needed for the denormalized numbers.  The sin
    of any denormalized value will be zero and it can be computed using
    the normal code.			*/

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

#define SIN_MAX      0x46490C00
#define SIN_EPS      0x39800000
#define PI_OVER_TWO  0x3FC90FDB
#define ONE_OVER_PI  (*((const double *) &hex_one_over_pi))
#define R1           (*((const double *) &hex_r1))
#define R2           (*((const double *) &hex_r2))
#define R3           (*((const double *) &hex_r3))
#define R4           (*((const double *) &hex_r4))
#define PI           (*((const double *) &hex_pi))

static const UInt32	hex_one_over_pi = 0x3EA2F983,
			hex_r1 =          0xBE2AAAA4,
			hex_r2 =          0x3C08873E,
			hex_r3 =          0xB94FB222,
			hex_r4 =          0x362E9C5B,
                        hex_pi =	  0x40490FDB;

double sin(double arg)
{
    STATIC realnumberF       f, g;
    STATIC register UInt32   sgn;

    f.value = arg;

    if (isNANf(f))
	return( _fp_errorf((UInt32) (FP_sinf | SIGNAN), f.ivalue) );

    sgn = f.ivalue & 0x80000000;	/* save sign */
    f.ivalue ^= sgn;			/* f = abs(arg) */

 /* Is the argument an infinity? */
    if (isINFINITYf(f))
	return( _fp_errorf((UInt32) (FP_sinf | OPERROR)) );
     
 /* Is the argument too large to have any accuracy? */
    if (f.ivalue > SIN_MAX)
	return( _fp_errorf((UInt32) (FP_sinf | PLOSS)) );


    if (f.ivalue > PI_OVER_TWO) {	/* does argument need reduction? */
 /* divide it by PI and round */
	g.ivalue = (UInt32) (f.value * ONE_OVER_PI + HALFf);
 /* If an odd multiple of PI, change sign */
	if (g.ivalue & 1)
	    sgn ^= 0x80000000;

	f.value -= g.ivalue * PI;
	g.ivalue = f.ivalue & 0x7FFFFFFF;	/* g <- abs(f) */
    } 
    else
	g.value = f.value;		/* argument already reduced */

 /* Step 11 starts here */
    if (g.ivalue >= SIN_EPS) {          /* reduced argument significant? */
	g.value = f.value * f.value;	/* calculate sine */  /* Step 12 */
 /* Steps 13 and 14 combined */
	f.value += f.value * ((((R4 * g.value + R3) * g.value + R2) *
		   g.value + R1) * g.value);
    }

    f.ivalue ^= sgn;		/* set the sign appropriately */
    return(f.value);
}  /* sinf() */
 
#endif /* EXCLUDE_m_sinf */
 

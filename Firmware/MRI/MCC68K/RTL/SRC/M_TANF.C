#include "lib_top.h"


#if EXCLUDE_m_tanf || _80960SB || _80960KB || _80960MC || (_MCCG32 && _FPU) \
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
    SINGLE PRECISION TANGENT

    Author: Mark McDowell
    Date:   December 20, 1984
    Source: Software Manual for the Elementary Functions
	by William James Cody, Jr. and William Waite (1980)

    This routine will work only for numbers in IEEE single precision
    floating point format. Of the numbers in this format, it checks for 
    and handles only normalized numbers and -0 and +0 -- not infinities, 
    denormalized numbers, nor NaN's.

*/
/*---------------------------------------------------------------------
    Revised by Michael Fluegge		07/13/87
  
      All of the IEEE numbers are now handled.  NaNs are returned.
    Infinities cause an exception and an NaN is returned.  Denormals
    will simply be returned after going through much of the
    computation.		*/

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

#define TAN_EPS      0x39800000		/* 2**-(24/2) == .000244140625 */
#define TAN_MAX      0x45C90800		/* INT(2**(24/2) * PI/2 */
#define PI_OVER_4    0x3F490FDB
#define TWO_OVER_PI  (*((const double *) &hex_two_over_pi))
#define P1           (*((const double *) &hex_p1))
#define Q1           (*((const double *) &hex_q1))
#define Q2           (*((const double *) &hex_q2))
#define PI_OVER_TWO  (*((const double *) &hex_pi_over_two))

static const UInt32	hex_two_over_pi =    0x3F22F983,
			hex_p1 =             0xBDC433B8,
			hex_q1 =             0xBEDBB7AF,
			hex_q2 =             0x3C1F3375,
			hex_pi_over_two =    0x3FC90FDA;


double tan(double arg)
{
    STATIC realnumberF             f, t1, t2;
    STATIC double                   g;
    STATIC register UInt32         argsign, n;


    t1.value = arg;

    if (isNANf(t1))
	return( _fp_errorf((UInt32) (FP_tanf | SIGNAN), t1.value) );

    argsign = t1.ivalue & 0x80000000;	/* argsign = sign of arg */
    t1.ivalue ^= argsign;		/* t1 = abs(arg) */

 /* Is the argument an infinity or will there be bad loss of precision? */
    if (isINFINITYf(t1))
	return( _fp_errorf((UInt32) (FP_tanf | OPERROR)) );
     
    if (t1.ivalue > TAN_MAX)
	return( _fp_errorf((UInt32) (FP_tanf | PLOSS)) );
     

    if (t1.ivalue > PI_OVER_4) {            /* does argument need reduction? */
 /* Divide by PI/2 and round */
	n = (UInt32) (t1.value * TWO_OVER_PI + HALFf);
	t2.value = (double) n;			/* t2 <- multiple of PI/2's */
	f.value = t1.value - t2.value * PI_OVER_TWO;	/* reduce */
	f.ivalue ^= argsign;			/* give it the correct sign */
	t1.ivalue = f.ivalue & 0x7FFFFFFF;	/* t1 <- abs(reduced arg) */
	n &= 1;                 /* n becomes odd/even flag */
    } 
    else {
	f.value = arg;		/* no reduction needed */
	n = 0;                  /* even multiple of PI/4 */
    }

    if (t1.ivalue < TAN_EPS)	/* reduced argument significant? */
 /* Return value is dependent on the multiple of PI/2 */
	return( (n) ? (-ONEf / f.value) : f.value );
    else {                      /* calculate tangent */
	g = f.value * f.value;			/* g is square of f */
	t1.value = P1 * g * f.value + f.value;
	t2.value = (Q2 * g + Q1) * g + ONEf;
	if (n) {                 /* odd multiple? */
	    t2.ivalue ^= 0x80000000;		/* Negate the result */
	    return(t2.value / t1.value);
	}
	else
	    return(t1.value / t2.value);
    }
}  /* tanf() */
 
#endif /* EXCLUDE_m_tanf */
 

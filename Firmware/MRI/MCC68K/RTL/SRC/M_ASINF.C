#include "lib_top.h"


#if EXCLUDE_m_asinf || _80960SB || _80960KB || _80960MC || (_MCCG32 && _FPU) \
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
    SINGLE PRECISION ARCSINE

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
  
      The changes for this routine were minimal.  More comments were
    added, the layout of the routine was changed to be consistent and
    a check was made for NaNs as arguments.  No special processing is
    needed for infinities or denormals.		*/

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

#include <math.h>
#include "m_incl.h"
#include "op_reent.h"

#define ASIN_EPS    0x39800000
#define HEX_HALF    0x3F000000
#define HEX_ONE     0x3F800000
#define PI_OVER_4   (*((const double *) &hex_pi_over_4))
#define P1          (*((const double *) &hex_p1))
#define P2          (*((const double *) &hex_p2))
#define Q0          (*((const double *) &hex_q0))
#define Q1          (*((const double *) &hex_q1))

static const UInt32	hex_pi_over_4 = 0x3F490FDB,
			hex_p1 = 0x3F6F166B,
			hex_p2 = 0xBF012065,
			hex_q0 = 0x40B350F0,
			hex_q1 = 0xC0B18D0B;


double asin(double arg)
{
    STATIC register UInt32	argsign;
    STATIC register char	i;
    STATIC realnumberF		y, g;


    y.value = arg;

    if (isNANf(y))
	return( _fp_errorf((UInt32) (FP_asinf | SIGNAN), y.ivalue) );

    argsign = y.ivalue & 0x80000000;		/* save sign */
    y.ivalue ^= argsign;			/* y = abs(arg) */

    if (y.ivalue > HEX_HALF) {			/* compare with 1/2 */
	if (y.ivalue > HEX_ONE)                 /* abs(arg) > 1 means error */
	    return( _fp_errorf((UInt32) (FP_asinf | OPERROR)) );

	i = 1;				/* set i for abs(arg) > 1/2 */
 /* Step 8 */
#ifdef	hp64000
    /* NOTE: The unary plus prevents the constant folding of the two halfs */
	g.value = +(HALFf - y.value) + HALFf;	/* g = 1.0 - Y */
#else
	g.value = HALFf - y.value;		/* g = 1.0 - Y */
	g.value += HALFf;
#endif

	if (g.ivalue) {		/* Is it non-zero? */
	    g.ivalue -= 0x00800000;		/* g = 0.5 * g */
	    y.value   = sqrt(g.value);		/* Y = sqrt(g) */
	    y.ivalue += 0x80800000;		/* Y = -2.0 * Y */
	    goto step9;		/* Go evaluate polynomial */
	}
	else 
	    y.ivalue = 0x0;
    } 
    else {
	i = 0;				/* set i for abs(arg) < 1/2 */
	if (y.ivalue >= ASIN_EPS) {	/* significance? */
 /* Step 7 */
	    g.value = y.value * y.value;	/* calculate asin */
step9:          /* Step 9 */
	    y.value += y.value * (
			  ((P2 * g.value + P1) * g.value) 
		          /
			  ((g.value + Q1) * g.value + Q0)
		       );
	}
    }

 /* Now adjust result (if necessary) by PI/2 */
    if (i) 
	y.value = ((PI_OVER_4 + y.value) + PI_OVER_4);
    y.ivalue ^= argsign;	/* Negate the result if arg was negative */

    return(y.value);
}  /* asinf() */
 
#endif /* EXCLUDE_m_asinf */
 

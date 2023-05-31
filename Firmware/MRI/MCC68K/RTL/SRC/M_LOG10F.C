#include "lib_top.h"


#if EXCLUDE_m_log10f || _80960SB || _80960KB || _80960MC || (_MCCG32 && _FPU) \
	|| (_MCC68K && _FPU) \
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
    SINGLE PRECISION LOG10
    
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
  
      The code now handles all of the special cases of arguments.
    Negative numbers, and +-0.0 produce an exception and return an
    appropriate value.  NaNs are simply returned.
      Special code was added so that the algorithm can be applied to
    denormalized numbers.		*/

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

#include    <stddef.h>		/* Get the definition of errno */
#include    <errno.h>

#define C0        0x3F3504F3
#define C1        (*((double *) &hex_c1))
#define C2        (*((double *) &hex_c2))
#define C3        (*((double *) &hex_c3))
#define A0        (*((double *) &hex_a0))
#define B0        (*((double *) &hex_b0))

static const UInt32	hex_c1 = 0x3F318000,
			hex_c2 = 0xB95E8083,
			hex_c3 = 0x3EDE5BD9,
			hex_a0 = 0xBF0D7E3D,
			hex_b0 = 0xC0D43F3A;


double log10(double arg)
{
    STATIC realnumberF       f, w;
    STATIC register long     n;
    STATIC double             xn;

    f.value = arg;

    if (isNANf(f))
	return( _fp_errorf((UInt32) (FP_log10f | SIGNAN), f.ivalue) );

    if ((n = isZEROf(f)) || isNEGf(f)) {
	f.value = _fp_errorf((UInt32) (FP_log10f | OPERROR));
	if (n) {
	    f.ivalue = 0xFF800000;		/* Return neg. infinity */
	    errno = ERANGE;	/* Set to EDOM by _fp_errorf(); correct it */
	}

	return(f.value);
    }

    if (isINFINITYf(f))
	return(f.value);			/* Return pos. infinity */

#if  DeNormsOK

#include "m_breakf.i"

#else
    if (isDENORMf(f)) {			/* Denormalized operand? */
	(void) _fp_errorf((UInt32) (FP_log10f | OPERROR));
	f.ivalue = 0xFF800000;		/* Return neg. infinity */
	return(f.value);
    }

 /* The number is normal.  Go ahead and compute the value. */
    n = (f.ivalue >> 23) - EXP_BIAS_MNS1_f;	/* isolate exponent */
    f.ivalue &= 0x007FFFFF;		/* scale to between 0.5 and 1.0 */
    f.ivalue |= EXP_BIAS_MNS1_f << 23;
#endif

    if (f.ivalue > C0) {                /* compare with sqrt(2)/2 */
 /* Step 10 */
	w.value = f.value;		/* f = (f - 1) / (f * .5 + .5) */
	w.ivalue -= 0x800000;		/* Divsion by 2 */
#ifdef	hp64000
    /* NOTE: The unary plus prevents the constant folding of the two halfs */
	f.value = (+(f.value - HALFf) - HALFf) / (w.value + HALFf);
#else
	f.value -= HALFf;
	f.value = (f.value - HALFf) / (w.value + HALFf);
#endif
    }
    else {
 /* Step 9 */
	n--;
 /* f = (f - .5) / ((f - .5) * .5 + .5) */
	w.value = f.value - HALFf;		/* w is znum(erator) */
	f.value = w.value / (w.value * HALFf + HALFf);
    }

 /* The variable 'f' is now the 'z' of the book */
    w.value = f.value * f.value;        /* w is now z squared */
    xn = (double) n;			/* convert to double */
    return(
	   C3 * 			/* Step 14 */
	     (
	      ((xn * C2) +
		  (f.value + (f.value * (	/* R(z) */
		     w.value *			/* r(z**2) */
			A0			/* A(w) */
			/ 
	                (w.value + B0)		/* B(w) */
		  )))
	      )
	      + (xn * C1)
	     )
	  );
}  /* log10f() */
 
#endif /* EXCLUDE_m_log10f */
 

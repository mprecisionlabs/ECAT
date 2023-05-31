#include "lib_top.h"


#if EXCLUDE_m_log10 || _80960SB || _80960KB || _80960MC || (_MCCG32 && _FPU) \
	|| (_MCC68K && _FPU) \
	|| (_MCC86 && (!_FPU)) \
	|| (__OPTION_VALUE ("-Kq"))
    #pragma option -Qws			/* Prevent compiler warning */
#else

/*
 * @(SUBID) 05/10/88 /src2/nls/libraries/libm/s.log10.c 1.15
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
    DOUBLE PRECISION LOG10

    Author: Mark McDowell
    Date:   November 27, 1984
    Source: Software Manual for the Elementary Functions
	by William James Cody, Jr. and William Waite (1980)

    This routine will work only for numbers in IEEE double precision
    floating point format. Of the numbers in this format, it checks for 
    and handles only normalized numbers and -0 and +0 -- not infinities, 
    denormalized numbers, nor NaN's.

*/
/*----------------------------------------------------------------------
    Revised by Michael Fluegge		05/10/88

      The code now handles all of the special cases of arguments.
    Negative numbers, and +-0.0 produce an exception and return
    an appropriate value.  NaNs are simply returned.
      Special code was added so that the algorithm can be applied to
    denormalized numbers.				*/
/*
Change history:
	11/03/93 - jpc  Converted "INTEL" and "MOTOROLA" to "_LITTLE_ENDIAN"
			and _BIG_ENDIAN, respectively; removed special handling
			of NaN's for little-endian machines -- all NaN's are
			handled the same now.
*/

#include "m_incl.h"
#include "op_reent.h"

#define C0 (*((const double *) hex_c0))
#define C1 (*((const double *) hex_c1))
#define C2 (*((const double *) hex_c2))
#define C3 (*((const double *) hex_c3))
#define A0 (*((const double *) hex_a0))
#define A1 (*((const double *) hex_a1))
#define A2 (*((const double *) hex_a2))
#define B0 (*((const double *) hex_b0))
#define B1 (*((const double *) hex_b1))
#define B2 (*((const double *) hex_b2))

static const UInt32
	hex_c0[2] = HD_INIT(0x3FE6A09E,0x667F3BCD),   /* ~= sqrt(0.5) */
	hex_c1[2] = HD_INIT(0x3FE63000,0x00000000),   /* 355 / 512 */
	hex_c2[2] = HD_INIT(0xBF2BD010,0x5C610CA8),
	hex_c3[2] = HD_INIT(0x3FDBCB7B,0x1526E50E),
	hex_a0[2] = HD_INIT(0xC05007FF,0x12B3B59A),
	hex_a1[2] = HD_INIT(0x4030624A,0x2016AFED),
	hex_a2[2] = HD_INIT(0xBFE94415,0xB356BD29),
	hex_b0[2] = HD_INIT(0xC0880BFE,0x9C0D9077),
	hex_b1[2] = HD_INIT(0x40738083,0xFA15267E),
	hex_b2[2] = HD_INIT(0xC041D580,0x4B67CE0F);


double
log10(arg)
double	arg;
{
    STATIC register Int32	n;
    STATIC double		xn;
    STATIC realnumber		f, w;


    f.value = arg;

    if (isNAN(f)) {
	return( _fp_error((UInt32) (FP_log10 | SIGNAN), f.value) );
    }

    if ((n = isZERO(f)) || isNEG(f)) {
	f.value = _fp_error((UInt32) (FP_log10 | OPERROR));
	if (n) {
	    f.value = -HUGE_VAL;
	    errno = ERANGE;	/* Set to EDOM by _fp_error(); correct it */
	}

	return(f.value);
    }

    if (isINFINITY(f))
	return(arg);			/* Return the pos. infinity arg */

#if  DeNormsOK

#include  "m_break.i"

#else
    if (isDENORM(f)) {			/* Denormalized operand? */
	(void) _fp_error((UInt32) (FP_log10 | OPERROR));
	return(-HUGE_VAL);
    }

 /* The number is normal.  Go ahead and compute the value */
    n = (f.half.hi >> 20) - EXP_BIAS_MNS1;	/* isolate exponent */
    f.half.hi &= 0xFFFFF;               /* scale to between 0.5 and 1.0 */
    f.half.hi |= (UInt32) EXP_BIAS_MNS1 << 20;
#endif

    if (f.value > C0) {			/* compare with sqrt(2)/2 */
 /* Step 10 */
	w.value = f.value;		/* f = (f - 1) / (f * .5 + .5) */
	w.half.hi -= 0x100000;		/* Division by 2 */
#ifdef	hp64000
    /* NOTE: The unary plus prevents constant folding of the two halfs */
	f.value = (+(f.value - HALF) - HALF) / (w.value + HALF);
#else
	f.value -= HALF;
	f.value = (f.value - HALF) / (w.value + HALF);
#endif
    }
    else {
 /* Step 9 */
	--n;
 /* f = (f - .5) / ((f - .5) * .5 + .5) */
	w.value = f.value - HALF;		/* w is znum(erator) */
	f.value = w.value / (w.value * HALF + HALF);
    }

 /* The variable 'f' is now the 'z' of the book */
    w.value = f.value * f.value;        /* w is now z squared */
    xn = (double) n;            /* xn = FLOAT(n) */
    return(
	C3 *                     /* Step 14 */
	  (
	   ((xn * C2) +
	       (f.value + (f.value * (		/* R(z) */
		  w.value *				/* r(z**2) */
		      ((A2 * w.value + A1) * w.value + A0)	/* A(w) */
		      /						/* B(w) */
		      (((w.value + B2) * w.value + B1) * w.value + B0)
	       )))
	   )
	   + (xn * C1)
	  )
    );
}

#endif /* EXCLUDE_m_log10 */


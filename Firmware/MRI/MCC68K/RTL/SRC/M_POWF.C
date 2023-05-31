#include "lib_top.h"


#if EXCLUDE_m_powf || _80960SB || _80960KB || _80960MC \
	|| (_MCC86 && (! _FPU)) \
	|| (_MCC68K && _FPU) || (!__OPTION_VALUE ("-Kq"))
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


/*	%W% %G%	*/

/*
    SINGLE PRECISION POWER

    Author: Mark McDowell
    Date:   January 4, 1985
    Source: Software Manual for the Elementary Functions
	by William James Cody, Jr. and William Waite (1980)

    This routine will work only for numbers in IEEE single precision
    floating point format. Of the numbers in this format, it checks for 
    and handles only normalized numbers and -0 and +0 -- not infinities, 
    denormalized numbers, nor NaN's.

*/
 /*--------------------------------------------------------------------
     Revised by Michael Fluegge		07/13/87

     This routine now handles all of the IEEE number classes.
   The computation is not carried out directly as described in the book!
   The equation pow(x, y) == exp(y * log(x)) is used instead where the
   computation of the exp() and log() functions are done using doubles.
     This method is faster than that used by the book and it gives
   good accuracy.					*/

/*
Note:
	This routine is the single precision pow () function.  It is
to be compiled only with the -Kq option turned on.  In this mode, all
doubles will be treated as size float (32 bits).
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

#define TRUE	1
#define FALSE	0

#define HEX_ONE		0x3F800000
#define C0              0x3F3504F3
#define ONE_OVER_LN2	(*((double *) &hex_one_over_ln2))
#define LN2_OVER_TWO	(*((double *) &hex_ln2_over_two))
#define LN2             (*((double *) &hex_ln2))
#define POW_EPS         (*((double *) &hex_pow_eps))
#define C1              (*((double *) &hex_c1))
#define C2              (*((double *) &hex_c2))
#define POW_MAX         (*((double *) &hex_pow_max))
#define POW_MIN         (*((double *) &hex_pow_min))
#define A0              (*((double *) &hex_a0))
#define B0              (*((double *) &hex_b0))
#define P0              (*((double *) &hex_p0))
#define P1              (*((double *) &hex_p1))
#define Q0              ((double) 0.5)
#define Q1              (*((double *) &hex_q1))

static const UInt32
	hex_one_over_ln2    = 0x3FB8AA3B,
	hex_ln2_over_two    = 0x3EB17218,
	hex_ln2		    = 0x3F317218,
	hex_pow_eps	    = 0x33000000,
	hex_c1		    = 0x3F318000,
	hex_c2		    = 0xB95E8082,
	hex_pow_max	    = 0x42B17218,			/*  88.722839 */
	hex_pow_min	    = 0xC2CE8ED0,			/* -103.27893 */
	hex_a0		    = 0xBF0D7E3C,
	hex_b0		    = 0xC0D43F3A,
	hex_p0		    = 0x3E800000,
	hex_p1		    = 0x3B885308,
	hex_q1		    = 0x3D4CBF5B;


static int
isINTEGER(double arg, int * oddflag)
{
    STATIC register UInt32	mask;
    STATIC register int	intflag, bits;
    STATIC realnumberF		num;

    *oddflag = FALSE;		/* Assume the number is even */
    intflag = FALSE;		/* Assume the number is not an integer */
    num.value = arg;

    bits = 150 - ((num.ivalue >> 23) & 0x0FF);  /* # bits to mask */

    if (bits < 0)
	intflag = TRUE;
    else if (bits <= 23) {	/* binary point within mantissa? */
	mask = ((UInt32) 0x1 << bits) - 1;
	if ( ! (num.ivalue & mask)) {	/* check for fractional bits */
	    intflag = TRUE;
 /* Look at the bit on the left of the binary point to determine odd or even */
	    *oddflag = (num.ivalue & (mask + 1)) != 0;
	}
    } 

    return(intflag);
}  /* isINTEGER() */


double pow(double x_arg, double y_arg)
{
    STATIC register	realnumberF		x, y, w;
    STATIC register	realnumberF		t1, t2, g;
    STATIC long		n;
    STATIC double	z, xn;
    STATIC int		yodd, an_int;
    STATIC UInt32	tmp1, tmp2;


    x.value = x_arg;
    y.value = y_arg;

#define  CONTIN  1
#define  RETURN  2
#define  EXCEPT  3

 /*    Assume that the value is special and that powf() can be 
  *  determined without the normal computation.  Also assume
  *  that the return value will be infinity.	*/
    n = RETURN;
    w.ivalue = 0x7F800000;

 /* If NaN then return the non-signaling version of the argument */
    if ((tmp1 = isNANf(x)) || isNANf(y)) {
	w.value = _fp_errorf((UInt32) (FP_powf | SIGNAN),
			    (tmp1) ? x.ivalue : y.ivalue);
    }
 /*    Check for zeros and denormals.  If denormals are not valid
  *  arguments, then treat them as if they were zero.		*/
 /* Notice that the logical OR is not used in the 'if' expression, this
  is because we do not want the boolean expression to short circuit.  Both
  tmp1 and tmp2 should be assigned a value, not tmp1 only in the case
  where the first operand is zero.		*/
    else if (
#if	DeNormsOK
	      (tmp1 = isZEROf(x))  |  (tmp2 = isZEROf(y))
#else
             (tmp1 = ExpZeroFp(x)) | (tmp2 = ExpZeroFp(y))
#endif
	                                                   ) {
 /* If both are zero or first is zero and second is negative
  		then handle properly */
	if (tmp1) {
	    if (tmp2)
		return ONEf;	/* 0.0 ** 0.0 == 1.0  !!!! - jpc */
	    else if (isNEGf (y)) {	/* 0.0 ** -x == <inf> !!!! - jpc */
		w.value = _fp_errorf ((UInt32) (FP_powf | OVERFLOW));
		errno = EDOM;		/* ANSI requires domain error */
		return w.value;
	    }
	    else w.value = ZEROf;
	}
	else
	    w.value = ONEf;
    }
    else if ((tmp1 = isINFINITYf(x)) | (tmp2 = isINFINITYf(y))) {
	if (tmp1) {		/* x_arg an infinity? */
	    if (isNEGf(y))
		w.value = ZEROf;
	    else {
		if (isNEGf(x)) {
		    if (tmp2) 		/* Was the call pow(-inf., +inf.)? */
			n = EXCEPT;
		    else {		/* Call was pow(-inf. +y) */
	 		if ( ! isINTEGER(y.value, &yodd))
			    n = EXCEPT;
			if (yodd)
			    w.ivalue |= 0x80000000;	    /* return -inf. */
		    }
		}
	    }
	}
	else {			/* y_arg is an infinity, x_arg is not */
	    if (isNEGf(x)) {
		if (x.ivalue == (HEX_ONE | 0x80000000)) {
		    n = EXCEPT;
		    w.value = ONEf;
		}
		else {
		    tmp2 = isNEGf(y);
		    if ( (tmp2 && x.value > -ONEf) ||
			          ((! tmp2) && x.value < -ONEf) )
			n = EXCEPT;
		    else
			w.value = ZEROf;
		}
	    }
	    else {
		tmp2 = isNEGf(y);
		if (x.ivalue == HEX_ONE)
		    w.value = x_arg;
		else if ((tmp2 && x.ivalue > HEX_ONE) ||
			            ((! tmp2) && x.ivalue < HEX_ONE))
		    w.value = ZEROf;
		else if ((!tmp2) && (x.ivalue > HEX_ONE))
		    return _fp_errorf ((UInt32) (FP_powf | OVERFLOW));
	    }
	}
    }
 /*    Can not quickly determine the function value.
  *    Have to go through the complete computation.		*/
    else
	n = CONTIN;


    if ( ! (n == CONTIN)) {
	if (n == EXCEPT)
	    (void) _fp_errorf((UInt32) (FP_powf | OPERROR));
	return(w.value);
    }



 /* If x_arg is negative then y_arg must be an integer
	otherwise there is an exception.	*/
    if (isNEGf(x)) {		/* is x negative? */
 /* Now must determine if y is an integer */
	if ( ! isINTEGER(y.value, &yodd))
	    return( _fp_errorf((UInt32) (FP_powf | OPERROR)) );

	x.ivalue &= 0x7FFFFFFF;			/* abs(x) */
    }
    else
	yodd = FALSE;

    if (x.ivalue == HEX_ONE) {		/* x_arg equal +-1.0? */
	w.ivalue = HEX_ONE;		/* Short-circuit the computation */
	goto FixSign;
    }

 /*  To take care of some complaints, if y.value is integral and <= 20 */
 /*  compute by simply multiplying.  */
    {
    double integral, runningProd;
    int	mask, intVal;

	if (modf (y_arg, &integral) == 0.) {	/* Is y an integer ? */
	    if ((integral <= 63.) && (integral > 0.0)) {
		mask = 32;			/* set mask */
		intVal = integral;
		runningProd = 1.;
		while (mask) {
		    if (runningProd != 1.)
			runningProd *= runningProd;
		    if (mask & intVal) {
			if (runningProd == 1.) {
			    runningProd = x_arg;
			}
			else {
			    runningProd *= x_arg;
			}
		    }
		    mask >>= 1;
		}
	        if (runningProd == HUGE_VAL)	/* check for overflow */
		    runningProd = _fp_errorf ((UInt32) (FP_pow | OVERFLOW));
		return runningProd; 			/* return */
	    }
	}
    }

 /* Decompose the first argument into a power of two and a fraction */
    x.value = frexp(x.value, &an_int);
    n = (long) an_int;

    if (x.ivalue > C0) {                /* compare with sqrt(2)/2 */
	t1.value = t2.value = x.value;	/* f = (f-1)/(f*.5 + .5) */
	t1.ivalue -= 0x800000;		/* t1 *= 0.5 */
	t2.value = ((t2.value - HALFf) - HALFf) / (t1.value + HALFf);
    }
    else {
	n--;
 /* f = (f-.5)/((f-.5) * .5 + .5) */
	t1.value = x.value - HALFf;
	t2.value = t1.value / (t1.value * HALFf + HALFf);
    }

    t1.value = t2.value * t2.value;	/* calculate log */
    xn = n;				/* convert to double */
    t1.value = y.value * (((xn * C2) + (t2.value +
		(t2.value * (t1.value * A0 / (t1.value + B0))))) + (xn * C1));


 /* At this point, t1.value = y_arg * ln(x_arg) */

    if (t1.value > POW_MAX) {		/* Overflow? */
	w.value = _fp_errorf((UInt32) (FP_powf | OVERFLOW));
	goto FixSign;
    }
    if (t1.value < POW_MIN) {		/* Underflow all the way to 0.0? */
	w.value = _fp_errorf((UInt32) (FP_powf | UNDERFLOW),
				100, x.ivalue = 0, 1);
		/*	Set the shift_count value to 100 so that there is
		    no doubt the value underflows to 0.0.  The round/sticky
		    variable is non-zero so that the resulting value will
		    be taken to be inexact.
		*/
	goto FixSign;
    }

    tmp1 = t1.ivalue & 0x80000000;	/* save sign of y * ln(x) */
    t1.ivalue ^= tmp1;			/* t1 = abs(t1) */

 /* for small values, answer is 1.0 */
    if (t1.value < POW_EPS) {
	w.ivalue = HEX_ONE;		/* Short-circuit the computation */
	goto FixSign;
    }

    if (t1.value > LN2_OVER_TWO) {      /* argument reduction needed? */
	n = (long) (t1.value * ONE_OVER_LN2 + HALFf);	/* divide by log(2) */
	xn = n;        			/* convert to double */
	g.value = t1.value - xn * LN2;  /* reduce */
	g.ivalue ^= tmp1;	/* adjust sign appropriately */
	if (tmp1)
	    n = -n;		/* adjust multiple's sign */
    }
    else {			/* no reduction needed */
	n = 0;
	g.value = t1.value;
	g.ivalue |= tmp1;	/* restore sign */
    }

    z = g.value * g.value;		/* calculate exp */
    t1.value = (P1 * z + P0) * g.value;
    t2.value = Q1 * z + Q0;
    t1.value = HALFf + (t1.value / (t2.value - t1.value));
    if (n <= -127) {		/* result is denorm -- so special handling */
	int  shift = 0;
	long guard;
	long round;

	if (0x3f000000 > t1.ivalue)		/* t1.value < .5 ?? */
	    ++shift;
	if ((shift += (-126 - n)) >= 24)	       /* compute shift value */
	    t1.ivalue = 0;
	else {
	    t1.ivalue = 0x00800000 | (t1.ivalue & 0x007fffff);/* visible bit */
	    guard = (0x007fffff >> (24 - shift)) & t1.ivalue;/* compute guard */
	    round = (0x00800000 >> (24 - shift)) & t1.ivalue;/* compute round */
	    t1.ivalue >>= shift;			/* shift and rebuild */
	    if (round && (guard || (t1.ivalue & 1)))
		++t1.ivalue;
	    }
        }
    else
	t1.ivalue += (n + 1) << 23;			/* adjust exponent */

    w.ivalue = t1.ivalue;		/* For "FixSign" code */

#if 0					/* Code no long used -- jpc */
 /* Convert the result to a double, but make sure that no error is reported */
    tmp1 = _get_fp_status();		/* Save the user's error bits */
    tmp2 = _get_fp_control();		/* Save the user's control bits */
    _clear_fp_status();			/* Clear the error bits */
    _set_fp_control(NOTRAP);		/* Turn off all trapping */

    w.value = (double) t1.value;		/* Convert to a double */

    _set_fp_control( (int) tmp2);	/* Restore the user's control bits */
    tmp2 = _get_fp_status();		/* Save the error indication */
    _set_fp_status( (int) tmp1);	/* Restore the user's error bits */

 /* The only possible errors are UNDERFLOW and INEXACT.  The computation 
  would not have gotten this far if there was to be OVERFLOW.	*/
    if (tmp2 & UNDERFLOW)		/* Underflow? */
	w.value = _fp_errorf((UInt32) (FP_powf | UNDERFLOW), 100, w.value, 1);
    else {
	if (tmp2 & INEXACT) {
	    _set_fp_status( (int) (tmp1 | INEXACT));
	    if (_get_fp_control() & INEXACT)
		w.value = _fp_errorf((UInt32) (FP_powf | INEXACT));
	}
    }
#endif

FixSign:
    if (w.ivalue == 0)
	errno = ERANGE;		/* Set errno appropriately */

    if (yodd)			/* 'yodd' indicates the sign of the result */
	w.ivalue |= 0x80000000;
    return(w.value);
}  /* powf() */
 
#endif /* EXCLUDE_m_powf */
 

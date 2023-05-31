#include "lib_top.h"


#if EXCLUDE_m_pow || _80960SB || _80960KB || _80960MC \
	|| (_MCC86 && (! _FPU)) \
	|| (_MCC68K && _FPU) || (__OPTION_VALUE ("-Kq")) || (! INCLUDE_FAST_POW)
    #pragma option -Qws			/* Prevent compiler warning */
#else

/*
 * @(SUBID) 05/10/88 /src2/nls/libraries/libm/s.pow.c 1.17
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
    DOUBLE PRECISION POWER

    Author: Mark McDowell
    Date:   November 27, 1984
    Source: Software Manual for the Elementary Functions
	by William James Cody, Jr. and William Waite (1980)

    This routine will work only for numbers in IEEE double precision
    floating point format. Of the numbers in this format, it checks for 
    and handles only normalized numbers and -0 and +0 -- not infinities, 
    denormalized numbers, nor NaN's.

    History:
	08/31/89 jpc:	cast the value of p in pow to int when used as
			index to array to allow mcc86 to work correctly
	02/06/90 jpc:   0.0 ^ 0.0 now returns 1.0 rather than NaN
	02/07/90 jpc:   0.0 ^ -x  now returns <inf>
	04/11/90 jpc:   Reference to _fperror -> _fp_error
	11/03/93 - jpc  Converted "INTEL" and "MOTOROLA" to "_LITTLE_ENDIAN"
			and _BIG_ENDIAN, respectively; removed special handling
			of NaN's for little-endian machines -- all NaN's are
			handled the same now.
	03/10/94 jpc:	When exponent is integer representable in two 
			unsigned longs, execute by sequence of multiplications.
*/
 /*--------------------------------------------------------------------
     Revised by Michael Fluegge		05/10/88

     This routine now handles all of the IEEE number classes.	*/


#include "m_incl.h"
#include "op_reent.h"

#define MAX_EXACT_INT   (18446744073709549567.) /* Max double which can */
                                                /* represent an integer */
                                                /* exactly. */

#define TRUE	1
#define FALSE	0

#define ONE_SIXTEENTH  0.0625 
#define P1             (*((const double *) hex_p1))
#define P2             (*((const double *) hex_p2))
#define P3             (*((const double *) hex_p3))
#define P4             (*((const double *) hex_p4))
#define Q1             (*((const double *) hex_q1))
#define Q2             (*((const double *) hex_q2))
#define Q3             (*((const double *) hex_q3))
#define Q4             (*((const double *) hex_q4))
#define Q5             (*((const double *) hex_q5))
#define Q6             (*((const double *) hex_q6))
#define Q7             (*((const double *) hex_q7))
#define K              (*((const double *) hex_k))

static const UInt32	hex_p1[2] = HD_INIT(0x3FB55555,0x5555554D),
			hex_p2[2] = HD_INIT(0x3F899999,0x999E080E),
			hex_p3[2] = HD_INIT(0x3F624924,0x2E278DAC),
			hex_p4[2] = HD_INIT(0x3F3C78FD,0xDB4AFC28),
			hex_q1[2] = HD_INIT(0x3FE62E42,0xFEFA39EF),
			hex_q2[2] = HD_INIT(0x3FCEBFBD,0xFF82C4CE),
			hex_q3[2] = HD_INIT(0x3FAC6B08,0xD703026D),
			hex_q4[2] = HD_INIT(0x3F83B2AB,0x6E131D98),
			hex_q5[2] = HD_INIT(0x3F55D87E,0x18D7CD9F),
			hex_q6[2] = HD_INIT(0x3F242F7A,0xE0384C74),
			hex_q7[2] = HD_INIT(0x3EEF4EDD,0xE392CC80),
			hex_k[2]  = HD_INIT(0x3FDC551D,0x94AE0BF8);

static const UInt32	hex_a1[18][2] = {
	HD_INIT(0x00000000,0x00000000), HD_INIT(0x3FF00000,0x00000000),
	HD_INIT(0x3FEEA4AF,0xA2A490DA), HD_INIT(0x3FED5818,0xDCFBA487),
	HD_INIT(0x3FEC199B,0xDD85529C), HD_INIT(0x3FEAE89F,0x995AD3AD),
	HD_INIT(0x3FE9C491,0x82A3F090), HD_INIT(0x3FE8ACE5,0x422AA0DB),
	HD_INIT(0x3FE7A114,0x73EB0187), HD_INIT(0x3FE6A09E,0x667F3BCD),
	HD_INIT(0x3FE5AB07,0xDD485429), HD_INIT(0x3FE4BFDA,0xD5362A27),
	HD_INIT(0x3FE3DEA6,0x4C123422), HD_INIT(0x3FE306FE,0x0A31B715),
	HD_INIT(0x3FE2387A,0x6E756238), HD_INIT(0x3FE172B8,0x3C7D517B),
	HD_INIT(0x3FE0B558,0x6CF9890F), HD_INIT(0x3FE00000,0x00000000)
};
static const UInt32	hex_a2[9][2] = {
	HD_INIT(0x00000000,0x00000000), HD_INIT(0xBC7E9C23,0x179C0000),
	HD_INIT(0x3C611065,0x89500000), HD_INIT(0x3C5C7C46,0xB0700000),
	HD_INIT(0xBC641577,0xEE040000), HD_INIT(0x3C76324C,0x05460000),
	HD_INIT(0x3C6ADA09,0x11F00000), HD_INIT(0x3C79B07E,0xB6C80000),
	HD_INIT(0x3C78A62E,0x4ADC0000)
};


static int _get_integer (double input, unsigned long *hi, unsigned long *lo) {

    register	unsigned long	local_hi, local_lo;
    register	int		shift;
    		int		return_val;

    local_hi = GET_HI (input);	/* Crack input double */
    local_lo = GET_LO (input);

    shift =  (0x3ff + 52) - ((local_hi >> 20) & 0x7ff);
    return_val = (local_hi & 0x80000000) ? 1 : 0;
    local_hi &= 0x000fffff;	/* zap exponent */
    local_hi |= 0x00100000;	/* insert visible bit */

    if (shift > 0) {		/* We will shift to the right */
	if (shift >= 32) {	/* low word is all fraction; so discard it */
	    local_lo = local_hi;
	    local_hi = 0;
	    shift -= 32;
	}

	local_lo >>= shift;		/* build lo word */
	*lo = local_lo | (local_hi << (32 - shift));
	*hi = local_hi >> shift;	/* shift hi word */

	return return_val;
    }

    shift = abs (shift);		/* Make positive */
    local_hi <<= shift;
    *hi = local_hi | (local_lo >> (32 - shift));
    *lo = local_lo << shift;
    return return_val;
}


static int isINTEGER(double arg, int *oddflag)
{
    STATIC realnumber		num;
    STATIC register int	intflag, bits;
    STATIC register UInt32	mask;

    *oddflag = FALSE;		/* Assume the number is even */
    intflag = FALSE;		/* Assume the number is not an integer */
    num.value = arg;

 /* 1075 = Exponent bias plus the number of significand bits */
    bits = 1075 - ((num.half.hi >> 20) & 0x7FF);  /* # bits to mask */

    if (bits < 0)
	intflag = TRUE;
    else if (bits <= 52) {		/* binary point within mantissa? */
	if (bits >= 32) {		/* binary point in upper 32 bits? */
	    if ( ! num.half.lo) {
		bits -= 32;		/* # bits to mask */
		mask = ((Int32) 0x1 << bits) - 1;	/* form the mask */
		if ( ! (num.half.hi & mask)) {	/* check for fractional bits */
		    intflag = TRUE;
 /* Look at the least significant bit of arg to determine odd or even */
		    *oddflag = (num.half.hi & (mask + 1)) != 0x0;
		}
	    }
	}
	else {              /* binary point in lower 32 bits */
	    mask = ((Int32) 0x1 << bits) - 1;
	    if ( ! (num.half.lo & mask)) {	/* check for fractional bits */
		intflag = TRUE;
 /* Look at the least significant bit of arg to determine odd or even */
		*oddflag = (num.half.lo & (mask + 1)) != 0;
	    }
	}
    } 

    return(intflag);
}  /* isINTEGER() */


/* reduce() returns a value which is its argument truncated 
             to the nearest multiple of 1/16		*/
static double reduce(double arg)
{
    STATIC realnumber		x;
    STATIC register int	t1;

    x.value = arg;

 /* find binary point */
    if ((t1 = 1071 - ((x.half.hi & 0x7FFFFFFF) >> 20)) > 0) {
	if (t1 <= 32) 		/* in lower 32 bits? */
	    x.half.lo &= 0xFFFFFFFF << t1;
	else {
	    x.half.lo = 0x0;
	    if (t1 <= 52) 	/* in upper 32 bits? */
		x.half.hi &= 0xFFFFFFFF << (t1 - 32);
	    else
		x.half.hi = 0x0;	/* argument is < 1/16 */
	}
    }
    return(x.value);
}  /* reduce() */


double pow(double x_arg, double y_arg)
{
    STATIC register UInt32	x1, x2;
    STATIC register long	iw1, m, p;
    STATIC int			OddInt, intgr;
    STATIC realnumber		u1, g, z, w2;
    STATIC double		v, r, u2, y1, y2, w, w1;
    STATIC const double	*a1 = (const double *) hex_a1,
			*a2 = (const double *) hex_a2;

    u1.value = x_arg;
    w2.value = y_arg;

#define  CONTIN  1
#define  RETURN  2
#define  EXCEPT  3

 /*    Assume that the value is special and that pow() can be 
  *  determined without the normal computation.  Also assume
  *  that the return value will be infinity.	*/
    p = RETURN;
    z.value = HUGE_VAL;

 /* If NaN then simply return the NaN argument */
    if ((x1 = isNAN(u1)) || isNAN(w2)) {
	z.value = _fp_error((UInt32) (FP_pow | SIGNAN),
				(x1) ? u1.value : w2.value);
    }

 /*    Check for zeros and denormals.  If denormals are not valid
  *  arguments then treat them as if they were zero.		*/
 /* Notice that the logical OR is not used in the 'if' expression, this
  is because we do not want the boolean expression to short circuit.  Both
  x1 and x2 should be assigned a value not only x1 in the case where the
  first operand is zero.		*/
    else if (
#if	DeNormsOK
	      (x1 = isZERO(u1))  |  (x2 = isZERO(w2))
#else
             (x1 = ExpZerop(u1)) | (x2 = ExpZerop(w2))
#endif
							) {
 /* If both are zero or first is zero and second is negative
  then handle properly */
	if (x1) {
	    if (x2) 
		return ONE;	/* 0.0 ** 0.0 == 1.0 !!!! - jpc */
	    else if (isNEG(w2)) {	/* 0.0 ** -x == <inf> !!!! -jpc */
		z.value = _fp_error ((UInt32) (FP_pow | OVERFLOW));
		errno = EDOM;		/* ANSI requires domain error */
		return z.value;
	    }
	    else z.value = ZERO;
	}
	else
	    z.value = ONE;
    }
    else if ((x1 = isINFINITY(u1)) | (x2 = isINFINITY(w2))) {
	if (x1) {		/* x_arg an infinity? */
	    if (isNEG(w2))
		z.value = ZERO;
	    else {
		if (isNEG(u1)) {
		    if (x2) 		/* Was the call pow(-inf., +inf.)? */
			p = EXCEPT;
		    else {		/* Call was pow(-inf. +y_arg) */
	 		if ( ! isINTEGER(w2.value, &OddInt))
			    p = EXCEPT;
			if (OddInt)
			    z.half.hi |= 0x80000000;    /* return -inf. */
		    }
		}
	    }
	}
	else {			/* y_arg is an infinity, x_arg is not */
	    if (isNEG(u1)) {
		if (x_arg == -ONE) {
		    p = EXCEPT;
		    z.value = ONE;
		}
		else {
		    x2 = isNEG(w2);
		    if ((x2 && x_arg > -ONE) || ((! x2) && x_arg < -ONE))
			p = EXCEPT;
		    else
			z.value = ZERO;
		}
	    }
	    else {
		x2 = isNEG(w2);
		if (x_arg == ONE)
		    z.value = x_arg;
		else if ((x2 && x_arg > ONE) || ((! x2) && x_arg < ONE))
		    z.value = ZERO;
		else if ((!x2) && (x_arg > ONE))
		    return _fp_error ((UInt32) (FP_pow | OVERFLOW));
	    }
	}
    }
 /*    Can not quickly determine the function value.
  *    Have to go through the complete computation.		*/
    else
	p = CONTIN;


    if ( ! (p == CONTIN)) {
	if (p == EXCEPT)
	    (void) _fp_error((UInt32) (FP_pow | OPERROR));
	return(z.value);
    }



 /* If x_arg is negative then y_arg must be an integer
          otherwise there is an exception.		*/
    if (isNEG(u1)) {			/* is x_arg negative? */
	if ( ! isINTEGER(w2.value, &OddInt))	/* Bad news if not an integer */
	    return( _fp_error((UInt32) (FP_pow | OPERROR)) );

	u1.half.hi &= 0x7FFFFFFF;		/* abs(x) */
    }
    else
	OddInt = FALSE;


 /* Is x_arg equal to 1.0? */
    if (( ! u1.half.lo)  &&  (u1.half.hi == 0x3FF00000)) {
	z.value = ONE;			/* Short-circuit the computation */
	goto FixSign;
    }

 /*  If y_arg is convertable to an exact integral compute by simply multiplying. */
    {
    double runningProd;
    unsigned long mask_hi, mask_lo;
    int dummy;
    int negative_flag;
    unsigned long int_hi, int_lo;

	if (isINTEGER (y_arg, &dummy) && 	/* Is y an integer? */
		(fabs (y_arg) <= MAX_EXACT_INT)) {

	    negative_flag =			/* get integer value of y_arg */
		_get_integer (y_arg, &int_hi, &int_lo);

	    mask_hi = 0x80000000;		/* set mask */
	    mask_lo = 0x0;
	    runningProd = 1.;
	    while (mask_hi || mask_lo) {
		if (runningProd != 1.)
		    runningProd *= runningProd;
		if ((mask_hi & int_hi) || (mask_lo & int_lo)) {
		    if (runningProd == 1.) {
			runningProd = x_arg;
		    }
		    else {
			runningProd *= x_arg;
		    }
		}
	    mask_lo >>= 1;
	    mask_lo |= (mask_hi << 31);
	    mask_hi >>=1;

	    }
	    if (runningProd == HUGE_VAL)	/* check for overflow */
		runningProd = _fp_error((UInt32) (FP_pow | OVERFLOW));
	    else if (runningProd == -HUGE_VAL)
		runningProd = - _fp_error((UInt32) (FP_pow | OVERFLOW));
						/* return other values */
	    return negative_flag ? (1./runningProd) : runningProd;
	}
    }

 /* Decompose the first argument into a power of two and a fraction */
    g.value = frexp(u1.value, &intgr);
    m = (long) intgr;

 /* 6 */
    p = 1;                      /* binary search of multiple of 1/16 */
    if (g.value <= a1[9])    p = 9;
    if (g.value <= a1[(int)p+4])  p += 4;
    if (g.value <= a1[(int)p+2])  p += 2;

 /* 7 */					/* z = (g-a)/(g+a) */
    z.value = ((g.value - a1[(int)p + 1]) - a2[((int)p + 1) >> 1]) /	
   	       (g.value + a1[(int)p + 1]);
    z.half.hi += 0x100000;			/* z = 2.0 * z */

 /* 8 */
    v = z.value * z.value;              /* calculate log2(x) */
    r = (((P4 * v + P3) * v + P2) * v + P1) * v * z.value;
    r += K * r;
    u2 = (r + z.value * K) + z.value;

 /* 9 */		/* FLOAT((1.0 * m - 0.0) * 16 - p) */
    u1.value = (double) ((m << 4) - p);		/* u1 + u2 = log2(x) */
    u1.half.hi -= 0x400000;		/* Multiply by .0625, divide by 16 */


 /* The next computation is that of y*log2(x).  This may underflow or
  overflow so turn off trapping while the computation takes place.  The
  error bits will be checked later to see if anything went wrong.	*/
    x1 = _get_fp_status();		/* Save the error bits */
    x2 = _get_fp_control();		/* Save the control bits */
    _clear_fp_status();			/* Clear the error flags */
    _set_fp_control(NOTRAP);		/* Turn all trapping off */

    y1 = reduce(y_arg);
    y2 = y_arg - y1;
    w = u2 * y_arg + u1.value * y2;
    w1 = reduce(w);
    w2.value = w - w1;
    w = w1 + u1.value * y1;
    w1 = reduce(w);
    w2.value += w - w1;
    w = reduce(w2.value);
    w2.value -= w;
    iw1 = (long) (16.0 * reduce(w1 + w));

 /* INVALID OPERATION will be signaled if the value was too large to
  be stored as a long.  In that case the computation can not continue.
  Determine UNDERFLOW or OVERFLOW of the pow computation then return.	*/
    if (_get_fp_status() & OPERROR) {
	u1.value = x_arg;	w2.value = y_arg;
	u1.half.hi &= 0x7FFFFFFF;		/* u1 = abs(x_arg) */
	if ((isNEG(w2) && u1.value < ONE) || ((! isNEG(w2)) && u1.value > ONE))
	    z.value = _fp_error((UInt32) (FP_pow | OVERFLOW));
	else
	    z.value = _fp_error((UInt32) (FP_pow | UNDERFLOW), 100, ZERO, 1);
		/*	Set the shift_count value to 100 so that there is
		    no doubt the value underflows to 0.0.  The round/sticky
		    variable is non-zero so that the resulting value will
		    be taken to be inexact.
		*/

	goto FixSign;			/* Make sure the sign is correct */
    }

 /* 13 */
    if ( ! isNEG(w2)) {       /* w2 must be <= 0 */
	++iw1;
	w2.value -= ONE_SIXTEENTH;
    }
    m = (iw1 >> 4) + 1;             /* new m */
    p = (m << 4) - iw1;             /* new p */

 /* 14 */                 /* perform 2^(y*log(x)) */
    z.value = ((((((Q7 * w2.value + Q6) * w2.value + Q5) * w2.value + Q4) *
	      w2.value + Q3) * w2.value + Q2) * w2.value + Q1) * w2.value;
 /* 15 */
    z.value = a1[(int)p + 1] + a1[(int)p + 1] * z.value;

 /* Don't worry about the sign of z, it must be zero */
    p = m + (z.half.hi >> 20);	/* Biased exponent of the result */


    _set_fp_control( (int) x2);	/* Restore the old control bits */
    x2 = _get_fp_status();	/* Save any generated error bits */
    _set_fp_status( (int) x1);	/* Restore the old error bits */

 /* Error bit other than INEXACT set or 'p' out-of-range? */
    if ((x2 & ~INEXACT)  ||  p < 1  ||  p > 0x7FE) {
	if (x2 & UNDERFLOW  ||  p < 1)
	    z.value = _fp_error((UInt32) (FP_pow | UNDERFLOW),
				(int) (1 - p), z.value, 1);
	else if (x2 & OVERFLOW  ||  p > 0x7FE)
	    z.value = _fp_error((UInt32) (FP_pow | OVERFLOW));
	else
	    z.value = _fp_error((UInt32) (FP_pow | x2));
    }
    else {					/* The result was computable */
	if (x2 & INEXACT) {
    	    _set_fp_status( (int) (x1 | INEXACT));
	    if (_get_fp_control() & INEXACT)
		z.value = _fp_error((UInt32) (FP_pow | INEXACT));
	}
	z.half.hi += m << 20;		/* adjust exponent */
    }

 FixSign:
 /* 'OddInt' is set if the result is negative */
    if (OddInt)
	z.half.hi |= 0x80000000;
    return(z.value);
}  /* pow() */

#endif /* EXCLUDE_m_pow */


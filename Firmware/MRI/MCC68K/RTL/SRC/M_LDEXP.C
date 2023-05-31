#include "lib_top.h"


#if EXCLUDE_m_ldexp || _80960SB || _80960KB || _80960MC \
	|| (_MCC86 && (! _FPU)) \
	|| (__OPTION_VALUE ("-Kq"))
    #pragma option -Qws			/* Prevent compiler warning */
#else

/*
 * @(SUBID) 05/10/88 /src2/nls/libraries/libm/s.ldexp.c 1.17
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

#include "m_incl.h"
#include "op_reent.h"

 /*--------------------------------------------------------------------
     AUTHOR: Michael Fluegge		05/10/88

     ldexp() takes two arguments, one is a double and the other is an
   int.  The result is the double multiplied by 2 raised to the int
   power.  Underflow or Overflow may occur.			*/
/*
Change history:
	11/03/93 - jpc  Converted "INTEL" and "MOTOROLA" to "_LITTLE_ENDIAN"
			and _BIG_ENDIAN, respectively; removed special handling
			of NaN's for little-endian machines -- all NaN's are
			handled the same now.
*/

 /* These are the largest (smallest) values for the 'exp' variable where
  overflow (underflow) will not occur for all values of the double argument. */
#define  MAXEXP		2097
#define  MINEXP		-2098

double
ldexp(arg, exp)
double	arg;
int	exp;
{
    STATIC register UInt32	sgn;
    STATIC register int	ResultExp;
    STATIC realnumber		num;

    num.value = arg;

    if (isNAN(num)) {
	return( _fp_error((UInt32) (FP_ldexp | SIGNAN), num.value) );
    }

 /* Simply return infinities and zeroes. */
    if (ExpAll1sp(num) || isZERO(num))
	return(arg);

 /* If the exponent is very large or very small the exponent bits can not
  prevent the overflow or underflow.  Check for such an exception and set
  the ResultExp to a sufficiently large or small value. */
    if (exp > MAXEXP)
	ResultExp = MAXEXP;
    else if (exp < MINEXP)
    	ResultExp = MINEXP;
    else {

 /* Compute the new biased exponent value */
	if (isDENORM(num)) {
	    int	Exp;

	    num.value = frexp(num.value, &Exp);
 /* Set ResultExp to the biased exponent.  Note that one less than
  the true exponenent is used since frexp() returns a value with an
  unbiased exponent of -1.		*/
	    ResultExp = Exp + EXP_BIAS_MNS1;
	}
	else
	    ResultExp = (num.half.hi & 0x7FF00000) >> 20;

	ResultExp += exp;		/* Final result expon. value */
    }

    sgn = num.half.hi & 0x80000000;	/* Save the sign bit */
    num.half.hi &= 0x000FFFFF;		/* Remove old expon. bits */

    if (ResultExp > 0x7FE)		/* Overflow? */
	num.value = _fp_error((UInt32) (FP_ldexp | OVERFLOW));
    else if (ResultExp <= 0)		/* Underflow? */
	num.value = _fp_error((UInt32) (FP_ldexp | UNDERFLOW),
			     (int) (1 - ResultExp), num.value, 0);
    else
 /* Place new exp. in value */
	num.half.hi |= (long) ResultExp << 20;

    num.half.hi |= sgn;
    return(num.value);
}  /* ldexp() */

#endif /* EXCLUDE_m_ldexp */


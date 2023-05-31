#include "lib_top.h"


#if EXCLUDE_m_ldexpf || _80960SB || _80960KB || _80960MC \
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

#include "m_incl.h"
#include "op_reent.h"

 /*--------------------------------------------------------------------
     AUTHOR: Michael Fluegge		07/01/87

     ldexpf() takes two arguments, one is a float and the other is an
   int.  The result is the float multiplied by 2 raised to the int
   power.  Underflow or Overflow may occur.			*/

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

 /* These are the largest (smallest) values for the 'exp' variable where
  overflow (underflow) will not occur for all values of the float argument. */
#define  MINEXP		-277
#define  MAXEXP		276

double ldexp (double arg, int exp)
{
    STATIC realnumberF		num;
    STATIC register UInt32	sgn;
    STATIC register int	ResultExp;

    num.value = arg;

    if (isNANf(num))
	return( _fp_errorf((UInt32) (FP_ldexpf | SIGNAN), num.ivalue) );

 /* Simply return infinities and zeroes. */
    if (ExpAll1sFp(num) || isZEROf(num))
	return(num.value);

    if (exp > MAXEXP)
	ResultExp = MAXEXP;
    else if (exp < MINEXP)
	ResultExp = MINEXP;
    else {

 /* Compute the normalized exponent value */
	if (isDENORMf(num)) {
	    int	Exp;

	    num.value = frexp(num.value, &Exp);
 /* Set ResultExp to the biased exponent.  Note that one less than
  the true bias is used since frexp() returns a value with an
  unbiased exponent of -1.		*/
	    ResultExp = Exp + EXP_BIAS_MNS1_f;
	}
	else
	    ResultExp = (num.ivalue & 0x7F800000) >> 23;

	ResultExp += exp;		/* Final result expon. value */
    }

    sgn = num.ivalue & 0x80000000;	/* Save the sign bit */
    num.ivalue &= 0x007FFFFF;		/* Remove old expon. bits */

    if (ResultExp > 0xFE)		/* Overflow? */
	num.value = _fp_errorf((UInt32) (FP_ldexpf | OVERFLOW));
    else if (ResultExp <= 0)		/* Underflow? */
	num.value = _fp_errorf((UInt32) (FP_ldexpf | UNDERFLOW),
			      (int) (1 - ResultExp), num.ivalue, 0);
    else
 /* Place new exp. in value */
	num.ivalue |= (long)ResultExp << 23;
     
    num.ivalue |= sgn;		/* Restore the sign bit */
    return(num.value);
}  /* ldexpf() */
 
#endif /* EXCLUDE_m_ldexpf */
 

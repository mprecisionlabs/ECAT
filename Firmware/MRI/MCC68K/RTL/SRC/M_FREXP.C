#include "lib_top.h"


#if EXCLUDE_m_frexp || _80960SB || _80960KB || _80960MC \
	|| (_MCC86 && (! _FPU)) \
	|| (__OPTION_VALUE ("-Kq"))
    #pragma option -Qws			/* Prevent compiler warning */
#else

/*
 * @(SUBID) 05/10/88 /src2/nls/libraries/libm/s.frexp.c 1.14
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

#if CPLUSPLUS_NAME_CONVERT
    #include "m_nf_fix.h"
#endif

#include "m_incl.h"
#include "op_reent.h"

 /*--------------------------------------------------------------------
     AUTHOR: Michael Fluegge		05/10/88

     frexp() takes two arguments, one is a float and the other is a ptr
   to an int.  The float argument (arg) is used to create a fractional number
   (return value) and a power of two (*PwrOf2).  Two raised to the *PwrOf2
   power times the return value is equal to the argument.  An exception
   occurs if arg is an infinity.			*/
/*
Change history:
	11/03/93 - jpc  Converted "INTEL" and "MOTOROLA" to "_LITTLE_ENDIAN"
			and _BIG_ENDIAN, respectively; removed special handling
			of NaN's for little-endian machines -- all NaN's are
			handled the same now.
*/


double frexp(double arg, int *PwrOf2)
{
    STATIC realnumber	D;

    D.value = arg;
    *PwrOf2 = 0;

    if (isNAN(D)) {
	return( _fp_error((UInt32) (FP_frexp | SIGNAN), D.value) );
    }

    if (isZERO(D))
	return(D.value);

 /* There is an exception if the number is an infinity */
    if (isINFINITY(D))
	return( _fp_error((UInt32) (FP_frexp | OPERROR)) );


    *PwrOf2 = ((D.half.hi & 0x7FF00000) >> 20) - EXP_BIAS_MNS1;

    if (isDENORM(D)) {
	register Int32	tmp;
	register int	shftcnt;
	register UInt32	sign;

 /* Save the sign so the computation can progress more easily */
	D.half.hi ^= (sign = (D.half.hi & 0x80000000));

 /* The power is off by one since the number is a denormal, fix it */
	(*PwrOf2)++;    

 /*   Locate the left-most 1 bit.  This bit must be moved to the implicit
  *  leading bit.  Thus it will be removed from the representation and
  *  the trailing bits moved left to their new positions.  The exponent
  *  variable, 'PwrOf2', must be adjusted by the number of bits shifted.    */
    	if (D.half.hi & 0x0FFFFF) {	/* A 1 bit in high word? */

 /* Locate the first 1-bit */
	    for (tmp = D.half.hi << 12, shftcnt = 1;  
	               tmp > 0;
			     ++shftcnt, tmp <<= 1)
	        ;
 /* Discard leading zeroes of the significand and the first 1-bit */
	    D.half.hi = (shftcnt >= 32) ? 0 : D.half.hi << shftcnt;
	    *PwrOf2 -= shftcnt;
        }
        else {			/* left-most 1 bit is in the low word */
 /* Locate the first 1-bit */
	    for (tmp = D.half.lo, shftcnt = 1;
		       tmp > 0;
			     ++shftcnt, tmp <<= 1)
	        ;
 /* Discard leading zeroes and the first 1-bit */
	    D.half.lo = (shftcnt >= 32) ? 0 : D.half.lo << shftcnt;
	    *PwrOf2 -= shftcnt + 20;
 /* A shftcnt of 20 accounts for all the 0 sig. bits of the high word */
	    shftcnt = 20;
        }
 /* Move bits of the low word to the high word and replace the sign bit */
	D.half.hi |= sign | (D.half.lo >> (32 - shftcnt));
	D.half.lo = (shftcnt >= 32) ? 0 : D.half.lo << shftcnt;
    }							/* Place zeroes in */
							/* the bottom of lo */
    D.half.hi &= 0x800FFFFF;		/* Use an unbiased exp. of -1 */
    D.half.hi |= (Int32) EXP_BIAS_MNS1 << 20;

    return(D.value);
}  /* frexp() */

#endif /* EXCLUDE_m_frexp */


#include "lib_top.h"


#if EXCLUDE_m_sqrtf || _80960SB || _80960KB || _80960MC || (_MCCG32 && _FPU) \
	|| (_MCC68K && _FPU) \
	|| (_MCC86 && _FPU) \
	|| (!__OPTION_VALUE ("-Kq"))
    #pragma option -Qws			/* Prevent compiler warning */
#else

/*
 * ====================================================
 * Copyright (C) 1992 by Sun Microsystems, Inc.
 *
 * Developed at SunPro, a Sun Microsystems, Inc. business.
 * Permission to use, copy, modify, and distribute this
 * software is freely granted, provided that this notice 
 * is preserved.
 * ====================================================
 *
 * Please forward all questions and comments to Microtec Research Inc.
 *
 */

/************************************************************************/
/*									*/
/*	Microtec Research revision history:				*/
/*									*/
/*	01/14/93 jpc -	Extensive mods for compatibility with Microtec.	*/
/*									*/
/************************************************************************/
/*	%W% %G%	*/

/* sqrt(x)
 * Return correctly rounded sqrt.
 * Method: 
 *   Bit by bit method using integer arithmetic. (Slow, but portable) 
 *   1. Normalization
 *	Scale x to y in [1,4) with even powers of 2: 
 *	find an integer k such that  1 <= (y=x*2^(2k)) < 4, then
 *		sqrt(x) = 2^k * sqrt(y)
 *   2. Bit by bit computation
 *	Let q  = sqrt(y) truncated to i bit after binary point (q = 1),
 *	     i							 0
 *                                     i+1         2
 *	    s  = 2*q , and	y  =  2   * ( y - q  ).		(1)
 *	     i      i            i                 i
 *                                                        
 *	To compute q    from q , one checks whether 
 *		    i+1       i                       
 *
 *			      -(i+1) 2
 *			(q + 2      ) <= y.			(2)
 *     			  i
 *							      -(i+1)
 *	If (2) is false, then q   = q ; otherwise q   = q  + 2      .
 *		 	       i+1   i             i+1   i
 *
 *	With some algebric manipulation, it is not difficult to see
 *	that (2) is equivalent to 
 *                             -(i+1)
 *			s  +  2       <= y			(3)
 *			 i                i
 *
 *	The advantage of (3) is that s  and y  can be computed by 
 *				      i      i
 *	the following recurrence formula:
 *	    if (3) is false
 *
 *	    s     =  s  ,	y    = y   ;			(4)
 *	     i+1      i		 i+1    i
 *
 *	    otherwise,
 *                         -i                     -(i+1)
 *	    s	  =  s  + 2  ,  y    = y  -  s  - 2  		(5)
 *           i+1      i          i+1    i     i
 *				
 *	One may easily use induction to prove (4) and (5). 
 *	Note. Since the left hand side of (3) contain only i+2 bits,
 *	      it does not necessary to do a full (53-bit) comparison 
 *	      in (3).
 *   3. Final rounding
 *	After generating the 53 bits result, we compute one more bit.
 *	Together with the remainder, we can decide whether the
 *	result is exact, bigger than 1/2ulp, or less than 1/2ulp
 *	(it will never equal to 1/2ulp).
 *	The rounding mode can be detected by checking whether
 *	huge + tiny is equal to huge, and whether huge - tiny is
 *	equal to huge for some floating point number "huge" and "tiny".
 *		
 * Special cases:
 *	sqrt(+-0) = +-0 	... exact
 *	sqrt(inf) = inf
 *	sqrt(-ve) = NaN		... with invalid signal
 *	sqrt(NaN) = NaN		... with invalid signal for signaling NaN
 *---------------
 */

/*
NOTE:
	This routine is designed to be used only when the -Kq switch is turned
on. When this is done, doubles are treated as though they are only 32-bit long. 
And as such, floats and doubles are the same.
*/

/*
Change history:
	11/12/93 - jpc	Converted to function with -Kq switch
			floats converted to doubles
*/

#include <math.h>
#include <errno.h>

#include "m_incl.h"
#include "op_macro.h"
#include "op_reent.h"

double sqrt (double input)		/* let <math.h> do renaming */
{
	STATIC register Int32 ix0;
	STATIC Int32 	sign = 0x80000000; 
	STATIC UInt32	r,t1,s1,q1;
	STATIC Int32	s0,q,m,t,i;

	ix0 = *(Int32*)&input;			/* high word of input */

	if ((ix0 & M_7F800000) == M_7F800000) {	/* inf & nan */
	    if (ix0 & M_007FFFFF) {		/* nan */
		return( _fp_errorf((UInt32) (FP_sqrt | SIGNAN), ix0));
	    }
	    else if (ix0 & M_80000000) {	/* negative infinity */
		return( _fp_errorf((UInt32) (FP_sqrt | OPERROR)));
	    }
	    else				/* positive infinity */
		return input;
	}

	/* Check the special cases */

	if ((ix0 & M_7FFFFFFF) == 0)		/* +-0. */
	    return input;

	if (ix0 < 0)				/* negative argument */
	    return( _fp_errorf((UInt32) (FP_sqrt | OPERROR)));

    /* normalize input */
	m = (ix0 >> 23);			/* isolate exponent */
	if(m == 0) {				/* subnormal input */
	    for(i=0; (ix0 & M_00800000) == 0; i++)	/* shift until we get */
		ix0 <<= 1;				/* correct alignment */
	    m -= i-1;
	}

	m -= 0x7f;				/* unbias exponent */
	ix0 = (ix0 & M_007FFFFF) | M_00800000;	/* make invisible bit visible */

	if(m & 1){		/* odd m, double input to make it even */
	    ix0 <<= 1;
	}
	m >>= 1;		/* m = [m/2] */

    /* generate sqrt(input) bit by bit */
	ix0 <<= 1;
	q = s0 = 0;		/* [q] = sqrt(input) */
	r = M_01000000;		/* r = moving bit from right to left */

	while(r != 0) {
	    t = s0 + r; 
	    if(t <= ix0) { 
		s0   = t + r; 
		ix0 -= t; 
		q   += r; 
	    } 
	    ix0 <<= 1;
	    r >>= 1;
	}

    /* Round if necessary */
    /* Round to nearest is assumed.  If other modes are needed, see original */
    /* code. */

	if(ix0 != 0) ++q;

    /* Now build the result, complete with exponent */

	ix0 = (q>>1) + M_3F000000;
	ix0 += (m << 23);
	*(Int32 *)&input = ix0;		/* Place back into float */

	return input;
}

#endif	/* EXCLUDE_m_sqrtf */

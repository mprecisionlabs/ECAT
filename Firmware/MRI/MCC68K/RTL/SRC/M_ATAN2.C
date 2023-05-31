#include "lib_top.h"


#if EXCLUDE_m_atan2 || _80960SB || _80960KB || _80960MC || \
	(__OPTION_VALUE ("-Kq"))
    #pragma option -Qws			/* Prevent compiler warning */
#else

/*
 * @(SUBID) 05/10/88 /src2/nls/libraries/libm/s.atan.c 1.17
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
    DOUBLE PRECISION ARCTANGENT 2

    Author: Mark McDowell
    Date:   November 27, 1984
    Source: Software Manual for the Elementary Functions
	    by William James Cody, Jr. and William Waite (1980)

    This routine will work only for numbers in IEEE double precision
    floating point format. Of the numbers in this format, it checks for 
    and handles only normalized numbers and -0 and +0 -- not infinities, 
    denormalized numbers, nor NaN's.
*/
 /*--------------------------------------------------------------------
     Revised by Michael Fluegge		05/10/88

       This routine required extensive additions.  Checks were added
     for arguments which are NaNs and infinities.  NaN arguments are
     changed to be non-signaling.  Infinities as arguments produce a
     result unless both of the arguments are infinity.  More code was
     needed to allow for arguments which were denormals.  Denormals
     require extra processing to determine when underflow or overflow
     might occur.	*/

/*
Change history:
	11/15/93 - jpc	This routine split from m_atan.c into seperate module.
	11/03/93 - jpc  Converted "INTEL" and "MOTOROLA" to "_LITTLE_ENDIAN"
			and _BIG_ENDIAN, respectively; removed special handling
			of NaN's for little-endian machines -- all NaN's are
			handled the same now.
*/

#if CPLUSPLUS_NAME_CONVERT
    #include "m_nf_fix.h"
#endif

#include "m_incl.h"
#include "op_reent.h"

#define PI               (*((const double *) hex_pi))
#define PI_OVER_2        (*((const double *) hex_pi_over_2))

static const UInt32
	hex_pi[2] =              HD_INIT(0x400921FB,0x54442D18),
	hex_pi_over_2[2] =       HD_INIT(0x3FF921FB,0x54442D18);

double
atan2(arg1, arg2)
double	arg1, arg2;
{
    realnumber		v, u;
    register Int32	x1, x2;
    register int	status;
    int			temp;

#define  CONTIN  1
#define  RETURN  2
#define  EXCEPT  3

    v.value = arg1;
    u.value = arg2;

 /*    Assume that the value is special and that atan2() can be 
  *  determined without the normal computation	*/
    status = RETURN;

    if ((x1 = isNAN(v)) || isNAN(u)) {
	v.value = _fp_error((UInt32) (FP_atan2 | SIGNAN),
				(x1) ? v.value : u.value);
    }
 /*    Check for zeros and denormals.  If denormals are not valid
  *  arguments then treat them as if they were zero.		*/
    else if (
 /* Notice that the logical OR is not used in the 'if' expression, this
  is because we do not want the boolean expression to short circuit.  Both
  x1 and x2 should be assigned a value not only x1 in the case where the
  first operand is zero.		*/
#if	DeNormsOK
	       (x1 = isZERO(v)) | (x2 = isZERO(u))
#else
             (x1 = ExpZerop(v)) | (x2 = ExpZerop(u))
#endif
	                                            ) {
 /* If both are zero then signal an exception */
	if (x1 && x2) {
	    v.value = ZERO;
	    status  = EXCEPT;
	}
	else if (x1) {
 /* Return a value in the range [-PI, PI] where the sign indicates
  which quadrant the plot of the two arguments falls in. */
	    u.value = (isNEG(u)) ? (PI) : (ZERO);
	    if (isNEG(v))
		u.half.hi |= 0x80000000;
	    v.value = u.value;
	}
	else
	    v.value = (isNEG(v)) ? -PI_OVER_2 : PI_OVER_2;
    }
 /* If infinity then signal an exception only if both arguments are infinity */
    else if ((x1 = isINFINITY(v)) | (x2 = isINFINITY(u))) {
	if (x1 && x2) {
	    v.value = anNAN;
	    status  = EXCEPT;
	}
 /* Only one argument is infinity, return a result */
	else if (x1)
	    v.value = (isPOS_INFINITY(v)) ? PI_OVER_2 : -PI_OVER_2;
	else {
	    u.value = (isPOS_INFINITY(u)) ? (ZERO) : (PI);
	    if (isNEG(v))
		u.half.hi |= 0x80000000;
	    v.value = u.value;
	}
    }
 /*    Can not quickly determine the function value.
  *    Have to go through the complete computation.		*/
    else
	status = CONTIN;


    if ( ! (status == CONTIN)) {
	if (status == EXCEPT)
	    u.value = _fp_error((UInt32) (FP_atan2 | OPERROR));
	    		/* Ignore the returned value */

	return(v.value);
    }


/* The following code sequence is to determine if arg1/arg2 will
   underflow or overflow. It basically decomposes the arguments
   into mantissas and exponents and performs the division in such
   a way as to anticipate an exception condition before causing a
   trap.                                                           */

#if	DeNormsOK
 /*    Decompose into mantissa and exponents by using frexp() */
    v.value = frexp(v.value, &temp);
    x2 = temp;

    u.value = frexp(u.value, &temp);
    x1 = temp;
#else
    x1 = x2 = 0x7FF00000;
    v.half.hi ^= x2 &= v.half.hi;	/* isolate exponents */
    u.half.hi ^= x1 &= u.half.hi;
    v.half.hi |= 0x3FF00000;		/* set exponents to 3FF */
    u.half.hi |= 0x3FF00000;
    x1 = (x1 >> 20) - EXP_BIAS;		/* Compute the unbiased exponents */
    x2 = (x2 >> 20) - EXP_BIAS;
#endif

 /* Form the difference of the two exponents */
    x2 -= x1;
    v.value /= u.value;			/* "divide mantissas" */
    v.half.hi &= 0x7FFFFFFF;		/* v = abs(v) */
    x1 = x2 + ((v.half.hi & 0x7FF00000) >> 20);

 /* Are we safe?  No exception? */
    if (((UInt32) x1 < 0x7FF) && x1) {
	v.half.hi += (x2 << 20);	/* We are safe, make v = arg1/arg2 */
	v.value = atan(v.value);        /* calculate atan(arg1/arg2) */
	if (u.half.hi & 0x80000000)
	    v.value = PI - v.value;     /* adjust as needed */
    } 
 /* Can not compute value by calling atan().  There 
  was underflow or overflow on the division.	 */
    else
	if (x2 >= 0)			/* Overflow? */
	    v.value = PI_OVER_2;
	else				/* Underflow */
	    v.value = (u.half.hi & 0x80000000)
			? PI
			: _fp_error((UInt32) (FP_atan2 | UNDERFLOW),
				   (int) (1 - x1), v.value, 0);

    if (arg1 < ZERO)
	v.half.hi ^= 0x80000000;	/* Adjust sign */

    return(v.value);
}  /* atan2() */

#endif /* EXCLUDE_m_atan2 */


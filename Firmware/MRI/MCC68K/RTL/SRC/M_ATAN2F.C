#include "lib_top.h"


#if EXCLUDE_m_atanf || _80960SB || _80960KB || _80960MC \
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
    SINGLE PRECISION ARCTANGENT 2

    Author: Mark McDowell
    Date:   December 20, 1984
    Source: Software Manual for the Elementary Functions
	by William James Cody, Jr. and William Waite (1980)

    This routine will work only for numbers in IEEE single precision
    floating point format. Of the numbers in this format, it checks for 
    and handles only normalized numbers and -0 and +0 -- not infinities, 
    denormalized numbers, nor NaN's.

 */

 /*--------------------------------------------------------------------
     Revised by Michael Fluegge		07/13/87
   
       This routine required extensive additions.  Checks were added
     for arguments which are NaNs and infinities.  NaN arguments are
     changed to be non-signaling.  Infinities as arguments produce a
     result unless both of the arguments are infinity.  More code was
     needed to allow for arguments which were denormals.  Denormals
     require extra processing to determine when underflow or overflow
     might occur.	*/

/*
NOTE:
	This routine is designed to be used only when the -Kq switch is turned
on. When this is done, doubles are treated as though they are only 32-bit long. 
And as such, floats and doubles are the same.
*/

/*
Change history:
	11/15/93 - jpc	Split atan2f out into seperate module.
	11/03/93 - jpc  Converted "INTEL" and "MOTOROLA" to "_LITTLE_ENDIAN"
			and _BIG_ENDIAN, respectively; removed special handling
			of NaN's for little-endian machines -- all NaN's are
			handled the same now.
	11/12/93 - jpc	Converted to function with -Kq switch
			floats converted to doubles
*/

#include "m_incl.h"
#include "op_reent.h"

#define PI               (*((const double *) &hex_pi))
#define PI_OVER_2        (*((const double *) &hex_pi_over_2))

static const UInt32	hex_pi =           0x40490FDB,
			hex_pi_over_2 =    0x3FC90FDB;


double atan2(double arg1, double arg2)
{
    STATIC realnumberF      v, u;
    STATIC register Int32   f1, f2;
    STATIC register UInt32  status;

#define  CONTIN  1
#define  RETURN  2
#define  EXCEPT  3

    v.value = arg1;
    u.value = arg2;

 /*    Assume that the value is special and that atan2f() can be 
  *  determined without the normal computation	*/
    status = RETURN;

 /* If there is an NaN then return the offending argument */
    if ((f1 = isNANf(v)) || isNANf(u))
	v.value = _fp_errorf((UInt32) (FP_atan2f | SIGNAN),
			    (f1) ? v.ivalue : u.ivalue);

 /*    Check for zeros and denormals.  If denormals are not valid
  *  arguments then treat them as if they were zero.		*/
 /* Notice that the logical OR is not used in the 'if' expression, this
  is because we do not want the boolean expression to short circuit.  Both
  f1 and f2 should be assigned a value not just f1 in the case where the
  first operand is zero.		*/
    else if (
#if DeNormsOK
	        (f1 = isZEROf(v)) | (f2 = isZEROf(u))
#else
              (f1 = ExpZeroFp(v)) | (f2 = ExpZeroFp(v))
#endif
	                                               ) {
 /* If both are zero then signal an exception */
	if (f1 && f2) {
	    v.value = ZEROf;
	    status  = EXCEPT;
	}
	else if (f1) {
 /* Return a value in the range (-PI, PI] where the sign indicates
  which quadrant the plot of the two arguments falls in. */
	    if (isNEGf(u))
		v.value = PI;
	    else  	/* Return +-0.0 */
		v.ivalue = (isNEGf(v)) ? 0x80000000 : 0x0;
	}
	else
	    v.value = (isNEGf(v)) ? -PI_OVER_2 : PI_OVER_2;
    }
 /* If infinity then signal an exception only if both arguments are infinity */
    else if ((f1 = isINFINITYf(v)) | (f2 = isINFINITYf(u))) {
	if (f1 && f2) {
	    v.value = anNANf;
	    status  = EXCEPT;
	}
 /* Only one argument is infinity, return a result */
	else if (f1)
	    v.value = (isPOS_INFINITYf(v)) ? PI_OVER_2 : -PI_OVER_2;
	else
	    v.value = (isPOS_INFINITYf(u)) ? ZEROf : PI;
    }
 /*    Can not quickly determine the function value.
  *    Have to go through the entire computation.		*/
    else
	status = CONTIN;


    if ( ! (status == CONTIN)) {
	if (status == EXCEPT)
	    (void) _fp_errorf((UInt32) (FP_atan2f | OPERROR));
			/* Ignore the returned value */
	 
	return(v.value);
    }

    status = v.ivalue & 0x80000000;	/* Save sign of arg1 */

    f1 = _get_fp_status();		/* Save the error bits */
    f2 = _get_fp_control();
    _clear_fp_status();		/* Clear error bits, trapping off */
    _set_fp_control(NOTRAP);

    v.value /= u.value;
    v.ivalue &= 0x7FFFFFFF;		/* v = abs(v) */

    _set_fp_control( (int) f2);		/* Restore the control bits */
    f2 = _get_fp_status();		/* Save the error indication */
    _set_fp_status( (int) f1);		/* Restore users error bits */

    if (f2 & UNDERFLOW)		/* Did underflow occur? */
	v.value = (u.ivalue & 0x80000000) ? PI
			: _fp_errorf((UInt32) (FP_atan2f | UNDERFLOW),
					100, v.ivalue, 1);
    else {
	if (INEXACT & f2) {		/* INEXACT result? */
	    _set_fp_status( (int) (f1 | INEXACT));
	    if (_get_fp_control() & INEXACT)	/* Trap inexact? */
		return( _fp_errorf((UInt32) (FP_atan2f | INEXACT)) );
	}
	v.value = atan(v.value);    	/* calculate atan(v/u) */
	if (u.ivalue & 0x80000000)
	    v.value = PI - v.value;		/* adjust as needed */
    } 
     
    if (status)			/* Sign of arg1 negative? */
	v.ivalue ^= 0x80000000;		/* adjust sign */

    return(v.value);
}  /* atan2f() */
 
#endif /* EXCLUDE_m_atanf */
 

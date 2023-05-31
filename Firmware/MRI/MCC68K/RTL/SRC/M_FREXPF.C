#include "lib_top.h"


#if EXCLUDE_m_frexpf || _80960SB || _80960KB || _80960MC \
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

     frexpf() takes two arguments, one is a float and the other is a ptr
   to an int.  The float argument (arg) is used to create a fractional number
   (return value) and a power of two (*PwrOf2).  Two raised to the *PwrOf2
   power times the return value is equal to the argument.  An exception
   occurs if arg is an infinity.			*/

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

double frexp(double arg, int * PwrOf2)
{
    STATIC realnumberF       D;

    D.value = arg;
    *PwrOf2 = 0;

    if (isNANf(D))
	return( _fp_errorf((UInt32) (FP_frexpf | SIGNAN), D.ivalue) );

    if (isZEROf(D))
	return(D.value);

 /* There is an exception if the number is an infinity */
    if (isINFINITYf(D))
	return( _fp_errorf((UInt32) (FP_frexpf | OPERROR)) );
	 
			
    *PwrOf2 = ((D.ivalue & 0x7F800000) >> 23) - EXP_BIAS_MNS1_f;

    if (isDENORMf(D)) {
	register UInt32	sign, tmp;
        register int	shftcnt;

 /* Save the sign so the computation can progress more easily */
	sign = D.ivalue & 0x80000000;

 /*   Locate the left-most 1 bit.  This bit must be moved to the implicit
  *  leading bit.  Thus it will be removed from the representation and
  *  the trailing bits moved left to their new positions.  The exponent
  *  variable, 'PwrOf2', must be adjusted by the number of bits shifted.  */

 /* Calculate the shift count */
 /*    Start the shift count at 0 instead of 1 since the power is
  *  already one too large because the number is a denormal.		*/
	for (tmp = (D.ivalue & 0x007FFFFF) << 1, shftcnt = 0;  
                   tmp < 0x00800000;
                         ++shftcnt, tmp <<= 1)
	    ;
 /* Create the new value with the shifted bits. */
	D.ivalue = (tmp & 0x007FFFFF) | sign;
	*PwrOf2 -= shftcnt;
    }
    else
    	D.ivalue &= 0x807FFFFF;		/* Discard old exponent bits */

    D.ivalue |= (long)EXP_BIAS_MNS1_f << 23;/* Use an unbiased exponent of -1 */

    return(D.value);
}  /* frexpf() */
 
#endif /* EXCLUDE_m_frexpf */
 

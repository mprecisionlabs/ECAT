#include "lib_top.h"


#if EXCLUDE_m_modff || _80960SB || _80960KB || _80960MC \
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


/* %W% %G% */

#include    "m_incl.h"
#include    "op_reent.h"

 /*--------------------------------------------------------------------
     AUTHOR: Michael Fluegge		07/02/87

     This routine decomposes its argument into the integer part of the
   number and the fractional part of the number.  The integer part is
   placed at the location pointed to by 'iptr'.  The fractional part
   is the value which is returned.
     NaNs are returned with both the integer part and fractional part
   set to the non-signaling version of the argument value.  Infinities
   cause both parts to be set to the result of _fp_errorf().		*/


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

double modf(double arg, double *iptr)
{
    STATIC realnumberF		intgr, frctn;
    STATIC register int	shftcnt;


    intgr.value = *iptr = arg;

    if (isNANf(intgr))
	return( *iptr = _fp_errorf((UInt32) (FP_modff|SIGNAN), intgr.ivalue) );

    if (isINFINITYf(intgr))
	return( *iptr = _fp_errorf((UInt32) (FP_modff | OPERROR)) );


 /* Get a feel for the size of the number.  150 == EXP_BIAS_f + # sig bits */
    shftcnt = 150 - ((intgr.ivalue >> 23) & 0xFF);  /* # bits to mask */

    if (shftcnt > 23) { 	/* Is magnitude less than 1.0? */
 /* If we were passed a +-0.0 then keep '*iptr' at
           that value, else return a +0.0 	*/
	if ( ! isZEROf(intgr))
	    *iptr = 0.0;
	return(intgr.value);
    }

    if (shftcnt < 1) 	/* Is it huge?  No binary point in mantissa? */
	return(ZEROf);

 /* 'shftcnt' tells us where the first fractional bit is located */
    intgr.ivalue &= ALLONES << shftcnt;	/* Discard fraction bits */
    *iptr = intgr.value;

    return( arg - intgr.value );	/* Return fractional part */
}  /* modff() */
 
#endif /* EXCLUDE_m_modff */
 

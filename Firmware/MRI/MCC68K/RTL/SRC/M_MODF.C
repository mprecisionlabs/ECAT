#include "lib_top.h"


#if EXCLUDE_m_modf || _80960SB || _80960KB || _80960MC \
	|| (_MCC86 && (! _FPU)) \
	|| (_MCC68K && _FPU) || (__OPTION_VALUE ("-Kq"))
    #pragma option -Qws			/* Prevent compiler warning */
#else

/*
 * @(SUBID) 05/10/88 /src2/nls/libraries/libm/s.modf.c 1.11
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

     This routine decomposes its argument into the integer part of the
   number and the fractional part of the number.  The integer part is
   placed at the location pointed to by 'iptr'.  The fractional part
   is the value which is returned.
     NaNs are returned with both the integer part and fractional part
   set to the non-signaling version of the argument value.  Infinities 
   cause both parts to be set to the result of _fp_error().		*/
/*
Change history:
	11/03/93 - jpc  Converted "INTEL" and "MOTOROLA" to "_LITTLE_ENDIAN"
			and _BIG_ENDIAN, respectively; removed special handling
			of NaN's for little-endian machines -- all NaN's are
			handled the same now.
*/



double
modf(arg, iptr)
double	arg, *iptr;
{
    STATIC register int	shftcnt;
    STATIC realnumber		intgr;


    intgr.value = *iptr = arg;

    if (isNAN(intgr)) {
	return( *iptr = _fp_error((UInt32) (FP_modf | SIGNAN), intgr.value) );
    }

    if (isINFINITY(intgr))
	return( _fp_error((UInt32) (FP_modf | OPERROR)) );


 /* Get a feel for the size of the number.  1075 == EXP_BIAS + # sig bits */
    shftcnt = 1075 - ((intgr.half.hi >> 20) & 0x7FF);	/* # bits to mask */

    if (shftcnt > 52) { 	/* Is magnitude less than 1.0? */
 /* If we were passed a +-0.0 then keep '*iptr' at
           that value, else return a +0.0 	*/
	if ( ! isZERO(intgr))
	    *iptr = ZERO;
	return(arg);
    }

    if (shftcnt < 1) 	/* Is it huge?  No binary point in mantissa? */
	return(ZERO);

 /* 'shftcnt' tells us where the first fractional bit is located */
    if (shftcnt >= 32) {	/* binary point in upper 32 bits? */
 /* Discard the fractional bits */
	intgr.half.hi &= ALLONES << (shftcnt - 32);
	intgr.half.lo = 0x0;
    }
    else           		/* binary point is in lower 32 bits */
	intgr.half.lo &= ALLONES << shftcnt;	/* Discard fraction */

    return( arg - (*iptr = intgr.value) );
}  /* modf() */

#endif /* EXCLUDE_m_modf */


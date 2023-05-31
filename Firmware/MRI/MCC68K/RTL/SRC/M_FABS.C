#include "lib_top.h"


#if EXCLUDE_m_fabs || _80960SB || _80960KB || _80960MC || (_MCCG32 && _FPU) \
	|| (_MCC68K && _FPU) \
	|| (_MCC86 && (! _FPU)) \
	|| (__OPTION_VALUE ("-Kq"))
    #pragma option -Qws			/* Prevent compiler warning */
#else
 
/*
 * @(SUBID) 05/10/88 /src2/nls/libraries/libm/s.fabs.c 1.10
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

     This routine returns the absolute value of a double argument.
   If the argument is an NaN, it is tranformed into a non-signaling
   NaN, but the sign bit is not changed.  For NaNs, the sign bit may 
   or may not be intended for use as the sign, so don't mess with it.	*/
/*
Change history:
	11/03/93 - jpc  Converted "INTEL" and "MOTOROLA" to "_LITTLE_ENDIAN"
			and _BIG_ENDIAN, respectively; removed special handling
			of NaN's for little-endian machines -- all NaN's are
			handled the same now.
*/

double
fabs(arg)
double  arg;
{
    STATIC realnumber  num;

    num.value = arg;
    if (isNAN(num)) {
	return( _fp_error((UInt32) (FP_fabs | SIGNAN), num.value) );
    }
    else {
	num.half.hi &= 0x7FFFFFFF;	/* Clear the sign bit */
	return(num.value);
    }
}  /* fabs() */
 
#endif /* EXCLUDE_m_fabs */
 

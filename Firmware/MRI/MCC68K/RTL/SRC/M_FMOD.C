#include "lib_top.h"


#if EXCLUDE_m_fmod || _80960SB || _80960KB || _80960MC || (_MCCG32 && _FPU) \
	|| (_MCC68K && _FPU) \
	|| (_MCC86 && (! _FPU)) \
	|| (__OPTION_VALUE ("-Kq"))
    #pragma option -Qws			/* Prevent compiler warning */
#else

/*
 * @(SUBID) 05/10/88 /src2/nls/libraries/libm/s.fmod.c 1.13
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

       This routine performs the float remainder operation.  Division
     of the two operands is performed to do this computation.  A first
     argument of infinity or a second argument of zero causes an 
     exception to occur.				*/
/*
Change history:
	11/03/93 - jpc  Converted "INTEL" and "MOTOROLA" to "_LITTLE_ENDIAN"
			and _BIG_ENDIAN, respectively; removed special handling
			of NaN's for little-endian machines -- all NaN's are
			handled the same now.
*/


double
fmod(Xarg, Yarg)
double	Xarg, Yarg;
{
    STATIC register Int32	flag;
    STATIC register UInt16	fperr, fpctrl;
    STATIC realnumber		x, y;

    x.value = Xarg;   y.value = Yarg;
    if ((flag = isNAN(x)) || isNAN(y)) {
	return( _fp_error((UInt32) (FP_fmod | SIGNAN),
			(flag) ? x.value : y.value) );
    }

    if (isINFINITY(x) || isZERO(y))
	return( _fp_error((UInt32) (FP_fmod | OPERROR)) );

    if (isZERO(x) || isINFINITY(y))
	return(x.value);


 /* Take the absolute value of both of the operands */
    x.half.hi &= 0x7FFFFFFF;   y.half.hi &= 0x7FFFFFFF;

 /* Return the first argument if its magnitude is less than the
  magnitude of the second argument, otherwise the computation must
  be done.						*/
    if (x.value < y.value)
	return(Xarg);


    fperr  = _get_fp_status();		/* Save the accumulated error bits */
    fpctrl = _get_fp_control();		/* Save the user's ctrl info */
    _clear_fp_status();			/* Trapping off; clear error bits */
    _set_fp_control(NOTRAP);

 /* Break Xarg/Yarg into integer and fractional portions.  Discard fraction. */
    (void) modf(Xarg / Yarg, &y.value);

 /* The only error that matters is OVERFLOW.  If OVERFLOW occured then
  report a PLOSS error since the float remainder is non-computable.	*/
    if (_get_fp_status() & OVERFLOW)
	return( _fp_error((UInt32) (FP_fmod | PLOSS)) );

    y.value *= Yarg;

    _clear_fp_status();		/* Clear the error flags */

    x.value = Xarg - y.value;	/* Compute the final answer */

    flag = _get_fp_status();	/* Save indication of error */
    _set_fp_control(fpctrl);	/* Restore user's control bits */

    if (flag & INEXACT) {
	_set_fp_status(fperr | INEXACT);
	if (fpctrl & INEXACT)		/* Trapping on? */
	    return( _fp_error((UInt32) (FP_fmod | INEXACT)) );
    }
    else
    	_set_fp_status(fperr);		/* Restore user's error bits */	

    return(x.value);
}  /* fmod() */

#endif /* EXCLUDE_m_fmod */


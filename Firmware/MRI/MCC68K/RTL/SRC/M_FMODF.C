#include "lib_top.h"


#if EXCLUDE_m_fmodf || _80960SB || _80960KB || _80960MC || (_MCCG32 && _FPU) \
	|| (_MCC68K && _FPU) \
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

       This routine performs the float remainder operation.  Division
     of the two operands is performed to do this computation.  A first
     argument of infinity or a second argument of zero causes an 
     exception to occur.				*/

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

double fmod(double Xarg, double Yarg)
{
    STATIC register Int32	flag;
    STATIC realnumberF		x, y;

    x.value = Xarg;   y.value = Yarg;
    if ((flag = isNANf(x)) || isNANf(y))
	return( _fp_errorf((UInt32) (FP_fmodf | SIGNAN),
			  (flag) ? x.ivalue : y.ivalue) );
    
    if (isINFINITYf(x) || isZEROf(y))
	return( _fp_errorf((UInt32) (FP_fmodf | OPERROR)) );

    if (isZEROf(x) || isINFINITYf(y))
	return(x.value);


 /* Take the absolute value of both of the operands */
    x.ivalue &= 0x7FFFFFFF;   y.ivalue &= 0x7FFFFFFF;

 /* Return the first argument if its magnitude is less than the
  magnitude of the second argument, otherwise the computation must
  be done.						*/
    if (x.value < y.value)
	return(Xarg);


    {
	STATIC register UInt16	fperr, fpctrl;

	fperr = _get_fp_status();	/* Save the accumulated error bits */
	fpctrl = _get_fp_control();	/* Save the user's ctrl info */
	_clear_fp_status();		/* Clear the error bits */
	_set_fp_control(NOTRAP);	/* Trapping off */

 /* Break Xarg/Yarg into its integer and fractional parts.  Use the int part. */
	(void) modf(Xarg / Yarg, &y.value);

 /* OVERFLOW is the only error which is of concern.  If OVERFLOW
  occured then report a PLOSS error since the float remainder is
  non-computable.	*/
	if (_get_fp_status() & OVERFLOW)
	    return( _fp_errorf((UInt32) (FP_fmodf | PLOSS)) );

	y.value *= Yarg;

	_clear_fp_status();		/* Clear the error flag */

	x.value = Xarg - y.value;	/* Compute the final answer */

	flag = _get_fp_status();	/* Save indication of error */
	_set_fp_control(fpctrl);	/* Restore user's control bits */

	if (flag & INEXACT) {
	    _set_fp_status(fperr | INEXACT);
	    if (fpctrl & INEXACT)		/* Trapping on? */
		return( _fp_errorf((UInt32) (FP_fmodf | INEXACT)) );
	}
	else
	    _set_fp_status(fperr);	/* Restore user's error bits */

	return(x.value);
    }
}  /* fmodf() */
 
#endif /* EXCLUDE_m_fmodf */
 

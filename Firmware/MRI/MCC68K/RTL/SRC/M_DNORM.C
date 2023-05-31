#include "lib_top.h"


#if EXCLUDE_m_dnorm || _80960SB || _80960KB || _80960MC \
	|| (__OPTION_VALUE ("-Kq"))
    #pragma option -Qws			/* Prevent compiler warning */
#else
 
/*
 * $Header: fperror.c,v 1.8 88/05/10 15:00:49 hmgr Exp $
 * @(SUBID) $Source: /src2/nls/libraries/libm/fperror.c,v $ $Revision: 1.8 $
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


/* %W% %G% */

#if CPLUSPLUS_NAME_CONVERT
    #include "m_nf_fix.h"
#endif

#include <stdarg.h>

#include "m_incl.h"
#include "op_reent.h"

void	_err_handler();

int _denormalize(realnumber *mant, int RndStcky, int shftcnt)
{
	STATIC register UInt32	round, sticky, mask, sgn;

	round = RndStcky < 0;		/* Seperate rnd and stcky */
	sticky = 0x7FFFFFFF & RndStcky;

 /* Only do the denormalization if the shftcnt is >= 1 */
	if (shftcnt < 1)
		goto Finish;

	sgn = mant->half.hi & 0x80000000;	/* Save the sign bit */

	mant->half.hi &= 0x000FFFFF;	/* Remove sign and exp bits */
	mant->half.hi |= 0x00100000;	/* Make implicit bit explicit */

     /* Set the rounding flags */
	if (shftcnt > 53) {			/* All bits shift out? */
		round = 0;			/* No round */
		sticky |= mant->half.hi | mant->half.lo;
	}
	else if (shftcnt >= 33) {		/* Shift out 'hi' bits? */
		mask = (Int32) 0x1 << (shftcnt - 33);
		round = mant->half.hi & mask;
		sticky |= mant->half.lo || (mant->half.hi & --mask);
	}
	else {			/* Only shift out 'lo' bits */
		mask = (Int32) 0x1 << (shftcnt - 1);
		round = mant->half.lo & mask;
		sticky |= mant->half.lo & --mask;
	}

     /* Shift the sig bits */
	if (shftcnt >= 53)		/* Will all sig bits be shifted? */
		mant->half.hi = mant->half.lo = 0x0;	/* Underflow to +-0 */
	else if (shftcnt >= 32) {
		mant->half.lo = mant->half.hi >> (shftcnt - 32);
		mant->half.hi = 0x0;
	}
	else {
		mant->half.lo >>= shftcnt;		/* Discard bits */
     /* Place the low 'shftcnt' bits of hi into top 'shftcnt' bits of lo */
		mant->half.lo |= mant->half.hi << (32 - shftcnt);
		mant->half.hi >>= shftcnt;
	}

     /* Do we round the value up?  Round to even on a tie. */
	if (round && (sticky || (mant->half.lo & 0x1)))
		if (++mant->half.lo == 0x0)
			++mant->half.hi;

	mant->half.hi |= sgn;		/* Replace the sign bit */


  Finish:
	if (!(mant->half.hi | mant->half.lo))	/* Set range error */
	    errno = ERANGE;			/* on underflow to zero */

/* Only signal underflow when the result is a denormal number */
	return(
		(((mant->half.hi & 0x7FFFFFFF) < 0x00100000)
			? UNDERFLOW : 0 ) |
		((round || sticky) ? INEXACT : 0)
	      );
}  /* _denormalize() */
 
#endif /* EXCLUDE_m_dnorm */
 

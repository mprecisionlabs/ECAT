#include "lib_top.h"


#if EXCLUDE_m_fdnorm || _80960SB || _80960KB || _80960MC \
	|| (_MCC86 && (! _FPU)) \
	|| (!__OPTION_VALUE ("-Kq"))
    #pragma option -Qws			/* Prevent compiler warning */
#else
 
/*
 * $Header: fperror.c,v 1.8 88/05/10 15:00:49 hmgr Exp $
 * @(SUBID) $Source: /src2/nls/libraries/libm/fperror.c,v $ $Revision: 1.8 $
 * COPYRIGHT  Hewlett-Packard Company  1987, 1988
 */
/*      Copyright 1991, 1992, Microtec Research, Inc.                   */
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

#include <stdarg.h>

#include "m_incl.h"
#include "op_reent.h"

void	_err_handler();

int _denormalizef (realnumberF *inArg, int RndStcky, int shftcnt)
{
	STATIC register UInt32	round, sticky, mask, sgn;
	STATIC realnumberF mant;

	mant.ivalue = inArg->ivalue;	/* Place arg in local space */
	round = RndStcky < 0;		/* Seperate rnd and stcky */
	sticky = 0x7FFFFFFF & RndStcky;

 /* Only do the denormalization if the shftcnt is >= 1 */
	if (shftcnt < 1)
		goto Finish;

	sgn = mant.ivalue & 0x80000000;	/* Save the sign bit */

	mant.ivalue &= 0x007FFFFF;	/* Remove sign and exp bits */
	mant.ivalue |= 0x00800000;	/* Make implicit bit explicit */

     /* Set the rounding flags */
	if (shftcnt > 24) {			/* All bits shift out? */
		round = 0;			/* No round */
		sticky |= mant.ivalue;
	}
	else {			/* Only shift out 'lo' bits */
		mask = (Int32) 0x1 << (shftcnt - 1);
		round = mant.ivalue & mask;
		sticky |= mant.ivalue & --mask;
	}

     /* Shift the sig bits */
	if (shftcnt >= 24)		/* Will all sig bits be shifted? */
		mant.ivalue = 0x0;	/* Underflow to +-0 */
	else 
		mant.ivalue >>= shftcnt;		/* Discard bits */

     /* Do we round the value up?  Round to even on a tie. */
	if (round && (sticky || (mant.ivalue & 0x1)))
		++mant.ivalue;

	mant.ivalue |= sgn;		/* Replace the sign bit */


  Finish:
	if (!mant.ivalue)			/* Set range error */
	    errno = ERANGE;			/* on underflow to zero */

	inArg->ivalue = mant.ivalue;		/* Return result */

/* Only signal underflow when the result is a denormal number */
	return(
		(((mant.ivalue & 0x7FFFFFFF) < 0x00800000)
			? UNDERFLOW : 0 ) |
		((round || sticky) ? INEXACT : 0)
	      );
}  /* _denormalize() */
 
#endif /* EXCLUDE_m_fdnorm */
 

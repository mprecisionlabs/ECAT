#include "lib_top.h"


#if EXCLUDE_m_errorf || _80960SB || _80960KB || _80960MC \
	|| (!__OPTION_VALUE ("-Kq"))
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

#include <stdarg.h>
#include "m_incl.h"
#include "op_math.h"
#include "op_reent.h"

void	_err_handler();
int 	_denormalizef(realnumberF *, int, int);

/*************************************************************************/
/*	The following routine should only be used in single precision	 */
/*	mode								 */
/*	It assumes that floting point arguments are pushed onto the 	 */
/*	stack as "float".						 */
/*************************************************************************/

float _fp_errorf(long caller_and_error, ...)
{
	STATIC va_list		arg_list;
	STATIC int		error_type;
	STATIC realnumberF	mant;

	va_start(arg_list, caller_and_error);

	if ((error_type = (int) caller_and_error) & OPERROR) {
		errno = EDOM;
		mant.value = anNANf;		/* Return value is an NaN */
		error_type = OPERROR;
	}
	else if (error_type & OVERFLOW) {
		errno = ERANGE;
		mant.value = HUGE_VAL;		/* Return value is +infinity */
		error_type = OVERFLOW | INEXACT;
	}
	else if (error_type & UNDERFLOW) {
		int	shift_count, round_sticky;

		shift_count  = va_arg(arg_list, int);
		mant.ivalue   = va_arg(arg_list, unsigned long);
		round_sticky = va_arg(arg_list, int);

		error_type = _denormalizef(&mant, round_sticky, shift_count);
	}
	else if (error_type & DIVZERO) {
		errno = ERANGE;
		mant.value = HUGE_VAL;		/* Return value is +infinity */
		error_type = DIVZERO;
	}
	else if (error_type & PLOSS) {
		mant.value = anNANf;	/* Return value is an NaN */
		error_type = PLOSS | INEXACT;
	}
	else if (error_type & INEXACT) {
		error_type = INEXACT;
	}
	else {		/* Unknown exception type */
		va_end(arg_list);

		mant.value = anNANf;	/* Return value is an NaN */

		return(float) (mant.value);
	}
	va_end(arg_list);

#if 0
	_err_handler(error_type);
#endif

/*	Depending on the value of the trapping bits,
 * the code may or may not return to this point. */

	return((float) mant.value);
}
 
#endif /* EXCLUDE_m_errorf */
 

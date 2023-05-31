#include "lib_top.h"


#if EXCLUDE_xldiv
    #pragma option -Qws			/* Prevent compiler warning */
#else
 
/****************************************************************************/
/* THIS INFORMATION IS PROPRIETARY TO                                       */
/* MICROTEC RESEARCH, INC.                                                  */
/*--------------------------------------------------------------------------*/
/* Copyright (c) 1990, 1992 Microtec Research, Inc.                         */
/* All rights reserved                                                      */
/****************************************************************************/
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

#include <stdlib.h>

#if _sun
    typedef struct ldiv_t {long quot, rem;} ldiv_t;
#endif

ldiv_t ldiv(register long numer, register long denom)
{
	ldiv_t	rslt;

	rslt.rem = numer - (rslt.quot = numer / denom) * denom;
	return(rslt);
}  /* ldiv() */
 
#endif /* EXCLUDE_xldiv */
 

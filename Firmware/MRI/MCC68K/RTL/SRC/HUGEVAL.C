#include "lib_top.h"


#if EXCLUDE_hugeval
    #pragma option -Qws			/* Prevent compiler warning */
#else

/************************************************************************/
/* THIS INFORMATION IS PROPRIETARY TO					*/
/* MICROTEC RESEARCH, INC.                                              */
/*----------------------------------------------------------------------*/
/* Copyright (c) 1993 Microtec Research, Inc.				*/
/* All rights reserved                                                  */
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

/************************************************************************/
/*									*/
/*	double _HUGEVAL ()						*/
/*									*/
/*		This routine returns IEEE infinity			*/
/*									*/
/************************************************************************/
#if CPLUSPLUS_NAME_CONVERT
    #include "m_nf_fix.h"
#endif

#include <math.h>
#include "op_macro.h"

double _HUGEVAL (void) {
    return 1.0e999;			/* generate infinity and return */
}

#endif /* EXCLUDE_infinity */


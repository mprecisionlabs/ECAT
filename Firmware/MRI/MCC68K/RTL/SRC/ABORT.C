#include "lib_top.h"


#if EXCLUDE_abort
    #pragma option -Qws			/* Prevent compiler warning */
#else
 

/************************************************************************/
/* THIS INFORMATION IS PROPRIETARY TO					*/
/* MICROTEC RESEARCH, INC.						*/
/* 2350 Mission College Blvd.						*/
/* Santa Clara, CA 95054						*/
/* USA									*/
/*----------------------------------------------------------------------*/
/* Copyright (c) Microtec Research, Inc. 1991, 1992.			*/
/* All rights reserved.							*/
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

/************************************************************************
*                                                                       *
*  void abort (void);							*	
*                                                                       *
*  Abort program with abnormal termination				*
*                                                                       *
*  abort:								*
*	Causes an abnormal termination.  This is accomplished with  	*
*	a "raise (SIGABRT)".  This function does not return.		*
*									* 
************************************************************************/

#include	<signal.h>
#include	<stdlib.h>

void abort (void)
{
    raise (SIGABRT);			/* cause unsuccessful termination */
}
 
#endif /* EXCLUDE_abort */
 

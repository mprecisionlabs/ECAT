#include "lib_top.h"


#if EXCLUDE_sigabort
    #pragma option -Qws			/* Prevent compiler warning */
#else
 
/****************************************************************************/
/* THIS INFORMATION IS PROPRIETARY TO                                       */
/* MICROTEC RESEARCH, INC.                                                  */
/*--------------------------------------------------------------------------*/
/* Copyright (c) 1992 Microtec Research, Inc.				    */
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


/************************************************************************/
/*	void _sig_abort (int var);					*/
/*		var - signal number					*/
/*									*/
/*	returns:  nothing						*/
/*									*/
/*	This routine is used by "raise ()" to execute a			*/
/*	"raise (SIGABRT)".  It is in a separate module to allow for	*/
/*	testing.							*/
/*									*/
/*  History --								*/
/*	02/26/92 jpc -  split _sig_abort to allow for testing		*/
/************************************************************************/
/*	%W% %G%	*/

#include <stdio.h>

void _sig_abort (int var)
{
    _exit_abort (1);
}
 
#endif /* EXCLUDE_sigabort */
 

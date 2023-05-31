#include "lib_top.h"


#if EXCLUDE_mriclock
    #pragma option -Qws			/* Prevent compiler warning */
#else
 
/****************************************************************************/
/* THIS INFORMATION IS PROPRIETARY TO                                       */
/* MICROTEC RESEARCH, INC.                                                  */
/*--------------------------------------------------------------------------*/
/* Copyright (c) 1991, 1992 Microtec Research, Inc.                         */
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

/************************************************************************/
/* NAME                         C L O C K                               */
/*                                                                      */
/* FUNCTION     clock - determine processor time			*/
/*                                                                      */
/* SYNOPSIS     clock_t clock (void)					*/
/*                                                                      */
/* RETURN       best approximation of the processor time used by the	*/
/*		program.						*/
/*                                                                      */
/* DESCRIPTION  This routine is supposed to return the best		*/
/*		approximation of processor time used by the program.	*/
/*		In order to measure the time spent in a program, the	*/
/*		"clock" function should be called at the beginning	*/
/*		of a program and its return value subtracted from the	*/
/*		value returned by subsequent calls.  This difference	*/
/*		should be divided by CLOCKS_PER_SEC to yield the number */
/* 		of seconds of processor time consumed by the program.	*/
/*									*/
/*		This particular implementation though simply returns	*/
/*		-1 to indicated that processor time is not available.	*/
/*		If the user wishes to provide his own "clock" function	*/
/*		he must link it in before the run-time libraries.	*/
/*                                                                      */
/* MODIFICATION HISTORY                                                 */
/*                                                                      */
/************************************************************************/

#include <time.h>

clock_t clock(void)
{
	return ((clock_t)-1);		/* clock not available */
}
 
#endif /* EXCLUDE_mriclock */
 

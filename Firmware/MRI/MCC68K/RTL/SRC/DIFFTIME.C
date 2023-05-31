#include "lib_top.h"


#if EXCLUDE_difftime
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
/* NAME                   D I F F T I M E				*/
/*                                                                      */
/* FUNCTION     difftime - compute difference between two calendar	*/
/*			   times					*/
/*                                                                      */
/* SYNOPSIS     double difftime (time_t time1, time_t time0);		*/
/*			time1 - a calendar time				*/
/*			time0 - a calendar time				*/
/*                                                                      */
/* RETURN       time1 - time0						*/
/*                                                                      */
/* DESCRIPTION  This routine returns time1 - time0 expressed as a	*/
/*		double							*/
/*                                                                      */
/* MODIFICATION HISTORY                                                 */
/*                                                                      */
/************************************************************************/

#include <time.h>

double difftime(time_t time1, time_t time0)
{
	return ((double)(time1 - time0));
}
 
#endif /* EXCLUDE_difftime */
 

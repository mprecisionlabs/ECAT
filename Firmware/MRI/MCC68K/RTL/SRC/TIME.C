#include "lib_top.h"


#if EXCLUDE_time
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
/* NAME                         T I M E                                 */
/*                                                                      */
/* FUNCTION     time - determine current calendar time			*/
/*                                                                      */
/* SYNOPSIS     time_t	time (time_t *timer)                            */
/*              time_t  *timer - address of location where time is	*/
/*				 returned to				*/
/*                                                                      */
/* RETURN       current calendar time in seconds since			*/
/*		00:00:00 Jan. 1, 1970					*/ 
/*                                                                      */
/* DESCRIPTION  This routine is supposed to return the number of seconds*/
/*		since 00:00:00 Jan. 1, 1970.  If timer is not NULL, this*/
/*		value is also returned to the address pointed to by	*/
/*		timer.							*/
/*									*/
/*		This particular implementation though simply returns	*/
/*		-1 to indicated that calendar time is not available.	*/
/*		If the user wishes to provide his own "time" function	*/
/*		he must link it in before the run-time libraries.	*/
/*                                                                      */
/* MODIFICATION HISTORY                                                 */
/*                                                                      */
/************************************************************************/

#include <time.h>

time_t time(time_t *timer)
{
	if (timer)
	    *timer = (time_t)-1;
	return ((time_t)-1);		/* time not available */
}
 
#endif /* EXCLUDE_time */
 

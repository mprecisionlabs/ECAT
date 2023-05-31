#include "lib_top.h"


#if EXCLUDE_strcoll
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

/*
 * strcoll compares two strings and returns an integer greater than,
 * equal to, or less than 0, according to the native collating sequence
 * table if string1 is greater than, equal to, or less than string2.
 */

#include <string.h>

int strcoll(const char *s1, const char *s2)
{
	return strcmp(s1, s2);
}
 
#endif /* EXCLUDE_strcoll */
 

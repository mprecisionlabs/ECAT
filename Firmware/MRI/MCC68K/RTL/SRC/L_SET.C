#include "lib_top.h"


#if EXCLUDE_l_set
    #pragma option -Qws			/* Prevent compiler warning */
#else
 
/****************************************************************************/
/* THIS INFORMATION IS PROPRIETARY TO                                       */
/* MICROTEC RESEARCH, INC.                                                  */
/*--------------------------------------------------------------------------*/
/* Copyright (c) 1990, 1991, 1992 Microtec Research, Inc.                   */
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

#include <stddef.h>

#define	NULL_CHAR	'\0'

/*	At present, this routine has no effect on the rest of the library
 *  routines.  There is only one known locale.  It is "C".  If a locale
 *  inquiry is made (i.e. locale == NULL) then "C" is returned.  If the
 *  invocation is a locale set, (i.e. locale != NULL) then the locale 
 *  must be "C" or "".  Other values for locale will cause a NULL to be
 *  returned (i.e. unsuccessful locale change).
 */

char *setlocale(int category, const char *locale)
{
	if ((locale == NULL) ||
		(*locale == NULL_CHAR) ||
		(*locale == 'C' && *++locale == NULL_CHAR))
			return("C");
	return(NULL);
}  /* setlocale() */
 
#endif /* EXCLUDE_l_set */
 

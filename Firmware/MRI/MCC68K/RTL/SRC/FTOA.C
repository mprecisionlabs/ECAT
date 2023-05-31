#include "lib_top.h"


#if EXCLUDE_ftoa
    #pragma option -Qws			/* Prevent compiler warning */
#else

/****************************************************************************/
/* THIS INFORMATION IS PROPRIETARY TO                                       */
/* MICROTEC RESEARCH, INC.                                                  */
/*--------------------------------------------------------------------------*/
/* Copyright (c) 1987, 1988, 1989, 1991, 1992 Microtec Research, Inc.       */
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

/************************************************************************
 *  ftoa.c
 *			F T O A    function
 *
 *	Copyright 1986 by Microtec Research Inc.  All rights reserved.
 *
 *  ftoa (Value, Str, Fmt, Prec)
 *
 *	Convert a double precision floating point number Value to a null-
 *	terminated ASCII string with a specified format and precision.
 *	The output is placed in the area pointed to by Str.  No leading
 *	blanks or zeros are produced.
 *
 *	The conversion format ('f', 'e', 'E', 'g', 'G') and precision
 *	are the same as those used for the __STDC__ printf function.
 *
 *	The function returns the number of characters placed in the
 *	output string, excluding the null terminator.
 *
 *	NOTE: IEEE double fp format (64-bit) has a sign bit, 11-bit
 *		biased exponent, and 52-bit mantissa.  Value range is
 *		9.46*10**-308 <= |x| <= 1.80*10**308.
 *
 *  written by H. Yuen  07/18/86
 *  05/26/88 HY - check input value for infinity
 *  09/16/89 jpc  added mrilib.h/mriext.h to allow easy making of '881 versions
 *  09/17/89 jpc  modified to allow output of -<NaN> and -<Infinity>
 *  09/31/89 jpc  infinity and nan strings now static
 *  09/07/89 jpc  remove ability to write -<NaN>
 *  11/28/89 jpc  fix rounding bug in "f" format fractions
 *   1/02/89 jpc  '86 RTS cannot handle denorms--treats as zero--ftoa fixed
 *		  to treat denorms as zero also.
 *   2/13/90 jpc  modified to generate alternate formats.  Sign bit is set in
 *                "fmt" by caller to indicate alternate.  NOT DOCUMENTED.
 *  05/09/90 jpc  modified to simply call _ftoa.
 *  11/19/91 jpc: removed __STDC__ construct
 *  02/14/92 jpc: added buffer to keep from blowing caller's buffer
 *
 ************************************************************************/
/* (last mod 05/26/88  HY) */

#include <string.h>
#include <stdlib.h>
#include <mriext.h>
#include "mrilib.h"

#define	NDIG	310		/* number of digits in f format */

int ftoa (double value, char *str, int fmt, int prec)
{
char	buf[NDIG+4];				/* buffer to be used by _ftoa */
int	retVal;

    retVal = _ftoa (value, buf, fmt, prec);	/* execute _ftoa-write to buf*/
    strcpy (str, buf);				/* copy result to caller */
    return retVal;
}

#endif /* EXCLUDE_ftoa */


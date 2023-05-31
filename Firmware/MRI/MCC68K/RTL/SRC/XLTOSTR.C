#include "lib_top.h"


#if EXCLUDE_xltostr
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
 *  _ltostr.c
 *			L T O S T R   function
 *
 *  int _ltostr (N,Str,Base)
 *
 *	Converts an unsigned long integer N to a null-terminated ASCII string
 *	of number in base Base and place the output in the area pointed
 *	to by Str.  The specified base must be between 2 and 36, otherwise,
 *	base 10 is assumed.  No leading blanks or zeros are produced.
 *	In addition to numeric digits the letters 'A' through 'Z' are used
 *	for numbers of base greater than 10.
 *
 *	The function returns the number of characters placed in the
 *	output string, excluding the null terminator.
 *
 *	NOTE: use ltoa() for converting signed decimal numbers.
 *
 *  written by Herbert Yuen  06/04/85
 *
 *  03/08/91 HY  - increase buf[] size to 36 because of base 2
 *  04/04/91 jpc - ltostr -> _ltostr
 *
 ************************************************************************/
/* (last mod 03/08/91  HY) */

#include <string.h>

int _ltostr (unsigned long n, char *str, int base)

{
	register char *p;
	int  count;
	char buf[36];
	register unsigned long divRes;	/* Intermediate division result */
	register unsigned char mod;	/* Modulo result */

	if (base < 2 || base > 36) base = 10;
	p = &buf[35];
	*p = '\0';
	count = 0;
	while (1) {
	    if (base == 16) {		/* order of probability of occurrence */
		mod = n & 0xf;		/* call from printf */
		divRes = n >> 4;
		}
	    else if (base == 8) {
		mod = n & 0x7;
		divRes = n >> 3;
		}
	    else if (base == 10) {
#if _MCC80				/* Kinda clugie -- but it works */
		return _ultoa ((unsigned long) n, str);
#else
		divRes = n / 10;
		mod = n - ((divRes << 3) + (divRes + divRes));	/* *10 by shft*/
#endif
		} 
	    else {
		divRes = n / base;
		mod = n - (divRes * base);	/* nrmlly mult faster thn div */
		}

	    *--p = mod + '0';
	    if (*p > '9')
		*p += 'A'-10-'0';
	    ++count;
	    if (n < base) break;
	    n = divRes;
	}
	strcpy(str,p);
	return count;
}
 
#endif /* EXCLUDE_xltostr */
 

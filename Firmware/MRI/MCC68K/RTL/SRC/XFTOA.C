#include "lib_top.h"


#if EXCLUDE_xftoa
    #pragma option -Qws			/* Prevent compiler warning */
#else

/****************************************************************************/
/* THIS INFORMATION IS PROPRIETARY TO                                       */
/* MICROTEC RESEARCH, INC.                                                  */
/*--------------------------------------------------------------------------*/
/* Copyright (c) 1987, 1988, 1989, 1991, 1992  Microtec Research, Inc.	    */
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
 *  _ftoa.c
 *			_ F T O A    function
 *
 *	Copyright 1986 by Microtec Research Inc.  All rights reserved.
 *
 *  _ftoa (Value, Str, Fmt, Prec)
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
 *   5/09/90 jpc  changed name to _ftoa
 *   1/14/91 HY   use faster algorithm to compute exponent in powers of 10
 *  01/20/91 jpc  removed __STDC__ #if contoling functions declarations
 *  04/08/91 jpc  added another fix to do correct rounding in g format
 *  01/28/92 jpc  fixed bugs involved with rounding all the way to high
 *		  order digit.
 *  11/10/93 jpc  DIFF is now unsigned.
 *  11/19/93 jpc  changes for 960 big endian support
 *
 ************************************************************************/
/* (last mod 01/14/91  HY) */

#include <string.h>
#include <stdlib.h>
#include <mriext.h>
#include <math.h>

#include "mrilib.h"
#include "op_type.h"

#define MAXSIG	17	/* max significant digits of 64-bit IEEE f.p. */
#define NDIG	310	/* max # digits for 'f' format */

#if _SIZEOF_PTR == 2
    typedef unsigned short	DIFF;
#else
    typedef unsigned long	DIFF;
#endif

static const long ltab[] =
    { 100000000, 10000000, 1000000, 100000, 10000, 1000, 100, 10, 1, 0 };
static const double pow10[] =
    { 10., 1E2, 1E4, 1E8, 1E16, 1E32, 1E64, 1E128, 1E256 };


/*	Set up zero-test so that a reasonable test for zero can be made	*/
/*	For the '86 RTS							*/

#if _MCC86 
#define IS_ZERO(x)	((((short *)&x)[3] & 0x7ff0) == 0)
#else
#define IS_ZERO(x)	((x) == 0.0)
#endif

int _ftoa (double value, char *buf, int int_fmt, int prec)
{
	register int i;
	register char *p;
	int  negAlt;			/* ! alternate format flag */
	char *p0, *buf0;
	char *decpt;			/* ptr to decimal pt */
	char *round;			/* ptr to digit being rounded */
	int  count;
	char fmt = int_fmt;		/* store format as char */
	char f_fmt;			/* true if f format */
	char g_fmt;			/* true if g format */
	char iszero;			/* true if value is 0.0 */
	char negexp;			/* true if exp is negative */
	int  j, k, grp;
	int  xprec;			/* precision for rounding */
	long lval;
	int  iexp;			/* exponent */
	double mant;			/* mantissa */
	int  jexp;
static	const	char infinity[] = "<Infinity>";	/* moved to support */
static	const	char nan[]	= "<NaN>";	/* mcc */

	union	{
		double dbl;
		struct	{
#if _LITTLE_ENDIAN || __i960_BIG_ENDIAN__
			unsigned long	lo, hi;
#else
			unsigned long	hi, lo;
#endif
			} lng;
		} cracker;

	cracker.dbl = value;		/* prepare to break it apart */
	if ((cracker.lng.hi & 0x7ff00000) == 0x7ff00000)/* inf or nan ?? */
		{
		if ((cracker.lng.hi & 0x000fffff) || cracker.lng.lo)	/* NaN*/
			{
			strcpy (buf, nan);
			count = sizeof (nan);
			}
		else
			{
			count = 0;			/* preset */
			if (cracker.lng.hi & 0x80000000) /* neg. inf. */
				{
				*buf++ = '-';
				count++;
				}
			strcpy (buf, infinity);
			count += sizeof (infinity);
			}
		return --count;
		}

	negAlt = 1;			/* alt fmt == false */
	if (fmt & 0x80)			/* check sign bit in fmt */
	    {
	    negAlt = 0;			/* alt fmt == true */
	    fmt &= 0x7f;		/* correct fmt */
	    }

	count = 0;
	f_fmt = g_fmt = 0;		/* assume e format */
	iszero = 0;
	if (prec < 0)
	    prec = 6;
	if (fmt == 'g' || fmt == 'G') {
	    g_fmt = 1;
	    if (prec == 0) ++prec;
	}
	else {
	    ++prec;
	    if (fmt == 'f') f_fmt = 1;
	}
	xprec = prec;
	if (xprec > MAXSIG)		/* no significance if too big */
	    xprec = MAXSIG;
	if (value < 0.0) {		/* take care of negative number */
	    *buf++ = '-';
	    count++;
	    value = -value;
	}
				/* split value into mantissa and exponent */
	mant = value;
	if (IS_ZERO (mant))	/* check for zero */
	    { iexp = 0; iszero = 1; }
	else {
				/*
				 * want to compute exponent as 10 ** EXP
				 * frexp() returns exponent as 2 ** jexp
				 *	EXP = jexp * log10(2)
				 * Note: frexp() is fast - no FP involved
				 */
	    (void) frexp(mant,&jexp);
	    jexp = (jexp * 308L) >> 10;		/* 308/1024 ~= log10(2) */
	    negexp = 0;
	    if ((iexp = jexp) < 0) {
		jexp = -jexp;
		negexp++;
	    }
	    for (i = 0; jexp != 0; i++) {
		if (jexp & 1)
		    if (negexp)
			mant *= pow10[i];
		    else
			mant /= pow10[i];
		jexp = (unsigned)jexp >> 1;
	    }
	    if (mant < 1.0)
		{ iexp--; mant *= 10.0; }
	    else if (mant >= 10.0)
		{ iexp++; mant /= 10.0; }
	}
	if (g_fmt && iexp >= -4 && iexp < prec)
	    f_fmt = 1;			/* use f format */
	p = buf;
	buf0 = p;
	if (iszero)			/* value == 0.0 */
	    *p++ = '0';
	else {				/* do integer part */
	    i = 1;
	    if (! f_fmt)
		value = mant;
	    else if ((i = iexp) < 0)
		i = 1;
	    for (j = i; j >= 9; j -= 9)
		value /= 1E9;		/* scale down if > 2^63 - 1 */
	    for (grp = 0; grp < 3; ++grp) {
		lval = (long) value;	/* convert to long */
		if (grp != 0)
		    for (k=0; lval < ltab[k]; ++k)
			*p++ = '0';
		if (lval != 0L) {
		    p += _ltoa(lval,p);	/*   up to 9 digits at a time */
		    value -= (double) lval;
		}
		if ((i -= j) == 0)
		    break;
		j = 9;
		if (grp < 2)
		    value *= 1E9;	/* scale up for next 9 digits */
	    }
	    if (f_fmt) {
		if (iexp > MAXSIG)
		    for (p0 = buf0+MAXSIG; p0 < p; p0++)
			*p0 = '0';	/* no more sigificance */
		if (iexp < 0)
		    i = 1;
		for ( ; --i >= 0; )
		    *p++ = '0';		/* just fill with zeros */
		if (iexp + xprec >= MAXSIG)
		    if ((xprec = MAXSIG-iexp) <= 0)
			xprec = iszero = 1;	/* all zeros after decpt */
	    }
	}
	decpt = p;
	*p++ = '.';
	if (iszero)			/* value == 0.0 */
	    for (i = prec; --i != 0; )
		*p++ = '0';
	else {				/* do fraction part */
	    if (f_fmt && iexp < 0) {
		value = mant / 10.0;
		for (i = iexp; ++i != 0; )
		    *p++ = '0';		/* leading zeros in fraction */
	    }
	    for (i = 0; i < 2; i++) {	/* do twice (up to 18 digits) */
		if (value == 0.0) {
		    for (j = 9; --j >= 0; )
			*p++ = '0';
		    continue;
		}
		value *= 1E9;		/* 9 digits at a time */
		lval = (long) value;
		for (k=0; lval < ltab[k]; ++k)
		    *p++ = '0';
		if (lval != 0)
		    p += _ltoa(lval,p);
		if (prec <= 9)
		    break;
		if (i == 0)
		    value -= (double) lval;
	    }
	    if (g_fmt && f_fmt)		/* fixup for g format */
		xprec += -iexp;
	    for (p0=decpt+xprec+1; p0 < p; p0++)
		*p0 = '0';
	}
	*p = '\0';
					/* Now, round-off and truncate */
	if (g_fmt) {
	    if (xprec > prec)		/* due to leading zeros */
		prec = xprec;
	    if ((p0 = buf0 + prec) >= decpt) p0++;
	    if (p < p0) p0 = p;
	}
	else {
	    p0 = decpt + prec;
	    while (p <= p0)		/* precision > fractional digits */
		*p++ = '0';
	}
	p = round = decpt + xprec;
	if (*p >= '5') {
	    while (1) {			/* round preceeding digit */
		if (*--p == '.') --p;
		if ((*p += 1) <= '9') break;
		*p = '0';
		if (p == buf) {		/* round highest digit -- so shift */
		    register char * tempPnt;
		    ++p0;		/* bump pointers */
		    ++round;
		    for (tempPnt = round; tempPnt != p; --tempPnt)/* shift */
			*tempPnt = *(tempPnt-1);
		    *p = '1';		/* complete the round */
		    buf0 = p;
		    if (! f_fmt) {
		        *++p = '.';	/* change 10.*** to 1.0*** */
		        *++p = '0';
		        iexp++;		/* add 1 to exponent */
		        --p0;
		    }
		    break;
		}
	    }
	}

	*round = '0';			/* the digit being rounded */
	*p0 = '\0';			/* ends the string */
	p = p0;

	if (g_fmt) {
	    fmt += 'e' - 'g';
	    if (negAlt) {
		while (*--p == '0')     /* strip trailing zeros */
		    ;
		*++p = '\0';
	    }
	}

	if ((*(p-1) == '.') && negAlt)	/* no digit after decimal pt ? */
	    *--p = '\0';		/* strip '.' */

	if (! f_fmt) {			/* do exponent part */
	    *p++ = fmt;			/* 'e' or 'E' */
	    if (iexp >= 0)
		*p++ = '+';
	    else {
		*p++ = '-';
		iexp = -iexp;
	    }
	    if ((i = iexp) > 99) {	/* need 3 digits */
		*p++ = i / 100 + '0';
		i %= 100;
	    }
	    *p++ = i / 10 + '0';
	    *p++ = i % 10 + '0';
	    *p = '\0';
	}
	i = (DIFF)p - (DIFF)buf0;
	return (count + i);
}

#endif /* EXCLUDE_xftoa */


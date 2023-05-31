#include "lib_top.h"


#if EXCLUDE_atof
    #pragma option -Qws		/* Prevent compiler warning */
#else

/****************************************************************************/
/* THIS INFORMATION IS PROPRIETARY TO                                       */
/* MICROTEC RESEARCH, INC.                                                  */
/*--------------------------------------------------------------------------*/
/* Copyright (c) 1987, 1988, 1989, 1991, 1992 Microtec Research, Inc.	    */
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


/* %Z%  %M%  %I%  %G% */

/************************************************************************
 *  atof.c
 *			A T O F ,  S T R T O D    functions
 *
 *	Copyright 1986 by Microtec Research Inc.  All rights reserved.
 *
 *  atof (str)
 *  strtod (str, endptr)
 *
 *	Convert an ASCII string pointed to by str to double representation.
 *	The first unrecognized character ends the conversion.  If endptr
 *	is not a null pointer, a pointer to that character is stored in
 *	the object endptr points to.
 *
 *	NOTE: IEEE double fp format (64-bit) has a sign bit, 11-bit
 *		biased exponent, and 52-bit mantissa.  Value range is
 *		9.46*10**-308 <= |x| <= 1.80*10**308.
 *
 *  written by H. Yuen
 *	01/23/92 - jpc: mod for compare w. HUGE_VAL to remove exception
 *	04/01/91 - jpc: reworked to make atof re-entrant
 *      09/14/90 - jpc: strtod modifies errno; atof does not (per ANSI)
 *	01/05/90 - jpc: don't include stdlib.h for sun native
 *      12/07/89 - jpc: brought in fix from original '86 routines.
 *		        appears to have allowed the setting of *endptr with
 *			str when no conversion takes place.
 *	09/10/89 - jpc: added includes to allow name conversions to *881
 *			for 68K
 *	08/31/89 - jpc: pow10 is now static const
 *	10/06/89 - jpc: make "p" & "psave" const -- then cast them back
 *			when setting endptr.
 *	04/09/85 - implemented atof()
 *	10/15/86 - revised atof() as strtod()
 *
 ************************************************************************/
/* (last mod 10/15/85  HY) */

#include <math.h>
#include <errno.h>

#if CPLUSPLUS_NAME_CONVERT			/* used in C++ libraries also */
    #define	atof	_atof_nf
    #define	strtod	_strtod_nf

    #include	"m_nf_fix.h"			/* convert other names */
#endif

#ifdef sun
    #undef	HUGE_VAL	/* Define HUGE_VAL using constant */
    #define	HUGE_VAL  (1.0e309)
#else
    #include <stdlib.h>	/* don't include header for sun native */
#endif

#define MAX_ABS_EXP	325

static const double pow10[9] =
	{ 10., 1E2, 1E4, 1E8, 1E16, 1E32, 1E64, 1E128, 1E256 };

static double _strtod (const char *str, char **endptr, int *errnoPtr)

{
 	register const char *p;
	register int c;
	register int exp, eexp, negexp, neg;
	int  ndigits;
	double fnum;
	double fnum_orig;
	double big = 4503599627370496.;		/* 2^52 */
	const char *psave;
	unsigned char fdigit;

	fdigit = 0;
	psave = str;

	ndigits = neg = exp = 0;
	fnum = 0.0;
	p = str;
	while  (*p == ' ' || (*p >= 9 && *p <= 13))	/* "white spaces" */
	    ++p;
	if (*p == '-')				/* negative number ? */
	    { neg++; p++; }
	else if (*p == '+')			/* ignore + sign */
	    p++;
	while ((c = *p++) >= '0' && c <= '9') {
	    if (++ndigits < 16 || fnum < big)
		fnum = fnum*pow10[0] + (c - '0');
	    else ++exp;
	    fdigit = 1;
	}
	if (c == '.') {
	    while ((c = *p++) >= '0' && c <= '9')
		if (++ndigits < 16 || fnum < big) {
		    fnum = fnum*pow10[0] + (c - '0');
		    exp--;
		    fdigit = 1;
		}
	}
	fnum_orig = fnum;			/* save for zero test later */

	eexp = negexp = 0;
	if (c == 'E' || c == 'e') {
	    if ((c = *p) == '+')
		p++;
	    else if (c == '-')			/* negative exponent ? */
		{ negexp++; p++; }
	    while ((c = *p++) >= '0' && c <= '9') {
		eexp = eexp*10 + (c - '0');	/* get the exponent */
		if (abs (negexp ? exp - eexp : exp + eexp) > MAX_ABS_EXP)
		    break;			/* stop if eexp gets too big */
	    }
	    if (negexp)
		exp -= eexp;			/* overall exponent */
	    else
		exp += eexp;
	}
	negexp = 0;
	if (exp < 0) {
	    negexp++;			/* overall exponent is negative */
	    exp = -exp;
	}
	if (exp != 0) {
	    eexp = 256;
	    for (c = 8; c >= 0; c--) {
		while (exp >= eexp) {
		    if (negexp)
			fnum /= pow10[c];
		    else
			fnum *= pow10[c];
		    exp -= eexp;
		}
		if (exp == 0) break;
		eexp >>= 1;
	    }
	}
	if (endptr)
	    *endptr = (fdigit == 0) ? (char *) psave : (char *) p - 1;

	if ((fnum == 0.0) && (fnum_orig != 0.0)) {	/* underflow */
	    *errnoPtr = ERANGE;				/* as per ANSI */
	    return fnum;
	}

#if _MCC86 && _FPU			/* code to remove exception */
	{
	double temp = HUGE_VAL;

	if (! memcmp (&temp, &fnum, sizeof (double)))
	    *errnoPtr = ERANGE;		/* as per ANSI */
	}
#else
	if (fnum == HUGE_VAL)
	    *errnoPtr = ERANGE;		/* as per ANSI */
#endif
	if (neg)
	    fnum = -fnum;
	return (fnum);
}

double strtod (const char *str, char **endptr)

{
	return _strtod (str, endptr, (int *) &errno);	/* This one sets errno*/
}

double atof (const char *str)

{
int	dummyErrno;

	return _strtod (str, (char **)0, &dummyErrno);	/* Don't set errno */
}

#endif /* EXCLUDE_atof */


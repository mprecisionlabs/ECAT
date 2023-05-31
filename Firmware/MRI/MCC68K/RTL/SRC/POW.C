#include "lib_top.h"


#if EXCLUDE_pow || (_MCC68K && (!_FPU)) || (_MCC86 && _FPU)
    #pragma option -Qws			/* Prevent compiler warning */
#else
 
#if	_MCC68K
    #pragma option -f
    #ifdef	_FPU
	#undef	_FPU
    #endif	/* _FPU */
    #define	_FPU	1
#endif	/* _MCC68K */

/************************************************************************/
/* THIS INFORMATION IS PROPRIETARY TO					*/
/* MICROTEC RESEARCH, INC.						*/
/*----------------------------------------------------------------------*/
/* Copyright (c) 1986, 1991, 1992 Microtec Research, Inc.               */
/* All rights reserved							*/
/************************************************************************/
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
/* NAME                         P  O  W					*/
/*                                                                      */
/* FUNCTION     pow							*/
/*                                                                      */
/* SYNOPSIS     							*/
/*                                                                      */
/* RETURN       							*/
/*                                                                      */
/* DESCRIPTION                                                          */
/*                                                                      */
/* NOTE		This routine is also used by the 68k native libraries	*/
/*									*/
/* MODIFICATION HISTORY                                                 */
/*      Author - M.H. 10/14/86		                                */
/*	01/04/90 jpc: added include <math.h>; declaration -> ANSI	*/
/*		      added local def of HUGE_VAL			*/
/*	01/08/90 jpc: return nan on domain error on sun only		*/
/*	01/09/90 jpc: converted for single precision; errno not set 0 	*/
/*	02/20/90 jpc: pow (0.0, 0.0) now returns 1.0.			*/
/*                                                                      */
/************************************************************************/
/*	%W% %G%	*/

#include <math.h>
#include <errno.h>
#include "m_incl.h"

#if sun
    #undef	HUGE_VAL
    #define	HUGE_VAL (1.0e309)	/* ieee infinity */
#endif

#if sun
static const unsigned long nan[2] = {0x7fffffff, 0xffffffff};
#endif

#if __OPTION_VALUE ("-Kq")		/* single precision */
    
    #if _MCC86
        #define MINEEXP ((float) -87.336544)
    #else
        #define MINEEXP ((float) -103.278930)
    #endif

    #define MAXEEXP ((float) 88.722099)
    
    #undef	isNAN
    #define	isNAN		isNANf
    #define	crackType	realnumberF
    
#else					/* double precision */

    #if _MCC86				/* does not support denorms */
	#define MINEEXP -708.396418532263
    #else
	#define MINEEXP -744.440071921381
    #endif

    #define MAXEEXP 709.782712893383
    
    #define	crackType	realnumber

#endif

double pow (double x, double y)
{
    register int i;
    register double result;
    crackType	cracker;

    cracker.value = x;
    if (isNAN (cracker))			/* check for NaN's */
	return x;
    cracker.value = y;
    if (isNAN (cracker))
	return y;

    i = (int) y;
	
    if ((double) i == y) {
	result = 1.0;
	if (x == 0.0 && i <= 0) {
	    if (i == 0)
		return 1.0;
	    else
		{
		errno = EDOM;
#if sun
	        return *(double *) nan;
#else
	        return (HUGE_VAL);
#endif
		}
	}
	while (i > 0) {
	    result *= x;
	    i--;
	}
	while (i < 0) {
	    result /= x;
	    i++;
	}
	return (result);
    }
    if (x < 0.0) {
	errno = EDOM;
#if sun
	return *(double *) nan;
#else
	return (-HUGE_VAL);
#endif
    }

    y = y*log(x);

    if (y > MAXEEXP)
	{
	errno = ERANGE;
        return(HUGE_VAL);
	}
    
    if (y < MINEEXP)
	{
	errno = ERANGE;
        return(0.0);
	}
    
    return (exp(y));
}
 
#endif /* EXCLUDE_pow */
 

#include "lib_top.h"


#if EXCLUDE_floor || (_MCC68K && (!_FPU)) || (_MCC86 && _FPU)
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
/* NAME		                F  L  O  O  R                           */
/*                                                                      */
/* FUNCTION     floor - returns the largest integer that is less than   */
/*                      or equal to the argument x.                     */
/*                                                                      */
/* SYNOPSIS     double floor(x)                                         */
/*              double x;                                               */
/*                                                                      */
/* RETURN       The floor function returns the largest integer not      */
/*              greater than x, expressed as a double.                  */
/*                                                                      */
/* DESCRIPTION  The floor function computes the largest integer not     */
/*              greater than x.                                         */
/*                                                                      */
/* NOTE		This file is also used by the 68k native libraries	*/
/*                                                                      */
/* MODIFICATION HISTORY                                                 */
/*      Author - M. H. 11/14/86                         	        */
/*                                                                      */
/*	01/04/90 jpc: include file added; declaration changed to ANSI	*/
/*	01/08/90 jpc: modified for single precision			*/
/*                                                                      */
/************************************************************************/
/*	%W% %G%	*/

#include <math.h>
#include "m_incl.h"

#if __OPTION_VALUE ("-Kq")			/* single precision */

    #define	crackType realnumberF
    #undef	isNAN
    #define	isNAN	isNANf

#else

    #define	crackType realnumber

#endif

double floor(double x)
{
double y, yn;
crackType cracker;

cracker.value = x;				/* check for NaN's */
if (isNAN (cracker))
    return x;

if (x == ((double) 0.0))
    return (((double) 0.0));

y=modf(x, &yn);

if (x > ((double) 0.0))
	return(yn);
   else 
	{
	if (x==yn) return(yn);
	   else return(yn - ((double) 1.0));
	}
}
 
#endif /* EXCLUDE_floor */
 

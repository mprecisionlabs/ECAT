#include "lib_top.h"


#if EXCLUDE_ceil || (_MCC68K && (!_FPU)) || (_MCC86 && _FPU)
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
/* NAME		                C  E  I  L                              */
/*                                                                      */
/* FUNCTION     ceil - returns the smallest integer that is greater     */
/*                     than or equal to the argument x.                 */
/*                                                                      */
/* SYNOPSIS     double ceil(x)                                          */
/*              double x;                                               */
/*                                                                      */
/* RETURN       The ceil function returns the smallest integer not less */
/*              than x, expressed as a double.                          */
/*                                                                      */
/* DESCRIPTION  The ceil function computes the smallest integer not     */
/*              less than x.                                            */
/*                                                                      */
/* NOTE		This file also used by 68k native libraries		*/
/*									*/
/* MODIFICATION HISTORY                                                 */
/*      Author - M. H. 11/14/86                         	        */
/*									*/
/*	01/04/90 jpc: declaration changed to ANSI style, include file	*/
/*		      added.						*/
/*	01/08/90 jpc: modified for single precision 			*/
/*                                                                      */
/************************************************************************/
/*	%W% %G%	*/

#include <math.h>
#include "m_incl.h"

#if __OPTION_VALUE ("-Kq")				/* single precision */

    #define	crackType realnumberF
    #undef	isNAN
    #define	isNAN	isNANf

#else

    #define	crackType realnumber

#endif

double ceil(double x)
{
double y, yn;
crackType cracker;

cracker.value = x;			/* check for NaN */
if (isNAN (cracker))
    return x;

if (x == ((double) 0.0)) 
    return (((double) 0.0));

y=modf(x, &yn);

if ( x > ((double) 0.0) && x!=yn ) 
    return (yn + ((double) 1.0));
else
    return (yn); 	
}
 
#endif /* EXCLUDE_ceil */
 

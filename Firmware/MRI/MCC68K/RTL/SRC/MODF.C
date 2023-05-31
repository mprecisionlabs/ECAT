#include "lib_top.h"


#if EXCLUDE_modf || (_MCC68K && (!_FPU)) || (_MCC86 && _FPU)
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
/* NAME                         M  O  D  F				*/
/*                                                                      */
/* FUNCTION     modf - breaks the argument value into intgral and 	*/
/*		       fractional parts.			 	*/
/*                                                                      */
/* SYNOPSIS     double(val,iptr)					*/
/*		double val, *iptr					*/
/*                                                                      */
/* RETURN       The modf function returns the signed fractional part of */
/*		value.							*/
/*                                                                      */
/* DESCRIPTION  the modf function splits a floating-point value into a  */
/*		integer and fractional parts, each of which has the 	*/
/*		same sign as the argument. It stores the integral part  */
/*		as a double in the objects pointed to by iptr.		*/
/*                                                                      */
/* NOTE		This routine is also used in the 68k native libraries	*/
/*                                                                      */
/* MODIFICATION HISTORY                                                 */
/*      Author - M.H. 10/14/86          	                        */
/*             - M.H. 2/2/87	Major rewrite to make function accurate */
/*	       - jpc 12/20/89	"limit" is now "const"			*/
/*	       - jpc 01/04/90	added include <math.h>; declaration ->	*/
/*				ANSI					*/
/*	       - jpc 01/09/90   Modified for single precision		*/
/*	       - jpc 01/16/90   Converted portion of alogithm to bit	*/
/*				diddling 'cuz it wasn't working well 	*/
/*				with '881 in target environment		*/
/*                                                                      */
/************************************************************************/
/*	%W% %G%	*/

#include <math.h>
#include "m_incl.h"

#if __OPTION_VALUE ("-Kq")			/* single precision */

    #define	MAX_VAL		((float) 1.0e8)
    #undef	isNAN
    #define	isNAN		isNANf
    #define	crackType	realnumberF

#else

    #define	MAX_VAL		1.0e16
    #define	crackType	realnumber

#endif

double modf (double x, double *intptr)
{
double  y, integer, fraction;
int	i, j;
char	negative;
int	exponent;
crackType	cracker;

#if ! __OPTION_VALUE ("-Kq") 		/* only for double version */
	
    unsigned long hiMask = 0xfff00000;
    unsigned long loMask = 0x00000000;
    
    double	yy, integer1, fraction1;

#endif

cracker.value = x;
if (isNAN (cracker))			/* Handle NaN's */
	{				/* Just return the NaN */
	*intptr = x;
	return x;
	}

if (x == 0.0) 
	{
	*intptr=0.0;
	return(0.0);
	}

if (x<0.0) 
	{
	negative='Y';
	y = -x;
	}
   else
	{
	negative='N';
	y = x;
	}

if (y >= MAX_VAL)
	{
	integer=y;
	fraction=0.0;
	}
else
	{
#if __OPTION_VALUE ("-Kq")
	integer = (float) ((long) y);
	fraction = y - integer;
#else	/* ! __OPTION_VALUE ("-Kq") */
	cracker.value = y;
	exponent = (cracker.half.hi >> 20) - 0x3ff;
	if (exponent < 0)	/* fraction */
	    {
	    integer = 0;
	    fraction = y;
	    }
	else
	    {
	    if (exponent <= 20)
		hiMask = ((long) hiMask) >> exponent;
	    else
		{
		hiMask = 0xffffffff;
		loMask = ((long) 0x80000000) >> (exponent - 21);
		}
	    cracker.value = y;
	    cracker.half.hi &= hiMask;
	    cracker.half.lo &= loMask;
	    integer = cracker.value;
	    fraction = y - integer;
	    }
#endif	/* ! __OPTION_VALUE ("-Kq") */
	}

if (negative=='Y') 
	{
	*intptr = -integer;
	if (fraction != 0.0)
	fraction = -fraction;
	}
   else
	{
	*intptr = integer;
	}
return(fraction);
}
 
#endif /* EXCLUDE_modf */
 

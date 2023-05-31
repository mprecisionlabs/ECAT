#include "lib_top.h"


#if EXCLUDE_m_atanf || _80960SB || _80960KB || _80960MC || (_MCCG32 && _FPU) \
	|| (_MCC68K && _FPU) \
	|| (_MCC86 && (! _FPU)) \
	|| (!__OPTION_VALUE ("-Kq"))
    #pragma option -Qws			/* Prevent compiler warning */
#else
 
/************************************************************************/
/* THIS INFORMATION IS PROPRIETARY TO					*/
/* MICROTEC RESEARCH, INC.						*/
/*----------------------------------------------------------------------*/
/* Copyright (c) 1991, 1992 Microtec Research, Inc.                     */
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


/* %W% %G% */

/*
    SINGLE PRECISION ARCTANGENT

    Author: Mark McDowell
    Date:   December 20, 1984
    Source: Software Manual for the Elementary Functions
	by William James Cody, Jr. and William Waite (1980)

    This routine will work only for numbers in IEEE single precision
    floating point format. Of the numbers in this format, it checks for 
    and handles only normalized numbers and -0 and +0 -- not infinities, 
    denormalized numbers, nor NaN's.

 */
/*---------------------------------------------------------------------
    Revised by Michael Fluegge		07/13/87
  
      The routine now handles all of the special IEEE format numbers.
    The only changes which were necessary was the addition of a check
    for NaNs.  The old code handles all of the other special forms
    correctly.				*/

/*
NOTE:
	This routine is designed to be used only when the -Kq switch is turned
on. When this is done, doubles are treated as though they are only 32-bit long. 
And as such, floats and doubles are the same.
*/

/*
Change history:
	11/15/93 - jpc	Split atan2f out into separate module.
	11/03/93 - jpc  Converted "INTEL" and "MOTOROLA" to "_LITTLE_ENDIAN"
			and _BIG_ENDIAN, respectively; removed special handling
			of NaN's for little-endian machines -- all NaN's are
			handled the same now.
	11/12/93 - jpc	Converted to function with -Kq switch
			floats converted to doubles
*/

#include "m_incl.h"
#include "op_reent.h"

#define TWO_MINUS_SQRT3  0x3E8930A3
#define ATN_EPS          0x39800000
#define ATN_MAX          0x4C700518
#define HEX_ONE          0x3F800000
#define PI               (*((const double *) &hex_pi))
#define PI_OVER_2        (*((const double *) &hex_pi_over_2))
#define SQRT3            (*((const double *) &hex_sqrt3))
#define SQRT3_MINUS1     (*((const double *) &hex_sqrt3_minus1))
#define P0               (*((const double *) &hex_p0))
#define P1               (*((const double *) &hex_p1))
#define Q0               (*((const double *) &hex_q0))
#define A1               (*((const double *) &hex_a1))
#define A2               (*((const double *) &hex_a2))
#define A3               (*((const double *) &hex_a3))

static const UInt32	hex_pi =           0x40490FDB,
			hex_pi_over_2 =    0x3FC90FDB,
			hex_sqrt3 =        0x3FDDB3D7,
			hex_sqrt3_minus1 = 0x3F3B67AF,
			hex_p0 =           0xBEF110F6,
			hex_p1 =           0xBD508691,
			hex_q0 =           0x3FB4CCD3,
			hex_a1 =           0x3F060A92,
			hex_a2 =           0x3FC90FDB,
			hex_a3 =           0x3F860A92;


double atan(double arg)
{
    STATIC realnumberF		f, g;
    STATIC register int	n;
    STATIC register UInt32	argsign;

    f.value = arg;
    
    if (isNANf(f))
	return( _fp_errorf((UInt32) (FP_atanf | SIGNAN), f.ivalue) );

    argsign = f.ivalue & 0x80000000;		/* save sign */
    f.ivalue ^= argsign;			/* f = abs(arg) */

    if (f.ivalue >= ATN_MAX) {			/* for large arguments... */
	f.value = PI_OVER_2;			/* answer is PI/2... */
	f.ivalue |= argsign;		/* with sign of original argument */
	return(f.value);
    }

 /* Step 6 */
    if (f.ivalue > HEX_ONE) {			/* check for 1st reduction */
	f.value = ONEf / f.value;
	n = 2;
    } 
    else
	n = 0;

 /* Step 7 */
    if (f.ivalue > TWO_MINUS_SQRT3) {            /* 2nd reduction */
 /* Step 8 */
	g.value = (SQRT3_MINUS1 * f.value) - HALFf;
	g.value -= HALFf;
	f.value = (g.value + f.value) / (SQRT3 + f.value);
	n++;
    }

 /* f is now less than 2 - sqrt(3) */

    g.ivalue = f.ivalue & 0x7FFFFFFF;           /* g = abs(f) */

 /* Step 10 */
    if (g.ivalue >= ATN_EPS) {
 /* Step 11 */
	g.value = f.value * f.value;            /* calculate atan */
 /* Steps 13 and 14 */
	f.value += f.value *
               	   (((P1 * g.value + P0) * g.value) / (g.value + Q0));
    }

    if (n > 1)
	f.ivalue ^= 0x80000000;		/* change sign if necessary */

    switch (n) {			/* add an appropriate constant */
	case 0:			 break;
	case 1: f.value += A1;   break;
	case 2: f.value += A2;   break;
	case 3: f.value += A3;   break;
    }

    f.ivalue ^= argsign;                    /* adjust sign */
    return(f.value);
}  /* atanf() */

 
#endif /* EXCLUDE_m_atanf */
 

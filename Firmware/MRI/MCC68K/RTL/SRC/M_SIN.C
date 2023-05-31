#include "lib_top.h"


#if EXCLUDE_m_sin || _80960SB || _80960KB || _80960MC || (_MCCG32 && _FPU) \
	|| (_MCC68K && _FPU) \
	|| (_MCC86 && (! _FPU)) \
	|| (__OPTION_VALUE ("-Kq"))
    #pragma option -Qws			/* Prevent compiler warning */
#else

/*
 * @(SUBID) 05/10/88 /src2/nls/libraries/libm/s.sin.c 1.13
 * COPYRIGHT  Hewlett-Packard Company  1987, 1988
 */

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


/*	%W% %G%	*/

/*
    DOUBLE PRECISION SINE

    Author: Mark McDowell
    Date:   December 14, 1984
    Source: Software Manual for the Elementary Functions
	by William James Cody, Jr. and William Waite (1980)

    This routine will work only for numbers in IEEE double precision
    floating point format. Of the numbers in this format, it checks for 
    and handles only normalized numbers and -0 and +0 -- not infinities, 
    denormalized numbers, nor NaN's.

*/
/*---------------------------------------------------------------------
    Revised by Michael Fluegge		05/10/88

      The routine now handles infinities and NaNs.   Infinities generate
    an NaN while NaN arguments are simply returned.
      No special checking is needed for the denormalized numbers.  The sin
    of any denormalized value will be zero and it can be computed by using
    the normal code.
 */
/*
Change history:
	11/03/93 - jpc  Converted "INTEL" and "MOTOROLA" to "_LITTLE_ENDIAN"
			and _BIG_ENDIAN, respectively; removed special handling
			of NaN's for little-endian machines -- all NaN's are
			handled the same now.
*/

#if CPLUSPLUS_NAME_CONVERT
    #include "m_nf_fix.h"
#endif

#include "m_incl.h"
#include "op_reent.h"

#define SIN_MAX     (*((const double *) hex_sin_max))
#define SIN_EPS     (*((const double *) hex_sin_eps))
#define ONE_OVER_PI (*((const double *) hex_one_over_pi))
#define PI_OVER_TWO (*((const double *) hex_pi_over_two))
#define C1          (*((const double *) hex_c1))
#define C2          (*((const double *) hex_c2))
#define R1          (*((const double *) hex_r1))
#define R2          (*((const double *) hex_r2))
#define R3          (*((const double *) hex_r3))
#define R4          (*((const double *) hex_r4))
#define R5          (*((const double *) hex_r5))
#define R6          (*((const double *) hex_r6))
#define R7          (*((const double *) hex_r7))
#define R8          (*((const double *) hex_r8))


static const UInt32
	hex_one_over_pi[2] = HD_INIT(0x3FD45F30,0x6DC9C883),
	hex_pi_over_two[2] = HD_INIT(0x3FF921FB,0x54442D18),
	hex_sin_max[2] = HD_INIT(0x41B1C583,0x1A000000),    /* PI*2**(53/2) */
	hex_sin_eps[2] = HD_INIT(0x3E46A09E,0x667F3BCD),    /* 2**-(53/2)   */
	hex_c1[2] = HD_INIT(0x40092200,0x00000000),	/* 3.1416015625 */
	hex_c2[2] = HD_INIT(0xBEE2AEEF,0x4B9EE59E),
	hex_r1[2] = HD_INIT(0xBFC55555,0x55555555),
	hex_r2[2] = HD_INIT(0x3F811111,0x111110B0),
	hex_r3[2] = HD_INIT(0xBF2A01A0,0x1A013E1A),
	hex_r4[2] = HD_INIT(0x3EC71DE3,0xA524F063),
	hex_r5[2] = HD_INIT(0xBE5AE645,0x4B5DC0AB),
	hex_r6[2] = HD_INIT(0x3DE6123C,0x686AD430),
	hex_r7[2] = HD_INIT(0xBD6AE420,0xDC08499C),
	hex_r8[2] = HD_INIT(0x3CE880FF,0x6993DF95);


double
sin(arg)
double	arg; 
{
    STATIC realnumber       f, g;
    STATIC register UInt32  sgn;


    f.value = arg;

    if (isNAN(f)) {
	return( _fp_error((UInt32) (FP_sin | SIGNAN), f.value) );
    }

    sgn = f.half.hi & 0x80000000;	/* save sign */
    f.half.hi ^= sgn;			/* f = abs(arg) */

 /* Is the argument an infinity or will the result be out of range? */
    if (isINFINITY(f))
	return( _fp_error((UInt32) (FP_sin | OPERROR)) );

    if (f.value > SIN_MAX)
	return( _fp_error((UInt32) (FP_sin | PLOSS)) );


    if (f.value > PI_OVER_TWO) {            /* does argument need reduction? */
 /* divide it by PI and round */     /* g = INTRND(f / PI) */
	g.half.hi = (UInt32) (f.value * ONE_OVER_PI + HALF);
	if (g.half.hi & 1)
	   sgn ^= 0x80000000;	/* if an odd multiple of PI, change sign */

 /* g is equal to FLOAT(N) according to the algorithm */
	g.value = (double) g.half.hi;	/* make multiple into a double */

	f.value = (f.value - g.value * C1) - g.value * C2;	/* reduce */

	g.value = f.value;
	g.half.hi &= 0x7FFFFFFF;	/* g <- abs(f) */
    } 
    else
    	g.value = f.value;		/* argument already reduced */

 /* Step 11 starts here */
    if (g.value >= SIN_EPS) {		/* reduced argument significant? */
	g.value = f.value * f.value;	/* calculate sine */  /* Step 12 */
 /* Steps 13 and 14 combined */
	f.value += f.value * ((((((((R8 * g.value + R7) * g.value + R6) *
		   g.value + R5) * g.value + R4) * g.value + R3) *
		   g.value + R2) * g.value + R1) * g.value);
    }

    f.half.hi ^= sgn;			/* set the sign appropriately */
    return(f.value);
}  /* sin() */

#endif /* EXCLUDE_m_sin */


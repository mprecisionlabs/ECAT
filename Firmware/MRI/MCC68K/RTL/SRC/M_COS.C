#include "lib_top.h"


#if EXCLUDE_m_cos || _80960SB || _80960KB || _80960MC || (_MCCG32 && _FPU) \
	|| (_MCC68K && _FPU) \
	|| (_MCC86 && (! _FPU)) \
	|| (__OPTION_VALUE ("-Kq"))
    #pragma option -Qws			/* Prevent compiler warning */
#else

/*
 * @(SUBID) 05/10/88 /src2/nls/libraries/libm/s.cos.c 1.13
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
    DOUBLE PRECISION COSINE

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
      No special checking is needed for the denormalized numbers.  The cos()
    of any denormalized value will be one also and it can be computed by
    using the normal code.				*/
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

#define COS_MAX     (*((const double *) hex_cos_max))
#define COS_EPS     (*((const double *) hex_cos_eps))
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


static const UInt32	hex_cos_max[2] =     HD_INIT(0x41B1C583,0x186DE04B),
			hex_cos_eps[2] =     HD_INIT(0x3E46A09E,0x667F3BCD),
			hex_one_over_pi[2] = HD_INIT(0x3FD45F30,0x6DC9C883),
			hex_pi_over_two[2] = HD_INIT(0x3FF921FB,0x54442D18),
			hex_c1[2] =          HD_INIT(0x40092200,0x00000000),
			hex_c2[2] =          HD_INIT(0xBEE2AEEF,0x4B9EE59E),
			hex_r1[2] =          HD_INIT(0xBFC55555,0x55555555),
			hex_r2[2] =          HD_INIT(0x3F811111,0x111110B0),
			hex_r3[2] =          HD_INIT(0xBF2A01A0,0x1A013E1A),
			hex_r4[2] =          HD_INIT(0x3EC71DE3,0xA524F063),
			hex_r5[2] =          HD_INIT(0xBE5AE645,0x4B5DC0AB),
			hex_r6[2] =          HD_INIT(0x3DE6123C,0x686AD430),
			hex_r7[2] =          HD_INIT(0xBD6AE420,0xDC08499C),
			hex_r8[2] =          HD_INIT(0x3CE880FF,0x6993DF95);


double
cos(arg)
double	arg;
{
    STATIC register UInt32  sgn;
    STATIC realnumber       f, g;


    f.value = arg;

    if (isNAN(f)) {
	return( _fp_error((UInt32) (FP_cos | SIGNAN), f.value) );
    }

 /* Ignore the sign of the argument */
    f.half.hi &= 0x7FFFFFFF;		            /* cos(-x) = cos(x) */

 /* Is the argument an infinity or will the result be out of range? */
    if (isINFINITY(f))
	return( _fp_error((UInt32) (FP_cos | OPERROR)) );

    if (f.value > COS_MAX)
	return( _fp_error((UInt32) (FP_cos | PLOSS)) );


 /* The next two statements do Steps 2, 3, 6, and 7 */
 /* divide it by PI and round */
    g.half.hi = (UInt32) ((f.value + PI_OVER_TWO) * ONE_OVER_PI + HALF); 
    sgn = (g.half.hi & 1) ? 0x80000000 : 0x0;       /* sign of result */

 /* Step 9 */
    g.value = (double) g.half.hi - HALF;	/* make into a double */
 /* Step 10 */
    f.value = (f.value - g.value * C1) - g.value * C2;	/* reduce */
    g.value = f.value;
    g.half.hi &= 0x7FFFFFFF;			/* g = abs(f) */

    if (g.value >= COS_EPS) {           /* reduced argument significant? */
 /* Steps 12 - 14 */
	g.value = f.value * f.value;		/* calculate cosine */
	f.value += f.value * ((((((((R8 * g.value + R7) * g.value + R6) 
	    * g.value + R5) * g.value + R4) * g.value + R3) 
	    * g.value + R2) * g.value + R1) * g.value);
    }
    f.half.hi ^= sgn;			/* set the sign appropriately */

    return(f.value);
}  /* cos() */

#endif /* EXCLUDE_m_cos */


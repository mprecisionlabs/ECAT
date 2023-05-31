/****************************************************************************/
/* THIS INFORMATION IS PROPRIETARY TO                                       */
/* MICROTEC RESEARCH, INC.                                                  */
/*--------------------------------------------------------------------------*/
/* Copyright (c) 1991 Microtec Research, Inc.				    */
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

/*==========================================================================*/
/*==========================================================================*/
/*************************************************************************/
/*	MRI revisions --						 */
/*	05/27/92 jpc:	"math.h" -> <math.h>				 */
/*	08/09/89 jpc:	Replaced "math.h" section with actual include of */
/*			actual "math.h".  This brings in correct value of*/
/*			HUGE_VAL.  Also brings in #defines which will	 */
/*			make it easy to generate *881.o files for 68k.	 */
/*			Remove definition of functions of the form 	 */
/*			*fp_status and *fp_control.  They are defined and*/
/*			used as macros anyway.  			 */
/*	03/01/91 jpc:	Removed explicit code and supstituted #include	 */
/*			for "op_type.h"				 	 */
/*	11/03/93 - jpc  Converted "INTEL" and "MOTOROLA" to 		 */
/*			"_LITTLE_ENDIAN" and _BIG_ENDIAN, respectively;	 */
/*			removed special handling of NaN's for		 */
/*			little-endian machines -- all NaN's are handled	 */
/*			the same now.					 */
/*	11/19/93 - jpc	Added in support for 960 big endian		 */
/*									 */
/*************************************************************************/

	
#include "op_type.h"

/*==========================================================================*/
/*==========================================================================*/

#include <math.h>		/* Get system <math.h> */
#include <errno.h>		/* Pick up EDOM and ERANGE */

/*==========================================================================*/
/*==========================================================================*/

/*
 * LSD:@(#) 0.74 88/04/26                                           
 * @(mktid) (02.00 26Apr88)                                         
 * @(SUBID):$Source: /src2/nls/libraries/headers/fp_control.h,v $ $Revision: 1.9 $
 */

/* This header file is to be used with the HP 68020 AxLS compiler */

/* These macros are used to set the rounding mode on the 68881 chip */
#define RNDNEAR		0x0000	/* round to nearest */
#define RNDZERO		0x0010	/* truncate */
#define RNDNEGINF	0x0020	/* round towards negative infinity */
#define RNDPOSINF	0x0030	/* round towards positive infinity */

/* The following macros are used to select precision */
#define SGLPREC		0x0040	/* single precision */
#define DBLPREC		0x0080	/* double precision */
#define EXTPREC		0x0000	/* extended precision */

#define NOTRAP		0x0000	/* disable all traps */
#define NOERRORS	0x0000	/* no errors have been detected */

/*
 *	The following macros have a dual purpose.  They can be used
 *  as arguments to _set_fp_control() to enable particular traps or
 *  they can be used to check the return value of _get_fp_status()
 *  to determine what (if any) exception conditions have occurred.
 */
#undef	PLOSS			/* undef these first cuz they are set by */
#undef	UNDERFLOW		/* sun <math.h>	*/
#undef	OVERFLOW

#define PLOSS		0x0080	/* loss of precision in result */
#define INEXACT		0x0200	/* inexact result */
#define DIVZERO		0x0400	/* division by zero */
#define UNDERFLOW	0x0800	/* operation underflow */
#define OVERFLOW	0x1000	/* operation overflow */
#define OPERROR		0x2000	/* illegal operand */
#define SIGNAN		0x4000	/* detection of signaling NaN */
#define INEXACTD	0x0100	/* inexact decimal input (68881 only) */
#define BSUN		0x8000	/* non-aware branches (68881 only) */

extern double	_fp_error(long status, ...);	/* Report errors */
extern float	_fp_errorf(long status, ...);	/* Report errors */

/*==========================================================================*/
/*==========================================================================*/

/*
 * @(SUBID) 05/10/88 /src2/nls/libraries/libm/s.IEEEmath.h 1.13
 * COPYRIGHT  Hewlett-Packard Company  1987, 1988
 */

#ifndef  DeNormsOK
    #ifdef	_MCC86
        #define  DeNormsOK  0
    #else
        #define  DeNormsOK  1
    #endif
#endif

/* Exponenent bias for the two floating point representations */
#define	 EXP_BIAS         1023
#define	 EXP_BIAS_MNS1	  1022
#define  EXP_BIAS_f 	  127
#define	 EXP_BIAS_MNS1_f  126

/* Floating point constants */
#define  ALLONES  ((UInt32) ~0)
#define	 ZERO	  0.0
#define	 ZEROf	  ((float) 0.0)
#define	 HALF	  0.5
#define	 HALFf	  ((float) 0.5)
#define	 ONE	  1.0
#define	 ONEf	  ((float) 1.0)


extern   const UInt32  _hex_NaN[];
extern   const UInt32  _hex_NaNf;
#define  anNAN   (*((const double *) _hex_NaN))
#define  anNANf  (*((const float *) &_hex_NaNf))


/* Zero exponent macros */
#define  ExpZerop(n)         ( ! (n.half.hi & 0x7FF00000))
#define  ExpZeroFp(n)        ( ! (n.ivalue  & 0x7F800000))

/* Exponent all 1-bits macros */
#define  ExpAll1sp(n)        ((n.half.hi & 0x7FF00000) == 0x7FF00000)
#define  ExpAll1sFp(n)       ((n.ivalue  & 0x7F800000) == 0x7F800000)

/* Significand non-zero (i.e. at least one 1-bit) */
#define  SigNonZerop(n)      (n.half.lo | (n.half.hi & 0x000FFFFF))
#define  SigNonZeroFp(n)     (n.ivalue & 0x007FFFFF)

/* Significand zero */
#define  SigZerop(n)         ( ! SigNonZerop(n))
#define  SigZeroFp(n)        ( ! SigNonZeroFp(n))

/* Argument is positive infinity */
#define  isPOS_INFINITY(n)   ( ! (n.half.lo | (0x7FF00000 ^ n.half.hi)))
#define  isPOS_INFINITYf(n)  (n.ivalue == 0x7F800000)

/* Argument is either positive or negative infinity */
#define  isINFINITY(n)       (((n.half.hi & 0x7FFFFFFF) == 0x7FF00000) && \
			     ( ! n.half.lo))
#define  isINFINITYf(n)      ((n.ivalue & 0x7FFFFFFF) == 0x7F800000)

/* Argument is positive zero */
#define  isPOS_ZERO(n)       ( ! (n.half.hi | n.half.lo))
#define  isPOS_ZEROf(n)      ( ! n.ivalue)

/* Argument is not a zero quantity */
#define  isNonZero(n)	     (n.half.lo | (n.half.hi & 0x7FFFFFFF))
#define  isNonZerof(n)       (n.ivalue & 0x7FFFFFFF)

/* Argument is positive or negative zero */
#define  isZERO(n)           ( ! isNonZero(n) )
#define  isZEROf(n)          ( ! isNonZerof(n) )

/* Argument is a denormalized number */
#define  isDENORM(n)         (ExpZerop(n)  && SigNonZerop(n))
#define  isDENORMf(n)        (ExpZeroFp(n) && SigNonZeroFp(n))

/* Argument is an NaN (Not-A-Number) */
#define  isNAN(n)            (ExpAll1sp(n) && SigNonZerop(n))
#define  isNANf(n)           ((n.ivalue & 0x7FFFFFFF) > 0x7F800000)

/* Argument is negative */
#define  isNEG(n)            (n.half.hi & 0x80000000)
#define  isNEGf(n)           (n.ivalue  & 0x80000000)


/*	This macro is used to initialize a two element array of UInt32s
 * which will be accessed as a double.  This allows for the exact bit
 * pattern of a double to be entered as a constant into the source.
 * HD_INIT stands for hexadecimal_double_initialization.

 * Also define GET_HI and GET_LO macros.  These will extract hi order or
 * low order longs from a double.
 *
 */

#if _LITTLE_ENDIAN || __i960_BIG_ENDIAN__

    #define	HD_INIT(hi, lo)		{ lo, hi }
    #define	GET_HI(input)		(((unsigned long *)(&input))[1])
    #define	GET_LO(input)		(((unsigned long *)(&input))[0])

#else		/* _BIG_ENDIAN */

    #define	HD_INIT(hi, lo)		{ hi, lo }
    #define	GET_HI(input)		(((unsigned long *)(&input))[0])
    #define	GET_LO(input)		(((unsigned long *)(&input))[1])

#endif


/*    The following constants are used as parameters to _fp_error.  The
    bit patterns were randomly chosen given the constraints that bit
    15 is set if the calling routine was written in "C" and bit 8 is
    set if the routine has a result of type float.  These two constraints
    help _fp_error to construct the string name of the routine which
    encountered the error.
      The low sixteen bits are cleared because these bits will be set
    depending on the particular error encountered.		*/

#define  FP_acos	0xC00000
#define  FP_acosf	0xC10000
#define  FP_asin	0xC20000
#define  FP_asinf	0xC30000
#define  FP_atan	0xC40000
#define  FP_atanf	0xC50000
#define  FP_atan2	0xC60000
#define  FP_atan2f	0xC70000
#define  FP_ceil	0xC80000
#define  FP_ceilf	0xC90000
#define  FP_cos		0xCA0000
#define  FP_cosf	0xCB0000
#define  FP_cosh	0xCC0000
#define  FP_coshf	0xCD0000
#define  FP_exp		0xCE0000
#define  FP_expf	0xCF0000
#define  FP_fabs	0xD00000
#define  FP_fabsf	0xD10000
#define  FP_floor	0xD20000
#define  FP_floorf	0xD30000
#define  FP_fmod	0xD40000
#define  FP_fmodf	0xD50000
#define  FP_frexp	0xD60000
#define  FP_frexpf	0xD70000
#define  FP_ldexp	0xD80000
#define  FP_ldexpf	0xD90000
#define  FP_log		0xDA0000
#define  FP_logf	0xDB0000
#define  FP_log10	0xDC0000
#define  FP_log10f	0xDD0000
#define  FP_modf	0xDE0000
#define  FP_modff	0xDF0000
#define  FP_pow		0xE00000
#define  FP_powf	0xE10000
#define  FP_sin		0xE20000
#define  FP_sinf	0xE30000
#define  FP_sinh	0xE40000
#define  FP_sinhf	0xE50000
#define  FP_sqrt	0xE60000
#define  FP_sqrtf	0xE70000
#define  FP_tan		0xE80000
#define  FP_tanf	0xE90000
#define  FP_tanh	0xEA0000
#define  FP_tanhf	0xEB0000
#define  FP_frem	0xEC0000
#define  FP_fremf	0xED0000

#if	0					/* removed -- jpc 3/20/91 */
extern UInt16	_fp_status, _fp_control;
#endif

/* These macros correspond to the fp_control.h functions of the same name */
/* effectively removed	jpc - 3/20/91 */
#define _set_fp_status(stat)	(0)
#define _set_fp_control(ctrl)	(0)
#define _get_fp_status()	(0)
#define _get_fp_control()	(0)
#define _clear_fp_status()	(0)

/*==========================================================================*/
/*==========================================================================*/

extern int errno;

/*==========================================================================*/
/*==========================================================================*/


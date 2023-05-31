/************************************************************************/
/* THIS INFORMATION IS PROPRIETARY TO                                   */
/* MICROTEC RESEARCH, INC.                                              */
/*----------------------------------------------------------------------*/
/* Copyright (c) 1988, 1989, 1991 Microtec Research, Inc.               */
/* All rights reserved                                                  */
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

/************************************************************************
**
**	op_macro.h
**
**	macro dsefines for double and float off-line floating point
**	operations.
**
**	HISTORY:
**		01-30-89(mrm) Created
**
**************************************************************************/

/*	%W% %G%	*/

/*
**	These macros are used to reduce the number of typing errors when
**	entering masks.  If a macro name is mistyped, there will be an error
**	message printed, if a mask is mistyped, too bad.
*/
#define  M_00000000	0x00000000
#define  M_007FFFFF	0x007FFFFF
#define  M_80000000	0x80000000
#define  M_00080000	0x00080000
#define  M_00FFFFFF	0x00FFFFFF
#define  M_000FFFFF	0x000FFFFF
#define  M_800FFFFF	0x800FFFFF
#define  M_01000000	0x01000000
#define  M_00100000	0x00100000
#define  M_001FFFFF	0x001FFFFF
#define  M_00400000	0x00400000
#define  M_007FFFFF	0x007FFFFF
#define  M_00800000	0x00800000
#define  M_10000000	0x10000000
#define  M_1FFFFFFF	0x1FFFFFFF
#define  M_3FE00000	0x3FE00000
#define  M_3FF00000	0x3FF00000
#define	 M_3F000000	0x3F000000
#define  M_40000000	0x40000000
#define  M_7F7FFFFF	0x7F7FFFFF
#define  M_7FEFFFFF	0x7FEFFFFF
#define  M_7FF00000	0x7FF00000
#define  M_7FF80000	0x7FF80000
#define  M_7F000000	0x7F000000
#define  M_7F800000	0x7F800000
#define  M_7FC00000	0x7FC00000
#define  M_7FFFFFFF	0x7FFFFFFF
#define  M_BFF00000	0xBFF00000
#define  M_C1E00000	0xC1E00000
#define  M_FFFFFFFF	0xFFFFFFFF

#define	 EXP_BIAS         1023
#define	 EXP_BIAS_MNS1	  1022
#define  EXP_BIAS_f 	  127
#define  NumMantBits	  53	/* Number of mantissa bits of a Flt64 */
#define  NumSigBitsHi	  20	/* Number of significand bits in hi 32-bits of Flt64 */
#define  NumMantBits_f	  24	/* Number of mantissa bits of a Flt32 */
#define  NumSigBits_f	  23	/* Number of significand bits for a Flt32 */

/*
**	These macros check for NAN and inifinity of a double 
**	precision floating point numbers. (C double type)
*/
#define is_DOUBLE_INFINITY(n)		(((n.hi & M_7FFFFFFF) == M_7FF00000) && ( ! n.lo))
#define is_DOUBLE_NAN(n)		(((n.hi & M_7FFFFFFF) > M_7FF00000) || 		\
					 ((n.hi & M_7FFFFFFF) == M_7FF00000) && (n.lo))
#define is_DOUBLE_INIFINITY_OR_NAN(n)	((n.hi & M_7FFFFFFF) >= M_7FF00000)
#define get_DOUBLE_SIGN(n)		(n.hi & M_80000000)
extern void	_set_double_infinity();
extern void	_set_double_nan();


/* 
** These macros are used to determine if the float numbers are of specific
** types.  Most of the macros are self explanatory by their names.  Note that 
** the macros are not strictly boolean macros.  The values returned may be 
** different than 1 and 0.  Any non-zero value should be interpreted as TRUE.
*/

#ifdef	isFLOAT_ZERO
#undef	isFLOAT_ZERO
#endif
#define  isFLOAT_ZERO(n)        (!(n & M_7FFFFFFF))

#ifdef	isFLOAT_POS_ZERO
#undef	isFLOAT_POS_ZERO
#endif
#define  isFLOAT_POS_ZERO(n)    (!(n))

#ifdef	isFLOAT_NEG_ZERO
#undef	isFLOAT_NEG_ZERO
#endif
#define  isFLOAT_NEG_ZERO(n)    (n == M_80000000)

#ifdef	isFLOAT_DENORM
#undef	isFLOAT_DENORM
#endif
#define  isFLOAT_DENORM(n)      (((n & M_7F800000) == 0x0) && (n & M_007FFFFF)) ??

#ifdef	isFLOAT_NEG
#undef	isFLOAT_NEG
#endif
#define  isFLOAT_NEG(n)         (n & M_80000000)

#ifdef	FLOAT_SIGN
#undef	FLOAT_SIGN
#endif
#define  FLOAT_SIGN(n)		(n & M_80000000)

#ifdef	FLOAT_LEADING_BIT
#undef	FLOAT_LEADING_BIT
#endif
#define  FLOAT_LEADING_BIT	(M_00800000)

/*
**	These macros check for NAN and inifinity of a single 
**	precision floating point numbers. (C float type)
*/
#define is_FLOAT_INFINITY(n)		(((n & M_7FFFFFFF) == M_7F800000))
#define is_FLOAT_NAN(n)			((n & M_7FFFFFFF) > M_7F800000) 
#define is_FLOAT_INIFINITY_OR_NAN(n)	((n & M_7FFFFFFF) >= M_7F800000)
#define  get_FLOAT_SIGN(n)		(n & M_80000000)
extern void	_set_float_nan();
extern void	_set_float_infinity();




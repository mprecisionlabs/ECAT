
/************************************************************************/
/* Copyright (c) 1988-1991 Microtec Research, Inc.			*/
/* ALL RIGHTS RESERVED							*/
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

/*  <float.h>   Microtec Research, Inc. ANSI C Compiler  */
/* %W% %G% */


#ifndef __FLOAT_H
#define __FLOAT_H	1

#define FLT_RADIX		2
#define FLT_ROUNDS		(-1)

#define FLT_MANT_DIG		24
#define FLT_EPSILON		1.19209290E-07F
#define FLT_DIG			6
#define FLT_MIN_EXP		(-125)
#define FLT_MIN			1.17549435E-38F
#define FLT_MIN_10_EXP		(-37)
#define FLT_MAX_EXP		(+128)
#define FLT_MAX			3.40282347E+38F
#define FLT_MAX_10_EXP		(+38)

#if __OPTION_VALUE ("-Kq")			/* Is -Kq option being used ? */

    #define DBL_MANT_DIG	FLT_MANT_DIG
    #define DBL_EPSILON		FLT_EPSILON
    #define DBL_DIG		FLT_DIG
    #define DBL_MIN_EXP		FLT_MIN_EXP
    #define DBL_MIN		FLT_MIN	
    #define DBL_MIN_10_EXP	FLT_MIN_10_EXP
    #define DBL_MAX_EXP		FLT_MAX_EXP
    #define DBL_MAX		FLT_MAX
    #define DBL_MAX_10_EXP	FLT_MAX_10_EXP	

#else

    #define DBL_MANT_DIG	53
    #define DBL_EPSILON		2.2204460492503131E-16
    #define DBL_DIG		15
    #define DBL_MIN_EXP		(-1021)
    #define DBL_MIN		2.2250738585072014E-308
    #define DBL_MIN_10_EXP	(-307)
    #define DBL_MAX_EXP		(+1024)
    #define DBL_MAX		1.7976931348623157E+308
    #define DBL_MAX_10_EXP	(+308)

#endif

#define LDBL_MANT_DIG		DBL_MANT_DIG
#define LDBL_EPSILON		DBL_EPSILON
#define LDBL_DIG		DBL_DIG
#define LDBL_MIN_EXP		DBL_MIN_EXP
#define LDBL_MIN		DBL_MIN
#define LDBL_MIN_10_EXP		DBL_MIN_10_EXP
#define LDBL_MAX_EXP		DBL_MAX_EXP
#define LDBL_MAX		DBL_MAX
#define LDBL_MAX_10_EXP		DBL_MAX_10_EXP

#endif /* end __FLOAT_H */

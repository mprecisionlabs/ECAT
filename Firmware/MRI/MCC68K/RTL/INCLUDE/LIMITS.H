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

/*  <limits.h>   Microtec Research, Inc. ANSI C Compiler  */
/*	%W% %G%	*/


#ifndef __LIMITS_H
#define __LIMITS_H	1

#define CHAR_BIT		8

#define SCHAR_MIN		(-128)
#define SCHAR_MAX		127

#define UCHAR_MAX		255U

#if _CHAR_SIGNED
	#define CHAR_MIN	SCHAR_MIN
	#define CHAR_MAX	SCHAR_MAX
#endif
#if _CHAR_UNSIGNED
	#define CHAR_MIN	0U
	#define CHAR_MAX	UCHAR_MAX
#endif

#define MB_LEN_MAX		1

#define SHRT_MIN		(-32767-1)
#define SHRT_MAX		32767

#define USHRT_MAX		65535U

#if _SIZEOF_INT == _SIZEOF_SHORT
	#define INT_MIN		SHRT_MIN
	#define INT_MAX		SHRT_MAX

	#define UINT_MAX	USHRT_MAX
#endif

#if _SIZEOF_INT == _SIZEOF_LONG
	#define INT_MIN		LONG_MIN
	#define INT_MAX		LONG_MAX

	#define UINT_MAX	ULONG_MAX
#endif

#define LONG_MIN		(-2147483647-1)
#define LONG_MAX		2147483647

#define ULONG_MAX		4294967295U

#endif /* end __LIMITS_H */

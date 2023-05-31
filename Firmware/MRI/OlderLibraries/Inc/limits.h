
/************************************************************************/
/* Copyright (c) 1988-1989  Microtec Research, Inc.			*/
/* ALL RIGHTS RESERVED							*/
/************************************************************************/

/*  <limits.h>   Microtec Research, Inc. ANSI C Compiler  */
/* @(#)limits.h	1.2 4/13/90 */


#ifndef __LIMITS_H
#define __LIMITS_H

#define CHAR_BIT		8

#define SCHAR_MIN		(-128)
#define SCHAR_MAX		127

#define UCHAR_MIN		0
#define UCHAR_MAX		255

#define CHAR_MIN		SCHAR_MIN
#define CHAR_MAX		SCHAR_MAX

#define MB_LEN_MAX		1

#define SHRT_MIN		(-32768)
#define SHRT_MAX		32767

#define USHRT_MIN		0
#define USHRT_MAX		65535

#define INT_MIN			(-2147483648)
#define INT_MAX			2147483647

#define UINT_MIN		0
#define UINT_MAX		4294967295

#define LONG_MIN		(-2147483648)
#define LONG_MAX		2147483647

#define ULONG_MIN		0
#define ULONG_MAX		4294967295

#endif /* end __LIMITS_H */

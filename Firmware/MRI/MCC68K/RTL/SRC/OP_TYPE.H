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

/*	%W% %G%	*/
/*
Change history:
	11/03/93 - jpc  Converted "INTEL" and "MOTOROLA" to "_LITTLE_ENDIAN"
			and _BIG_ENDIAN, respectively; removed special handling
			of NaN's for little-endian machines -- all NaN's are
			handled the same now.
	11/19/93 - jpc	Added in support for 960 big endian
*/

#ifndef NLS_TYPES_INCLUDED

#define NLS_TYPES_INCLUDED	1

typedef	unsigned char	UChar;
typedef	short		Int16;
typedef	unsigned short	UInt16;
typedef	long		Int32;
typedef	unsigned long	UInt32;

typedef	void	*void_ptr;	

/*    The following union is used so double precision numbers may be
 *  accessed either as doubles or as bit patterns. 		 */

typedef union {
	double  value;
	struct {
#if _LITTLE_ENDIAN || __i960_BIG_ENDIAN__
		UInt32  lo, hi;
#else	/* _BIG_ENDIAN */
		UInt32  hi, lo;
#endif
	} half;
} realnumber;

/*    This union allows the access of single precision floating point
 *  numbers as floats or as bit patterns.	*/

typedef union {
	double	value;		/* changed from float to avoid "argument is */
	UInt32  ivalue;		/* incompatible with formal parameters" error */
} realnumberF;

#endif	/* NLS_TYPES_INCLUDED */

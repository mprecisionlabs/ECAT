#include "lib_top.h"


#if EXCLUDE_s_sbrk
    #pragma option -Qws			/* Prevent compiler warning */
#else
 

/************************************************************************/
/* THIS INFORMATION IS PROPRIETARY TO					*/
/* MICROTEC RESEARCH, INC.						*/
/* 2350 Mission College Blvd.						*/
/* Santa Clara, CA 95054						*/
/* USA									*/
/*----------------------------------------------------------------------*/
/* Copyright (c) Microtec Research, Inc. 1986-91. All rights reserved. 	*/
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

/************************************************************************/
/*									*/
/*  char *sbrk(int incr);						*/
/*									*/
/*  Allocate memory							*/
/*									*/
/*  sbrk:								*/
/*	Allocates memory, in general for use by heap management		*/
/*	routines.  If successful, sbrk returns the address of the	*/
/*	allocated memory.  If memory is not available, sbrk returns -1. */
/*									*/
/************************************************************************/

#include "s_info.mch"

#define	ALIGN_MASK	3	/* Word alignment for all 68k processors */

char *sbrk(unsigned incr)
{
	char *oldBreak, *newBreak;

	oldBreak = _HEAP =		/* Do alignment fixup if necessary */
		(char *) (((unsigned long) _HEAP + ALIGN_MASK) & (~ALIGN_MASK));

	if (incr) {
		newBreak = oldBreak + incr;
		if ((newBreak > (_HEAP_START + _HEAP_SIZE)) /* out of space ? */
			|| (newBreak <= _HEAP_START))       /* memory wrap ? */
		    return (char *) (-1);
		_HEAP = newBreak;
	}
	return oldBreak;
}
 
#endif /* EXCLUDE_s_sbrk */
 

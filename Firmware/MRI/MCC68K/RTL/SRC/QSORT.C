#include "lib_top.h"


#if EXCLUDE_qsort
    #pragma option -Qws			/* Prevent compiler warning */
#else
 
/****************************************************************************/
/* THIS INFORMATION IS PROPRIETARY TO                                       */
/* MICROTEC RESEARCH, INC.                                                  */
/*--------------------------------------------------------------------------*/
/* Copyright (c) 1987, 1988, 1989, 1991, 1992 Microtec Research, Inc.       */
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


/* %Z%  %M%  %I%  %G% */

/************************************************************************
 *  qsort.c
 *			Q S O R T   functions
 *
 *	Copyright 1985 by Microtec Research Inc.  All rights reserved.
 *
 *	This function is an implementation of the quick-sort algorithm.
 *	void qsort(base,num,siz,cmp)
 *		base is a pointer to the base of the data
 *		num is the number of elements
 *		siz is the size of an element in bytes
 *		cmp is a pointer to a comparison routine which takes two
 *		    pointers to items and returns an integer <, =, or > 0
 *		    according as item one is <, =, or > item two.
 *	
 *  written by H. Yuen  03/21/85
 *	03/18/91 jpc: Made re-entrant; some tweeks for speed
 *	11/10/93 jpc: DIFF now is unsigned; siz cast to unsigned in
 *			comparison
 *
 ************************************************************************/
/* (last mod 03/26/85  HY) */

#include <stddef.h>
#include <stdlib.h>

					/* run time reduction in strength */
#define MULT(X,Y) ((X)==sizeof(char *) ? ((Y) * sizeof(char *)) : ((X)*(Y)))

#if _SIZEOF_PTR == 2
    typedef unsigned short	DIFF;
#else
    typedef unsigned long	DIFF;
#endif

static swap(register char *a, register char *b, int nbyte)
{
	char tmp;

	for (++nbyte, --a, --b; --nbyte;) {
		tmp = *++a;
		*a = *++b;
		*b = tmp;
	}
}

static qqsort(char *bas, char *limit, int siz,
	int (*qcmp)(const void *, const void *))
{
	register char *i, *j;
	register char *pivot;
	unsigned n;

qstart:
	if ((n = (DIFF)limit - (DIFF)bas) > (unsigned) siz ) {
		i = bas;
		j = limit - siz;
		pivot = bas + MULT(siz,(n/(siz*2)));/* use middle one as pivot*/
		do {
		    while (i < j && (*qcmp)(i,pivot) <= 0)
			i += siz;
		    while (j > i && (*qcmp)(j,pivot) >= 0)
			j -= siz;
		    if (i < j) {
			swap(i,j,siz);
			i += siz;
		    }
		} while (i < j);
		while (pivot < i && (*qcmp)(i,pivot) >= 0) i -= siz;
		if (i != pivot)			/* put pivot element in */
		    swap(i,pivot,siz);		/* correct "middle" position */
		if (i > bas+siz)
		    qqsort(bas,i,siz,qcmp);
		bas = i + siz;
		goto qstart;			/* qqsort(i+siz,limit); */
	}
}

void qsort(void *base, size_t num, size_t siz,
	   int (*cmp)(const void *, const void *))

{
	qqsort(base, (char *) base + MULT (siz, num), siz, cmp);
}

 
#endif /* EXCLUDE_qsort */
 

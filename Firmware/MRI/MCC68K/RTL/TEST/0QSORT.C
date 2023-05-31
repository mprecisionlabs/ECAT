#include "lib_top.h"


#pragma	option	-nx

/************************************************************************/
/* THIS INFORMATION IS PROPRIETARY TO					*/
/* MICROTEC RESEARCH, INC.						*/
/* 2350 Mission College Blvd.						*/
/* Santa Clara, CA 95054						*/
/* USA									*/
/*----------------------------------------------------------------------*/
/* Copyright (c) 1991-1992  Microtec Research, Inc.			*/
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

/* pgm for testing qsort	03/21/85  HY */

/*	%W% %G%	*/

/* (last mod 03/26/84  HY) */

#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>

#include "message.h"

#define NE	100
int nq;			/* number of recursive calls to qsort */
int ncmp, nxchg;	/* number of compares; exchanges */
int xt;			/* execution (elapsed) time */

int errorCount;

long time_d ()
{
	return 0;
}

long timediff ()
{
	return 0;
}

main()
{
	register i, j;
	char *s;
	long t0, t1;
	int  a[NE];
	int icompar();

    for (j = 1; j <= 4; j++) {
	switch (j) {
	  case 1:			/* test #1: ordered set */
		for (i=0; i < NE; i++) a[i] = i;
		s = "ordered set";
		break;
	  case 2:			/* test #2: random set */
		for (i=0; i < NE; i++) a[i] = rand() & 255;
		s = "random set";
		break;
	  case 3:			/* test #3: all equal */
		for (i=0; i < NE; i++) a[i] = 100;
		s = "all equal ";
		break;
	  case 4:			/* test #4: inv ordered */
		for (i=0; i < NE; i++) a[i] = NE - 1 -i;
		s = "inv ordered";
		break;
	}
	nq = ncmp = nxchg = 0;
	qsort(a, NE, sizeof(int), icompar);	
	result(a,NE,s);			/* Check results */
    }
    if (errorCount)
	printf (ERRORS);
    else
	printf (NO_ERRORS);
}

result(a,n,s)		/* print results and check array */
int a[];
register int n;
char *s;
{
	register char *p;

	while (--n > 0)
	    if (a[n] < a[n-1])
		{ 
		printf(" *** error at a[%d] ***\n",n); 
		errorCount++;
		return; 
		}
}

icompar(ip,jp)
int *ip, *jp;
{
	ncmp++;
	return(*ip - *jp);
}
 

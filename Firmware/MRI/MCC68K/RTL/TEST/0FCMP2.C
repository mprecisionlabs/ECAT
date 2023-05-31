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

/* pgm to test floating point compares  by H. Yuen  12/23/86 */

/************************************************************************/
/* (last mod 06/27/88  HY) 						*/
/*	7/11/89	jpc: Changed some numbers cuz they were getting 	*/
/*			changed to zero and infinity			*/
/*			Also added test complete			*/
/************************************************************************/

/*	%W% %G%	*/

#include <stdio.h>

#include "message.h"

#define SINGLE_PREC 1

struct testcase {
#ifdef SINGLE_PREC
	float	d1, d2;
	float	x1, x2;
#else
	double	d1, d2;
	double	x1, x2;
#endif
} cases[] = {
		0.0,		0.0,		0.01,	0.03,
		-0.0,		-0.0,		0.01,	0.03,
		-0.0,		0.0,		0.01,	0.03,
		1.0,		1.0,		0.005,	0.007,
		-1.0,		-1.0,		0.002,	0.003,
		65590.0,	65590.0,	0.5,	0.7,
		-1000.0,	-1000.0,	0.5,	0.6,
		987654321.0,	987654321.0,	100.,	200.,
		-123456789.0,	-123456789.0,	100.,	200.,
		3.1415926535,	3.1415926535,	0.00005,0.00007,
		1.2345e-6,	1.2345e-6,	4.5e-3,	4.6e-3,
		-1.23456e-7,	-1.23456e-7,	6.3e-4,	6.4e-4,
		9.45e-36,	9.45e-36,	1e-37,	1.2e-37,
		-9.45e-37,	-9.45e-37,	1e-37,	1.2e-37,
		1.79e+37,	1.79e+37,	3e+35,	3.1e+35,
		-1.77e+38,	-1.77e+38,	3e+35,	3.1e+35
		
	 };

main()
{
	register struct testcase *p;
	register char *cc;
	register int n, i, j, cmp;
	int	ncase;
	static	char eq[6] =  { 0, 1, 1, 0, 1, 0 };	/* cmp for == */
	static	char lt[6] =  { 1, 0, 0, 0, 1, 1 };	/* cmp for <  */
	static	char gt[6] =  { 1, 0, 1, 1, 0, 0 };	/* cmp for >  */
	char err[6];
	static	char cond[][3] = { "==", "!=", " <", "<=", " >", ">=" };
	static	errorCount;

	p = &cases[0];
	for (i=0; i<6; i++)
		err[i] = 0;
	ncase = sizeof(cases) / sizeof(struct testcase);
	for (i=0; --ncase >= 0; i++, p++) {
	    cc = eq;
	    for (cmp=0; cmp < 3; ++cmp) {
		if (p->d1 == p->d2 && cc[0] != 0) err[0] = 1;
		if (p->d1 != p->d2 && cc[1] != 0) err[1] = 1;
		if (p->d1  < p->d2 && cc[2] != 0) err[2] = 1;
		if (p->d1 <= p->d2 && cc[3] != 0) err[3] = 1;
		if (p->d1  > p->d2 && cc[4] != 0) err[4] = 1;
		if (p->d1 >= p->d2 && cc[5] != 0) err[5] = 1;
	
		for (j=0; j < 6; ++j)
		    if (err[j] != 0) {
			printf ("fcmp error:  %g  %s  %g cmp = %d ncase = %d "
			    "%g %g\n", (double) p->d1, cond[j], (double) p->d2,
			    cmp, ncase, (double)p->x1, (double)p->x2);
			errorCount++;
			err[j] = 0;
		    }
		if (cmp == 0) {		/* have just compared for == */
		    p->d2 += p->x1;	/* make d1 < d2 */
		    cc = lt;
		}
		else if (cmp == 1) {	/* have just compared for < */
		    p->d1 += p->x2;	/* make d1 > d2 */
		    cc = gt;
		}
	    }
	}
	if (errorCount)
	    printf (ERRORS);
	else
	    printf (NO_ERRORS);
}
 

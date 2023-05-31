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

/* pgm to test atoi(), atol() - written by H. Yuen  05/24/85 */

/*	%W% %G%	*/

#include <stdio.h>
#include "message.h"

const struct testcase {
	char str[20];
	long  value;
} cases[] = {
		"  002A",	2,
		"10h",		10,
		"+1234",	1234,
		"abcd",		0,
		" -sorry",	0,
		"-32700",	-32700,
		"0x1234",	0,
		" 65540",	65540,
		"+2147483640",	2147483640,
		"  -65550",	-65550,
		" +02468 0",	2468,
		"\t\t 9876",	9876,
		"000111",	111,
		"",		0 };

main()
{
	register struct testcase *p;
	register int n, i, j;
	long	jl;
	long    atol();
	int	errorCount = 0;

	p = (struct testcase *) &cases[0];	/**** test atoi() ****/
	for (i=0; ; i++, p++) {
		if (p->str[0] == 0) break;
		j = atoi(p->str);
		if (j != (int)p->value) {
			printf("atoi error %2d  %s => %d  (%d)\n",
			    i, p->str, j, (int)p->value);
			errorCount++;
			}
	}

	p = (struct testcase *) &cases[0];	/**** test atol() ****/
	for (i=0; ; i++, p++) {
		if (p->str[0] == 0) break;
		jl = atol(p->str);
		if (jl != p->value) {
			printf("atol error %2d  %s => %ld  (%ld)\n",
			    i, p->str, jl, p->value);
			errorCount++;
			}
	}
	if (errorCount)
		printf (ERRORS);
	else
		printf (NO_ERRORS);
}
 

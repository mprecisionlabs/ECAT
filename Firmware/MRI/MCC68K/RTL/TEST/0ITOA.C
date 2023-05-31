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

/* pgm to test itoa(), ltoa() - written by H. Yuen  06/04/85 */

/*	%W% %G%	*/

#include <stdio.h>

#include "message.h"

const struct testcase {
	char str[20];
	long  value;
} cases[] = {
		"1234",		1234,
		"-32700",	-32700,
		"0",		0,
		"1000",		1000,
		"-99",		-99,
		"",		0,	/* end of cases for itoa() */
		"1234",		1234,
		"-32700",	-32700,
		"65540",	65540,
		"2147483640",	2147483640,
		"-65550",	-65550,
		"0",		0,
		"1000",		1000,
		"-99",		-99,
		"",		0 };

main()
{
	register struct testcase *p;
	register int n, i, j;
	char  buf[14];
	int   errorCount = 0;

	p = (struct testcase *) &cases[0];		/**** test itoa() ****/
	for (i=0; ; i++, p++) {
		if (p->str[0] == 0) break;

		j = itoa((int)p->value,buf);
		if (j != strlen(p->str) || strcmp(p->str,buf)) {
			printf("itoa error %2d  %d => %s  (%d)\n",
			    i, (int)p->value, buf, j);
			errorCount++;
			}
	}

	p++;				/**** test ltoa() ****/
	for (i=0; ; i++, p++) {
		if (p->str[0] == 0) break;

		j = ltoa(p->value,buf);
		if (j != strlen(p->str) || strcmp(p->str,buf)) {
			printf("ltoa error %2d  %ld => %s  (%d)\n",
			    i, p->value, buf, j);
			errorCount++;
			}
	}

	if (errorCount) 
	    printf (ERRORS);
	else
	    printf (NO_ERRORS);
}
 

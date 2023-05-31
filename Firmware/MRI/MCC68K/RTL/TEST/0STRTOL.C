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

/* pgm to test strtol() - written by H. Yuen  05/21/85 */

/*	%W% %G%	*/

#include <stdio.h>
#include <stdlib.h>

#include "message.h"

struct testcase {
	char str[20];
	int  base;
	int  value;
} cases[] = {
		"  002A", 16, 0x2A,
		"+1234",  16, 0x1234,
		"abcd",   16, 0xABCD,
		" -sorry",16, 0,
		"0x002a", 16, 0x2A,
		"0x1234", 16, 0x1234,
		"0xaBcD", 16, 0xABCD,
		"0xsorry",16, 0,
		" -002A", 0,  -2,
		"0x1234", 0,  0x1234,
		"0xsorry",0,  0,
		" +02468",0,  0246,
		"\t 9876",0,  9876,
		"0x1234", 8,  0,
		"13579",  8,  01357,
		"000111", 10, 111,
		"",	  0,  0 };

main()
{
	register struct testcase *p;
	register int n, i, j;
	int errorCount = 0;

	p = &cases[0];
	for (i=0; ; i++, p++) {
		if (p->str[0] == 0) break;
		j = (int)strtol(p->str,(char **)0,p->base);
		if (j != p->value) {
			printf("error %2d  %s  %d => %d  (%d)\n",
			    i, p->str, p->base, j, p->value);
			errorCount++;
			}
	}
	if (errorCount)
	    printf (ERRORS);
	else
	    printf (NO_ERRORS);
}
 

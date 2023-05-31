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

/*    %W% %G% */

/* pgm to test itostr(), ltostr() - written by H. Yuen  06/06/85 */

/*	@(#)0itostr.c	1.1 3/14/91	*/

#include <stdio.h>

#include "message.h"

struct testcase {
	char str[30];
	int  base;
	long  value;
} cases[] = {
		"65535",   10,	65535,
		"3600",	   10,	3600,
		"0",	   10,	0,
		"377",	    8,	255,
		"10306",    8,  010306,
		"2A",	   16,	0x2A,
		"AB0",	   16,	0xAB0,
		"0",	   16,	0,
		"11000101", 2,	0xC5,
		"101011",   2,	0x2B,
		"1000",	    0,	1000,
		"3737",	   37,	3737,
		"",	    0,	0,	/* end of cases for itostr() */
		"135792468",0,	135792468,
		"FF00000", 16,	0x0FF00000,
		"2147483647",0,	0x7FFFFFFF,
		"36115274", 8,	0x789ABC,
		"11110001001101010111100", 2, 0x789ABC,
		"4294967294",50, -2,
		"",	  0,  0 };
main()
{
	register struct testcase *p;
	register int n, i, j;
	char  buf[30];
	int errorCount = 0;
     
        cases[19].str[0] = -1;
	p = &cases[0];			/**** test itostr() ****/
	for (i=0; ; i++, p++) {
		if (p->str[0] == 0) break;

		j = itostr((int)p->value,buf,p->base);
		if (j != strlen(p->str) || strcmp(p->str,buf)) {
			printf("itostr error %2d  %s [%d] => %s  (%d)\n",
			    i, p->str, p->base, buf, j);
			errorCount++;
			}
	}

	p = &cases[0];			/**** test ltostr() ****/
	for (i=0; ; i++, p++) {
		if (p->str[0] == 0) continue;
		if (p->str[0] == -1) break;

		j = ltostr(p->value,buf,p->base);
		if (j != strlen(p->str) || strcmp(p->str,buf)) {
			printf("ltostr error %2d  %s [%d] => %s  (%d)\n",
			    i, p->str, p->base, buf, j);
			errorCount++;
			}
	}
	
	if (errorCount)
	    printf (ERRORS);
	else
	    printf (NO_ERRORS);
}
 

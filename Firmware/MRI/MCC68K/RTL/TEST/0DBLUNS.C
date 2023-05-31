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

#include <stdio.h>
#include "message.h"

static float f=16.;
static unsigned char c=11;

float *f1() { return &f; }
unsigned char *c1() { return &c;}

int errorCount;

main ()
{
    *c1() -= *f1();
    if (*c1() != 251) {
        printf ("test 1:  %u\n", *c1());
	errorCount++;
	}

    *c1() = 11;
    *c1() -= *f1();
    if (*c1() != 251) {
	printf ("test 2:  %u\n", *c1());
	errorCount++;
	}

    if (errorCount) {
	printf (ERRORS);
	exit (1);
	}
    else {
	printf (NO_ERRORS);
	exit (0);
	}
}
 

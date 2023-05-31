#include "lib_top.h"

/************************************************************************/
/* THIS INFORMATION IS PROPRIETARY TO                                   */
/* MICROTEC RESEARCH, INC.                                              */
/* 2350 Mission College Blvd.                                           */
/* Santa Clara, CA 95054                                                */
/* USA                                                                  */
/*----------------------------------------------------------------------*/
/* Copyright (c) 1994  Microtec Research, Inc.                          */
/* ALL RIGHTS RESERVED                                                  */
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

#include <stdio.h>
#include <stdarg.h>
#include "message.h"

struct foo { char c; };

struct init {
	int int1, int2;
	} startup;

struct foo x[] = { 't', 'e', 's', 't' };
char y[] = "test";

static int errorCount;

void bar(struct init x, ...)
{
    char *p = y;
    char c;
    va_list va;
    struct foo t;

    va_start(va, x);
    do {
	t = va_arg(va, struct foo);
	c = *p++;
	if (t.c != c) {
	    printf ("Incorrect argument: expected: %c  actual: %c\n",
		c, t.c);
	    errorCount++;
	}
    } while (*p);

    va_end(va);
}

void main()
{
    bar(startup,
	x[ 0], /* t */
	x[ 1], /* e */
	x[ 2], /* s */
	x[ 3]  /* t */
	);

    if (errorCount) {
	printf (ERRORS);
    } else {
	printf (NO_ERRORS);
    }
}

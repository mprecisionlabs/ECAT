#include "lib_top.h"


#pragma	option	-nx

/************************************************************************/
/* THIS INFORMATION IS PROPRIETARY TO					*/
/* MICROTEC RESEARCH, INC.						*/
/* 2350 Mission College Blvd.						*/
/* Santa Clara, CA 95054						*/
/* USA									*/
/*----------------------------------------------------------------------*/
/* Copyright (c) 1992  Microtec Research, Inc.				*/
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

/*	%W% %G%	*/

/************************************************************************/
/*	This routine simply tests to insure that 8086 and z80 style	*/
/*	libraries are set up correctly.					*/
/************************************************************************/

#include <stdio.h>

#include "message.h"

main ()
{
char string[256];

    sscanf ("TestString", "%s", string);

    if (! strcmp (string, "TestString"))
	printf (NO_ERRORS);
    else {
	printf ("string = %s\n", string);
	printf (ERRORS);
	}
}

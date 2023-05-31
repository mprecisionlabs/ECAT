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


/*	%W% %G%	*/

#include <mriext.h>
#include <stdio.h>

#include "message.h"

main ()
{
char	string1[24], string2[24];
int	i;
int	error = 0;

#if ! MCC86	/* test doesn't run on '86 */

for (i=0; i<24;)
    {
    string1[i++] = 0x5;
    string1[i++] = 0xa;
    }

swab (string1, string2, 24);

for (i=0; i<24;)
    {
    if (string2[i++] != 0xa)
	{
	printf ("Error\n");
	error++;
	}
    if (string2[i++] != 0x5)
	{
	printf ("Error\n");
	error++;
	}
    }

#endif

    if (error)
	printf (ERRORS);
    else
	printf (NO_ERRORS);
}
 

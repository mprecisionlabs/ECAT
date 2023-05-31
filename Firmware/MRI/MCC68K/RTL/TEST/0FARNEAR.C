#include "lib_top.h"


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


/************************************************************************/
/*	Quick test to check that far and near keywords are working on	*/
/*	mcc86.								*/
/************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include "message.h"

#if _MCC86
    #define FAR far
    #define NEAR near
#else
    #define FAR
    #define NEAR
#endif

static	int NEAR	nearTest0;
static	int NEAR	nearTest5 = 5;

static	int FAR		farTest0;
static	int FAR		farTest5 = 5;

main ()
{
char * temp;

    memset (temp = malloc (512), 0xa, 512);	/* First run malloc to see */
						/* if heap overwrites any far */
						/* or near data */
    if ((nearTest0 != 0) || (nearTest5 != 5) || (farTest0 != 0) || 
	(farTest5 != 5)) {
	printf (ERRORS);
	exit (1);
	}

    printf (NO_ERRORS);
}
 

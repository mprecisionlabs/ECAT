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


/**************************************************************************/
/*	This routine contains example tests from the ANSI spec for fscanf */
/*									  */
/*	A couple of extra tests added -- problems customers found	  */
/**************************************************************************/
/*	%W% %G%	*/

#include <stdio.h>
#include "message.h"

/*
Change history:
	11/20/93 jpc -	Modified DELTA to make opropriate for -Kq option.
*/

#if __OPTION_VALUE ("-Kq")
    #define	DELTA		(1.e-5)
#else
    #define	DELTA		(1.e-15)
#endif

#define TEST_HI_LO(x,y)	((x < (y * (1. - DELTA))) || (x > (y * (1. + DELTA))))

char source[][35] = {
		"2 quarts of oil",
		"-12.5degrees Celsius",
		"lots of luck",
		"10.0LBS    of    \n  fertilizer",
		"100ergs of energy",
		""
		};

float quantRes[] = {2., -12.5, -12.5, 10.0, 10.0, 10.0};

char unitsRes[][10] = {"quarts", "degrees", "degrees", "LBS", "LBS", "LBS"};

char itemRes[][15] = {"oil", "oil", "oil", "fertilizer", "fertilizer",
		"fertilizer"};

int countRes[] = {3, 2, 0, 3, 0, EOF};

main()
{
int i,j;
double r,s,t;
char c1,k,c2;
int l;
char str[50];
char dummy;
int error = 0;
/**/
float quant;
int count;
char units[21];
char item[21];
static char cstring[] = "dc 36000 3700ff";
unsigned long commands[10];


    sscanf("  25   54.32e-1     Thompson  ",
		"  %d     %lf           %s      ",&i,&r,str);
    if (i != 25) {
	printf ("Error 1: expected 25 got %d\n", i);
	error++;
	}
    if (TEST_HI_LO (r, 5.432)) {
	printf ("Error 2: expected 5.432 got %f\n", r);
	error++;
	}
    if (strcmp(str,"Thompson") != 0) {
	printf ("Error 3: expected <Thompson> got <%s>\n", str);
	error++;
	}

    sscanf("56789 0123 45a72","%2d %lf %*d %2s",&i,&r,str);
    if (i != 56) {
	printf ("Error 4: expected 56 got %d\n", i);
	error++;
	}
    if (r != 789.0) {
	printf ("Error 5: expected 789.0 got %f\n", r);
	error++;
	}
    if (strcmp(str,"45") != 0) {
	printf ("Error 6: expected <45> got <%s>\n", str);
	error++;
	}

    for (i=0; i < 6; i++) {
	count = sscanf (source[i], "%f%20s of %20s", &quant, units, item);
	if (quant != quantRes[i]) {
	    error++;
	    printf ("Error: quant: expected = %g, got = %g, i = %d\n",
		quantRes[i], quant, i);
	    }
	if (strcmp (units, unitsRes[i])) {
	    error++;
	    printf ("Error: units: expected = %s, got = %s, i = %d\n",
		unitsRes[i], units, i);
	    }
	if (strcmp (item, itemRes[i])) {
	    error++;
	    printf ("Error: item: expected = %s, got = %s, i = %d\n",
		itemRes[i], item, i);
	    }
	if (count != countRes[i]) {
	    error++;
	    printf ("Error: count: expected = %d, got = %d, i = %d\n",
		countRes[i], count, i);
	    }
	}

/*	The following is a test case which tests a problem which a customer
	found */

    count = sscanf (cstring,
	"%*s %6lx %6lx %6lx %6lx %6lx %6lx %6lx %6lx %6lx %6lx",
                &commands[0],
                &commands[1],
                &commands[2],
                &commands[3],
                &commands[4],
                &commands[5],
                &commands[6],
                &commands[7],
                &commands[8],
                &commands[9]);

    if (commands[0] != 0x36000) {
	error++;
	printf("Error:commands[0]: expected 0x36000, got 0x%lx, misc test 1\n",
		commands[0]);
	}

    if (commands[1] != 0x3700ff) {
	error++;
	printf("Error:commands[1]: expected 0x3700ff, got 0x%lx, misc test 1\n",
		commands[1]);
	}

    if (count != 2) {
	error++;
	printf ("Error: count: expected: 2, got %d, misc test 1\n", count);
	}

    if (error)
	printf(ERRORS);
    else
	printf(NO_ERRORS);

}

 

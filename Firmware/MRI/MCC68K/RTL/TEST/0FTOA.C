#include "lib_top.h"


#pragma	option	-nx

/****************************************************************************/
/* THIS INFORMATION IS PROPRIETARY TO                                       */
/* MICROTEC RESEARCH, INC.                                                  */
/*--------------------------------------------------------------------------*/
/* Copyright (c) 1992 Microtec Research, Inc.       			    */
/* All rights reserved                                                      */
/****************************************************************************/
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

#include <stdio.h>
#include <mriext.h>
#include "message.h"

char c_msg0[100];
char c_msg1[100];
double d_temp0;
int i;
int errorCount;
int returnVal;

main()
{
    d_temp0 = 4321.56789;

    if (9 != (returnVal = ftoa (d_temp0, c_msg0, 'f', 4)))
	{
	printf("Error: 1: bad return value; expected 9 actual %d\n", returnVal);
	errorCount++;
	}

    if (strcmp (c_msg0, "4321.5679"))
	{
	printf ("Error: 2 expected: 4321.5679 actual: %s\n", c_msg0);
	errorCount++;
	}

    ftoa (d_temp0, c_msg1, 'f', 4);

    if (9 != (returnVal = ftoa (d_temp0, c_msg1, 'f', 4)))
	{
	printf("Error: 3: bad return value; expected 9 actual %d\n", returnVal);
	errorCount++;
	}

    if (strcmp (c_msg1, "4321.5679"))
	{
	printf ("Error: 4 expected: 4321.5679 actual: %s\n", c_msg0);
	errorCount++;
	}

    /* test for spr 12868 */
    if (17 != (returnVal = ftoa (1.000000001, c_msg1, 'f', 15)))
	{
	printf ("Error: 5: bad return value; expected 17 actual %d\n",
		returnVal);
	errorCount++;
	}

    if (strcmp (c_msg1, "1.000000001000000"))
	{
	printf ("Error: 6: expected 1.000000001000000 actual: %s\n", c_msg1);
	errorCount++;
	}

    if (17 != (returnVal = ftoa (1.0000000001, c_msg1, 'f', 15)))
	{
	printf ("Error: 7: bad return value; expected 17 actual %d\n",
		returnVal);
	errorCount++;
	}

    if (strcmp (c_msg1, "1.000000000100000"))
	{
	printf ("Error: 8: expected 1.000000000100000 actual: %s\n", c_msg1);
	errorCount++;
	}

    if (errorCount)
	printf (ERRORS);
    else
	printf (NO_ERRORS);
}
 

#include "lib_top.h"



#pragma -nx

/****************************************************************************/
/* THIS INFORMATION IS PROPRIETARY TO 					    */
/* MICROTEC RESEARCH, INC.						    */
/*--------------------------------------------------------------------------*/
/* Copyright (c) 1993 Microtec Research, Inc.				    */
/* All rights reserved							    */
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

/****************************************************************************
*	0line_b1
*
*	This routine checks to see if the first parameter of the putc macro
*	is evaluated more than one, for line buffered output.  It does this
*	by inserted a side effect into the macro call and then performing
*	output.  After the output is performed, the results of the execution
*	of the side effect is examined to see if it is what it should be.
*
*****************************************************************************/

#include <stdio.h>
#include "message.h"

unsigned char test[] = NO_ERRORS;
int i;
int error_count;
int counter;

main () {

setvbuf (stdout, NULL, _IOLBF, 0);

for (i=0; i<sizeof(test)-1; i++) {
    putc ((counter++, test[i]), stdout);
    }


if (counter != sizeof(test) - 1) {
    printf ("Late error: Error occured on previous output\n");
    printf ("The char value for the macro putc seems to be evaluated more "
	    "than once.\n");
    printf (ERRORS);
    }
}

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
*	0inp_ff
*
*	This routine check to insure that inputting a 0xff will not generate
*	and EOF return from getc.  It does this by providing its own read
*	routine which places two 0xff's into the input buffer.
*
*****************************************************************************/

#include <stdio.h>
#include "message.h"

#ifndef LINE_BUFFER_SIZE
    #define LINE_BUFFER_SIZE    128
#endif

unsigned char buffer[LINE_BUFFER_SIZE];
int error_count;

/* Set up read to return two characters of value 0xff */

int read (int fd, char *start_buffer, unsigned nbyte) {
    *start_buffer 	= 0xff;
    start_buffer[1]	= 0xff;
    return 2;
}

main () {

setvbuf (stdin, (char *) buffer, _IOLBF, LINE_BUFFER_SIZE);

if (getc(stdin) == EOF) {	/* grab output from _filbuf */
    error_count++;
    }

if (getc(stdin) == EOF) {	/* grab output from buffer */
    error_count++;
    }

if (error_count) {
    printf ("Attempt to read 0xff resulted in return of EOF\n");
    printf (ERRORS);
    } else {
    printf (NO_ERRORS);
    }
}

#include "lib_top.h"


#pragma -nx

/****************************************************************************/
/* THIS INFORMATION IS PROPRIETARY TO					    */
/* MICROTEC RESEARCH, INC.						    */
/*--------------------------------------------------------------------------*/
/* Copyright (c) 1992 Microtec Research, Inc.				    */
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

/************************************************************************/
/*									*/
/* NAME				0 b i g l i n e				*/
/*									*/
/* FUNCTION	0bigline - Test to be sure that \n is flushed correctly	*/
/*			   when output buffer is full.			*/
/*									*/
/* DESCRIPTION								*/
/*		0bigline tests to be sure that \n is fushed when the 	*/
/*		output buffer is full and a \n is written as the next	*/
/*		character.  This is to check for a bug which exhibitted	*/
/*		itself in that \n was not written when the buffer was 	*/
/*		full and \n was the next character written.		*/
/*									*/
/* MODIFICATION HISTORY							*/
/*	01/26/94 jpc:	Written.					*/
/*									*/
/************************************************************************/

#include <stdio.h>

#include "message.h"

volatile extern unsigned char _simulated_output;

#ifndef LINE_BUFFER_SIZE
    #define LINE_BUFFER_SIZE    128
#endif

unsigned char main_buffer[256];
unsigned char *ptr;


write (int fd, unsigned char * in_buf, int nbyte) {	/* intercept output */
int i;				

    for (i=0; i<nbyte; i++) {
	*ptr++ = *in_buf++;
    }
    return nbyte;
}

main () {

int i;
unsigned char *msg_pnt;

    ptr = main_buffer; 
    setvbuf (stdout, NULL, _IOLBF, LINE_BUFFER_SIZE);

    for (i=0; i<LINE_BUFFER_SIZE; i++)
	putc ('b', stdout);

    putc ('\n', stdout);

    if (main_buffer[LINE_BUFFER_SIZE] != '\n') {
	msg_pnt = (unsigned char *) 
		"\\n not flushed correctly with full buffer.\n" ERRORS;
    } else {
	msg_pnt = (unsigned char *) NO_ERRORS;
    }

    while (*msg_pnt) {		/* Write message */
	_simulated_output = *msg_pnt++;
    }
}

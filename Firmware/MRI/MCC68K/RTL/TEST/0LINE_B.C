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
*	0line_b
*
*	The routine tests the functioning of line buffering.
*	It does this by specifying an output buffer locally.  It then writes
*	known data to stdout, which then should appear in that buffer.  The
*	routine checks for the contents of the buffer and for the parameters
*	in "_iob" to insure that they are correct.
*
*	Then this information is deleted and a correct completion message is
*	written.  The parameters are checked at the completion of the
*	operation to insure that output was attempted.
*
*	This routine of course cannot guarantee that any message was
*	written.  However, the test scripts do this as a matter of course.
*
*****************************************************************************/

#include <stdio.h>
#include "message.h"

#ifndef LINE_BUFFER_SIZE
    #define LINE_BUFFER_SIZE    128
#endif

unsigned char buffer[LINE_BUFFER_SIZE];
unsigned char test[] = "Buffer contains invalid data";
int i;
int error_count;

main () {

setvbuf (stdout, (char *) buffer, _IOLBF, LINE_BUFFER_SIZE);

for (i=0; i<sizeof(test); i++) {
    putc (test[i], stdout);
    }

if (memcmp (buffer, test, sizeof(test))) {
    error_count++;
    printf ("\n");					/* Print buffer */
    } else {
    if (stdout->_cnt != (LINE_BUFFER_SIZE - sizeof(test))) {
	stdout->_cnt = LINE_BUFFER_SIZE;		/* Clear buffer */
	stdout->_ptr = stdout->_base;
	printf ("stdout->_cnt is incorrect\n");
	} else {
	if (stdout->_ptr != (stdout->_base + sizeof(test))) {
	    stdout->_cnt = LINE_BUFFER_SIZE;		/* Clear buffer */
            stdout->_ptr = stdout->_base;
            printf ("stdout->_ptr is incorrect\n");
	    }
	}
    }

stdout->_cnt = LINE_BUFFER_SIZE;			/* Clear buffer */
stdout->_ptr = stdout->_base;

if (error_count) {
    printf (ERRORS);
    } else {
    printf (NO_ERRORS);
    }

if ((stdout->_ptr != stdout->_base) || (stdout->_cnt != LINE_BUFFER_SIZE)) {
    printf ("Late error: Error occured on previous output\n");
    printf (ERRORS);
    }
}

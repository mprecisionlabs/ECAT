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


/************************************************************************/
/*	This routine tests the functioning of the freopen function.	*/
/*	It does this by first executing freopen to re-open stdout.	*/
/*	Then it attempts a write operation, the the value of the 	*/
/*	file descriptor is checked to insure that it has changed to 	*/
/*	the correct value.						*/
/*									*/
/*	Written by jack christensen					*/
/************************************************************************/
/*	%W% %G%	*/

#include <stdio.h>
#include "message.h"

extern volatile char	_simulated_output;

int open (char *fileName, int flag, int mode)
{
return 99;		/* return fake file handle */
}

int write (int fd, char *buffer, unsigned nbyte)
{
	register unsigned int i;
	const char ErrorMessage[] = "Not writing to correct file handle\n" \
					ERRORS;
	char * charPnt;

	/* Memory mapped I/O: Write to the variable _simulated_output.      */
	/* The debugger is sensitive to the name of this variable, and will */
	/* automatically issue this command upon startup:                   */
	/*     OUTPORT &_simulated_output, STD.                             */

	if (fd != 99) {
	    for (i=sizeof(ErrorMessage) - 1, charPnt = buffer; i; --i) {
#if _MCC960 && _MONITOR
	        _console (1, *charPnt++);	/* form to position */	/* 960*/
#else
	        _simulated_output = *charPnt++;	/* form to position */
#endif
	    }					/* correctly */
	    exit (1);				/* exit after error message */
	}

	for (i=nbyte; i; --i) {			/* for loop must be in this */
#if _MCC960 && _MONITOR
	    _console (1, *buffer++);		/* form to position *//* 960 */
#else
	    _simulated_output = *buffer++;	/* form to position */
#endif
	}					/* correctly */
	return (nbyte);
}
 
int close (int fd) {			/* for 960 libs */
	return 0;
	}

main ()
{
	freopen ("dummy", "w", stdout);		/* re-open stdout */
	printf (NO_ERRORS);    /* attemp output */
}
 

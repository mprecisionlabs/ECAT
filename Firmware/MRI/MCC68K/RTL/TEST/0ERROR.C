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

/* pgm to test strerror(), perror() - written by H. Yuen  03/14/91 */

/*	%W% %G%	*/

#include <stdio.h>
#include "message.h"

const struct testcase {
	char str[20];
	int   err;
} cases[] = {
		"I/O error",   5,
		"Error undefined", -1,
		"Argument too large", 33,
		"Error undefined", 36,
		"Result too large", 34,
		"Error undefined", 2000,
		"Not enough space", 12,
		"", 0 };

extern char *strerror();
extern int errno;

volatile extern char _simulated_output;

char	buffer[256];
char	final[256];
char	*buffPtr;

char	errorString[] = ERRORS;
char	noErrorString[] = NO_ERRORS;

int	errorCount;

int write (int fd, char *buffer, unsigned nbyte)	/* intercept output */
{
	register unsigned int i;

        for (--buffer, i=nbyte; i; --i) {
	    if (*++buffer == '\n')
		*++buffPtr = '\0';			/* terminate string */
	    else
                *++buffPtr = *buffer;
        }
        return (nbyte);
}

main()
{
	register struct testcase *p;
	register int n, i, j;
	char  buf[30];
	char  *s;

	p = (struct testcase *)&cases[0];	/**** test strerror() ****/
	for (i=0; ; i++, p++) {
		if (p->str[0] == 0) break;
		s = strerror(p->err);
		if (strcmp(p->str,s)) {
		    printf("strerror error %2d  %s [%d] => %s\n",
			    i, p->str, p->err, s);
		    errorCount++;
		    }
	}

	p = (struct testcase *)&cases[0];	/**** test perror() ****/
	for (i=0; ; i++, p++) {
		if (p->str[0] == 0) break;
		buffPtr = buffer - 1;		/* preset buffer */
		errno = p->err;
		perror("got");

		strcpy (final, "got: ");	/* preset "standard" string */
		strcat (final, p->str);		/* put in message */
		if (strcmp (final, buffer)) {	/* and test */
			printf ("perror error %2d  %s [%d] => %s\n",
				i, final, p->err, buffer);
			errorCount++;
			}
	}

	if (errorCount) {
	    for (i=0; i<(sizeof (errorString) - 1); i++)
#if _MCC960 && _MONITOR
		_console (1, errorString[i]);
#else
		_simulated_output = errorString[i];
#endif
	    exit (1);
	    }
	else {
	    for (i=0; i<(sizeof (noErrorString) - 1); i++)
#if _MCC960 && _MONITOR
		_console (1, noErrorString[i]);
#else
		_simulated_output = noErrorString[i];
#endif
	    exit (0);
	    }
}
 

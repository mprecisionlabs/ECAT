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
/*									*/
/*	This routine will test fopen ().  It inserts its own open 	*/
/*	function to allow testing of the flags which fopen () sends to	*/
/*	open ().							*/
/*									*/
/************************************************************************/
/*	%W% %G%	*/

#include <stdio.h>
#include <mriext.h>
#include "message.h"

#define O_RDONLY        0x0000
#define O_WRONLY        0x0001
#define O_RDWR          0x0002
#define O_APPEND        0x0008
#define O_CREAT         0x0200
#define O_TRUNC         0x0400
#define O_FORM          0x4000
#define O_BINARY	0x8000

#define	num_el(X) (sizeof(X)/sizeof(X[0]))
static	int	i;
static	int	errorCount;
static	int	testFlag;
static	char	temp[256];

static const struct {
	char openString[5];
	int  openBits;
	} testData[] = {
		{"r",	O_RDONLY | O_FORM},
		{"w",	O_WRONLY | O_TRUNC  | O_CREAT | O_FORM},
		{"a",	O_WRONLY | O_APPEND | O_CREAT | O_FORM},
		{"rb",	O_RDONLY | O_BINARY},
		{"wb",	O_WRONLY | O_TRUNC  | O_CREAT | O_BINARY},
		{"ab",	O_WRONLY | O_APPEND | O_CREAT | O_BINARY},
		{"r+",	O_RDWR	 | O_FORM},
		{"w+",	O_RDWR	 | O_TRUNC  | O_CREAT | O_FORM},
		{"a+",	O_RDWR   | O_APPEND | O_CREAT | O_FORM},
		{"rb+",	O_RDWR	 | O_BINARY},
		{"wb+",	O_RDWR	 | O_TRUNC  | O_CREAT | O_BINARY},
		{"ab+",	O_RDWR   | O_APPEND | O_CREAT | O_BINARY},
		{"r+b",	O_RDWR	 | O_BINARY},
		{"w+b",	O_RDWR	 | O_TRUNC  | O_CREAT | O_BINARY},
		{"a+b",	O_RDWR   | O_APPEND | O_CREAT | O_BINARY} };

int open (char * filename, int mode, int access) 
{
    if (testFlag)			/* allow for calls from csys */
	if (testData[i].openBits != mode) {
	    write (1, temp, 
		sprintf (temp, "Error: test %d expected %.4x got %.4x\n",
		i, testData[i].openBits, mode));
	    errorCount++;
	    }
    return 10;	/* return # so fopen doesn't get confused */
}

int close (int dummy) {			/* for 960 */
	return 0;
	}

main ()
{
#if ! _MCC960
	fclose (stdprn);
	fclose (stdaux);
#endif
	
	testFlag++;			/* begin testing */

	for (i=0; i<num_el(testData); ++i)
	    if (NULL == fopen ("dummyFileName", testData[i].openString)) {
		printf ("Error: fopen returned NULL; test %d\n", i);
		errorCount++;
		}

	if (errorCount)
	    write (1, temp,
		sprintf (temp, ERRORS));
	else
	    write (1, temp,
		sprintf (temp, NO_ERRORS));

	_exit (0);	/* Don't do normal exit as this may confuse test */
}
 

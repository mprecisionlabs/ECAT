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


/*	%W% %G%	*/

#include <stdio.h>
#include <stdlib.h>

#include "message.h"

static int errorCount;


/*	
*	Intercept open call to eliminate "WARNING_open_stub_used_message"
*	from linker.
*/

int open (filename, flags, mode)
char *filename;
int  flags;
int  mode;
{
int i;
int track_array[FOPEN_MAX];

    for (i=0; i<FOPEN_MAX; i++) /* preset to zero */
        track_array[i] = 0;

    for (i=0; i<FOPEN_MAX; i++) /* set used file descriptors to 1 */
        if (_iob[i]._flag)
            track_array[_iob[i]._file] = 1;

    for (i=0; i<FOPEN_MAX; i++) /* return lowest available file descriptor */
        if (track_array[i] == 0)
            return i;

    return -1;                  /* could not find a file desriptor */
}



main ()
{
FILE	*fi, *fo;
int	a,c;
char	s[50], mybuf[BUFSIZ];
int	i;

if ((fi = fopen ("dummy", "r")) == NULL) {	/* Use files to ovoid */
    printf ("Failed to open input stream\n");	/* echoing the input */
    printf (ERRORS);
    exit (1);
    }

if ((fo = fopen ("dummy", "w")) == NULL) {
    printf ("Failed to open output stream\n");
    printf (ERRORS);
    exit (1);
    }

a = getc (fi);
if (a != (int) 'X')
	do_error (1);

c = getc (fi);
ungetc (c, fi);
if ((c != getc (fi)) || (c != (int) 'Y'))
	do_error (2);

if (fgetc (fi) != (int) 'A')
	do_error (3);

fgets (s, 6, fi);	/* reads 5 bytes */
if (strcmp (s, "1micr"))
	do_error (4);

if (errorCount)
	printf (ERRORS);
else
	printf (NO_ERRORS);

}

do_error (errno)
int errno;
{
	printf ("Error %d has occured\n", errno);
	errorCount++;
}
 

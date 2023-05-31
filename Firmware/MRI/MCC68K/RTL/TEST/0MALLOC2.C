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

/* pgm to test malloc(), free(), realloc() - written hy H. Yuen  06/16/85 */
/* 11/23/88 - fixed bug in reversing 2nd and 3rd args to memset() */

/* (last mod 11/23/88 HY) */
/*
 *  This test pgm calls malloc(), free(), and realloc() randomly until
 *  malloc() or realloc() fails.  It tries to test how well these functions
 *  manage the free block list in terms of searching and allocating and
 *  releasing space.  The total number of calls to each of these functions
 *  are printed at the end of the program.  Higher numbers means better
 *  efficiency for the memory management.
 */

/*
* CHANGE HISTORY
*	01/27/94 jpc:	added setvbuf to set output to unbuffered.  Line
*			buffering would fail because test fill heap space
*			before buffer is allocated.
*/

/*	%W% %G%	*/

#include <stdlib.h>
#include <stdio.h>

#include "message.h"

#define NULLPTR (char *)0
char *ptr[64];
int  size[64];

main(argc,argv)
char *argv[];
{
    int randx = 1;		/* for our own RNG */
    register int j;
    register unsigned nbytes;
    int  nm, nf, nr, oldsize;
    char *oldp;
    long mtotal, ftotal, rtotal;

#if _MCC86 && (_COMPACT || _LARGE)
    {
    extern unsigned long _HEAPSZ;
    _HEAPSZ = 0x20000;			/* Set to 128K for compact & large */
    }
#endif

    setvbuf (stdout, NULL, _IONBF, 0);

    mtotal = ftotal = rtotal = 0L;
    nm = nf = nr = 0;
    while (1) {
	randx = randx * 0x4E6D + 12345;
	j = (randx >> 2) & 63;			/* random number 0..63 */
	nbytes = (randx & 2047) + 4;		/* random size 4..2051 */
	if (ptr[j] == NULLPTR) {
		do
			nbytes = nbytes / 2 + 4;
		while (nbytes & 1);
		nbytes += nm;
		if ((ptr[j] = malloc(nbytes)) == NULLPTR)
			break;
		size[j] = nbytes;
		mtotal += nbytes;  nm++;
		memset(ptr[j],nm,nbytes);
		if (argc > 1)
		    printf("%3d  malloc(%d) %04x\n",nm,nbytes,ptr[j]);
		if (nm > 1000)		/* 32-bit targets may run too long */
		    break;
	}
	else if (randx & 16) {			/* random bit */
		free(oldp=ptr[j]);
		ftotal += size[j];  nf++;
		ptr[j] = NULLPTR;
		if (argc > 1)
		    printf("%3d    free(%04x) %d\n",nf,oldp,size[j]);
	}
	else {
		oldsize = size[j];
		nbytes += nr;
		if ((ptr[j] = realloc(oldp=ptr[j],nbytes)) == NULLPTR)
			break;
		size[j] = nbytes;
		rtotal += nbytes;  nr++;
		memset(ptr[j],nr,nbytes);
		if (argc > 1)
		    printf("%3d realloc(%04x [%d],%d) %04x\n",
			nr,oldp,oldsize,nbytes,ptr[j]);
	}
    }

    if (nm < 10) {
	printf("%d malloc: %ld   %d free: %ld   %d realloc: %ld (bytes)\n",
	    nm, mtotal, nf, ftotal, nr, rtotal);
	printf(ERRORS);
    }
    else {
	printf(NO_ERRORS);
    }
}
 

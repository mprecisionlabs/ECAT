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

/* pgm to test malloc()  -  06/16/85  HY */
/*
 *  This test pgm tests malloc() for how well it utilizes available memory
 *  in terms of low overhead and minimal wasted space.
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

char *sbrk();

#if (_LARGE || _COMPACT) && (! _MCCT9K)
    #define 	MEM_HUNK 0xff0
#else
    #define	MEM_HUNK 512
#endif

main(argc,argv)
{
	register char *p, *p0;
	register int i;
	char * sbrk0;

	setvbuf (stdout, NULL, _IONBF, 0);

	sbrk0 = sbrk (0);

	for (i=0; i < 1000; i++) {
		if ((p = malloc(MEM_HUNK)) == 0) break;	/* ask for memory */
		p0 = p;
		if (argc > 1)
			printf("i = %d  p = %p\n",i, p0);
	}

	if (i < 10) {		/* Less than 10 cycles indicate an error */
		printf("sbrk(0) = %p\n", sbrk0);
		printf("i = %d  p = %p  sbrk(0) = %p\n",i, p0, sbrk(0));
		printf(ERRORS);
	}
	else {
		printf (NO_ERRORS);
	}
}
 

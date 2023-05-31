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

/* pgm to test malloc(), free() - written by H. Yuen  06/16/85 */
/*
 *  This test pgm tests malloc() and free() for how well it manages the
 *  free block list in terms of reusing existing free spaces and
 *  combining adjacent free blocks.
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

main(argc,argv)
{
	register char *p1, *p2, *p3, *p4;
	register int i, t;

	setvbuf (stdout, NULL, _IONBF, 0);

	for (i=t=0; i < 1000; i++) {
		if ((p1 = malloc(1100)) == 0) break; t += 11;
		if ((p2 = malloc(800)) == 0) break;  t += 8;
		if ((p3 = malloc(600)) == 0) break;  t += 6;
		if ((p4 = malloc(400)) == 0) break;  t += 4;
		free(p3); free(p1); free(p2); t -= 27;
		p3 = malloc(2000); t += 20;
		free(p4); t -= 4;
		if (argc > 1)
			printf("p1 = %p   p3 = %p\n", p1, p3);
		if (p3 == 0) break;
	}

	if (i < 10) {
		printf("  i = %d  t = %d00\n", i, t);
		printf(ERRORS);
	}
	else {
		printf (NO_ERRORS);
	}
}
 

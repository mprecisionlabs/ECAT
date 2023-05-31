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
/*	Routine to test setvbuf().  This is done by calling setvbuf() in*/
/*	a couple of different ways and checking to see how _iob is 	*/
/*	modified							*/
/************************************************************************/
/*	%W% %G%	*/

#include <stdio.h>
#include "message.h"

int errorCount;
char array[25];

main ()
{
    setvbuf (stdin, NULL, _IOLBF, 0);

    if (stdin->_flag & _IONBF) {
	puts ("First setvbuf call: _flag not set right");
	errorCount++;
	}

    setvbuf (stdin, array, _IONBF, 25);

    if (! (stdin->_flag & _IONBF)) {
	puts ("Second setvbuf call: _flag not set right");
	errorCount++;
	}

    if (stdin->_base != NULL) {
	puts ("Second setvbuf call: _base not NULL");
	errorCount;
	}

    setvbuf (stdin, array, _IOFBF, BUFSIZ);

    if (stdin->_flag & _IONBF) {
	puts ("Third setvbuf call:  _flag not set right");
	errorCount;
	}

    if (stdin->_base != (unsigned char *) array) {
	puts ("Third setvbuf call: _base set incorrectly");
	errorCount++;
	}

    if (errorCount) {
	printf (ERRORS);
	}
    else {
	printf (NO_ERRORS);
	}
}
 

#include "lib_top.h"


#if EXCLUDE_zalloc
    #pragma option -Qws			/* Prevent compiler warning */
#else
 
/****************************************************************************/
/* THIS INFORMATION IS PROPRIETARY TO                                       */
/* MICROTEC RESEARCH, INC.                                                  */
/*--------------------------------------------------------------------------*/
/* Copyright (c) 1987, 1988, 1989, 1991, 1992 Microtec Research, Inc.       */
/* All rights reserved                                                      */
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


/*	%W% %G% */

/************************************************************************
 *  calloc.c
 *			Memory Allocation Functions
 *
 *  char *calloc(nelem, elsize)
 *		Allocates space for an array of nelem elements of size
 *		elsize and returns a pointer to it.
 *
 *  char *zalloc(size)
 *		Returns a pointer to a block of memory of size bytes
 *		and zero filled.
 *
 *	Both functions return NULL if not enough memory.
 *
 *	History:
 *	03/28/90 jpc:	zalloc and calloc now return void * when __STDC__
 *			is not defined
 *	05/10/90 jpc:	zalloc now in its own file.
 *	05/11/90 jpc:	_memclr -> memset
 *	11/19/91 jpc:	remove __STDC__ constructs
 ************************************************************************/
/* (last mod 06/23/85  HY) */

#include <stddef.h>
#include <stdlib.h>
#include <string.h>

void *zalloc (size_t size)
{
	register char *ptr;

	if ((ptr = malloc(size)) != NULL)
	    memset (ptr, (int) 0, size);		/* fill with zero */
	return ptr;
}
 
#endif /* EXCLUDE_zalloc */
 

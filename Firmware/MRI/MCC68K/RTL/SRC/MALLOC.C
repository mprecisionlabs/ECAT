#include "lib_top.h"


#if EXCLUDE_malloc
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


/*	%W% %G%	*/

/************************************************************************/
/* MODULE		  MALLOC					*/
/*		Memory Alloctions Functions				*/
/*									*/
/*	malloc - allocates a block of at least 'size' bytes beginning   */
/*		on a word boundary.					*/
/*									*/
/*	Ref: Knuth's book vol 1, pp 435-441.				*/
/*									*/
/* MODIFICATION HISTORY							*/
/*	06/21/85	HY - original implementation			*/
/*	09/30/86	wbdenniston - inserted header comments		*/
/*	12/01/86	HY - do not round units to multiples of 64;	*/
/*			     put realloc() in a separate module		*/
/*	05/12/88	HY - return NULL for size 0 as per __STDC__	*/
/*	 7/28/89	jpc- make data extern for 68k			*/
/*	 9/23/89	jpc- added _getavail as per 86 requirements	*/
/*	 2/06/90	HY - added addr(), norm() for large data model	*/
/*	 3/08/90	jpc- added NORM to eliminate address wrapping	*/
/*			     added MAXSIZE for same reason		*/
/*	 3/28/90	jpc- returns void * for non-ANSI		*/
/*	04/11/90	jpc- added _MCC88K to #if's			*/
/*	01/29/91	jpc- added _MCCSP to #if's			*/
/*			     remove non-ANSI function def's		*/
/*	04/01/91	jpc- remove #if for data declarations		*/
/*	02/02/92	jpc- split "free ()" into seperate module	*/
/*	12/13/93	jpc- added code to request minimal memory block	*/
/*			     from sbrk when first request fails.  Fixes	*/
/*			     spr 29998, 31799, and 30226.		*/
/*									*/
/************************************************************************/

#include <stddef.h>
#include <stdlib.h>

#define HEADER struct mem

HEADER {			/* struct of header for free block list */
    HEADER   *next;		/* ptr to HEADER of next free block */
    unsigned size;		/* block size (HEADER units) incl. HEADER */
};

extern	HEADER _membase;		/* head of free block list */
extern	HEADER *_avail;			/* ptr to  free block list */
extern	char   _badlist;		/* non-zero if list is corrupted */

#if (_MCC86 && (_COMPACT || _LARGE))

#pragma option -RbOg

#define	NORM(x)	  ((HEADER *)_norm(_addr(x)))
#define XEQ(a,b)  (_addr(a) == _addr(b))
#define XNE(a,b)  (_addr(a) != _addr(b))
#define XLT(a,b)  (_addr(a) <  _addr(b))
#define XLE(a,b)  (_addr(a) <= _addr(b))
#define XGT(a,b)  (_addr(a) >  _addr(b))
#define XGE(a,b)  (_addr(a) >= _addr(b))

#else	/* _MCC86 */

#define	NORM(x)	  (x)
#define _addr(x)  (x)
#define _norm(x)  (x)
#define XEQ(a,b)  ((a) == (b))
#define XNE(a,b)  ((a) != (b))
#define XLT(a,b)  ((a) <  (b))
#define XLE(a,b)  ((a) <= (b))
#define XGT(a,b)  ((a) >  (b))
#define XGE(a,b)  ((a) >= (b))

#endif	/* ! _MCC86 */

#if _MCC86				/* Don't allow inadvertent wrapping */
#define	MAXSIZE	  ((unsigned)(-sizeof(HEADER) - 0x10 - 1))
#else
#define	MAXSIZE	  ((unsigned)(-sizeof(HEADER) - 1))
#endif

/************************************************************************/
/* NAME                         M A L L O C                             */
/*                                                                      */
/* FUNCTION     malloc - allocates a block of at least 'size' bytes     */
/*              beginning on a word boundary.                           */
/*                                                                      */
/* SYNOPSIS     char *malloc(size)                                      */
/*              unsigned int size ; size of block in bytes              */
/*                                                                      */
/* RETURN       Pointer to a block of memory. NULL if out of memory     */
/*              or free list is corrupted.                              */
/*                                                                      */
/* DESCRIPTION                                                          */
/* malloc() searches the circular free block list, starting from 	*/
/* _avail, and allocates the first block big enough for size bytes.    */
/* It calls () to get more memory from the system when there is 	*/
/* no suitable free space.  It only asks for memory in multiple blocks  */
/* of HEADER's.  The allocated block of memory also has a header so	*/
/* that free() can determine how much to free. 				*/
/************************************************************************/

extern	void * sbrk (size_t);

void *malloc (size_t size)
{
    register    HEADER *p, *q;		/* ptrs to current & last HEADER */
    register	HEADER *highest;	/* highest free block in memory */
    unsigned    units, nblock;

    if (size == 0)			/* get malloc fix to match previous */
	size = 1;			/* behavior for spr 40719 */

    if (size > MAXSIZE)
	return NULL;
    units = (size+sizeof(HEADER)-1)/sizeof(HEADER)+1;

    q = _avail;					/* start search from here */

    if (!_badlist)				/* quit if free list is bad */

    for (p=q->next; ; q=p, p=p->next) {

        /* is this free block big enough ? */
	if (p->size >= units) {
	    if (p->size <= units+2)
		q->next = p->next;	/* < 2 units left, use whole block */
	    else {				/* cut up block */
		q->next = NORM(p) + units;	/* start of new block */
		q->next->size = p->size - units;/* size of new block */
		q->next->next = p->next;	/* next link of new block */
		p->size = units;		/* size of allocated block */
	    }
	    _avail = q;				/* next time, start from here */

	    return ((char *)(NORM(p) + 1));/* user memory starts after header */
	}

	if (XEQ(p,_avail)) {		/* end of list, get more memory */
	    nblock = (units<128) ? 128:units;	/* get at least 128 units */
	    if ((p=(HEADER *)sbrk(nblock*sizeof(HEADER))) == (HEADER *)-1) {
						/* Re-use variables !! */
		highest = _avail;		/* Failed allocating big block*/

		while (XGT (highest->next, highest))	/* find highest free */
		    highest = highest->next;

		nblock = units;	/* Check for free block at end of list */
		if (NORM ((char *)&highest[highest->size]) == NORM (sbrk(0)))
		    nblock -= highest->size;

				/* Try to allocate smallest possible block */
		if ((p=(HEADER *)sbrk(nblock*sizeof(HEADER))) == (HEADER *)-1)
		    return NULL;
	    }

	    p->size = nblock;
	    free((char *)(NORM(p) + 1));/* add to free list */
	    p = _avail;
	}   
    }
    return NULL;			/* out of memory or bad free list */
}
 
#endif /* EXCLUDE_malloc */
 

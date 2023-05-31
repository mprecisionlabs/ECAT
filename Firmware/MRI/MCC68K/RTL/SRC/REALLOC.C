#include "lib_top.h"


#if EXCLUDE_realloc
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


/*      %W% %G% */

/************************************************************************/
/* MODULE                  REALLOC                                      */
/*		Memory Alloctions Functions                             */
/*                                                                      */
/*	realloc - changes the size of the block pointed to by ptr to    */
/*              size bytes and returns a pointer to the (possibly       */
/*              moved) block.                                           */
/*                                                                      */
/* MODIFICATION HISTORY                                                 */
/*	12/01/86	HY - separated this module from malloc		*/
/*	 9/23/89	jpc - put in changes hy had put into 86 routines*/
/*			      This fixed a bug in the realloc routine.	*/
/*	 2/06/90	HY - added addr(), norm() for large data model	*/
/*       3/08/90        jpc- added NORM to eliminate address wrapping   */
/*                           added MAXSIZE for same reason              */
/*			     modified realloc to conform to ANSI for 	*/
/*			     NULL ptr and zero size			*/
/*	 03/28/90	jpc- realloc now returns void * for non-ANSI	*/
/*	 08/02/90	jpc- removed #pragma option			*/
/*	 11/19/91	jpc- removed __STDC__ constructs		*/
/*                                                                      */
/************************************************************************/

#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#define HEADER struct mem

HEADER {			/* struct of header for free block list */
    HEADER   *next;		/* ptr to HEADER of next free block */
    unsigned size;		/* block size (HEADER units) incl. HEADER */
};

#if (_MCC86 && (_COMPACT || _LARGE))

extern unsigned long _addr();	/* expand p to real physical addr */

extern char *_norm();		/* normalize 32-bit real addr to seg:off */

#define NORM(x)   ((HEADER *)_norm(_addr(x)))
#define XEQ(a,b)  (_addr(a) == _addr(b))
#define XNE(a,b)  (_addr(a) != _addr(b))
#define XLT(a,b)  (_addr(a) <  _addr(b))
#define XLE(a,b)  (_addr(a) <= _addr(b))
#define XGT(a,b)  (_addr(a) >  _addr(b))
#define XGE(a,b)  (_addr(a) >= _addr(b))

#else	/* _MCC86 */

#define NORM(x)   (x)
#define _addr(x)  (x)
#define _norm(x)  (x)
#define XEQ(a,b)  (((char *)(a)) == ((char *)(b)))
#define XNE(a,b)  (((char *)(a)) != ((char *)(b)))
#define XLT(a,b)  (((char *)(a)) <  ((char *)(b)))
#define XLE(a,b)  (((char *)(a)) <= ((char *)(b)))
#define XGT(a,b)  (((char *)(a)) >  ((char *)(b)))
#define XGE(a,b)  (((char *)(a)) >= ((char *)(b)))

#endif	/* ! _MCC86 */

/************************************************************************/
/*	_xmalloc(size) returns the next block malloc() will get.	*/
/*	_xmalloc() does not do the actual storage allocation.		*/
/************************************************************************/

static char *_xmalloc(size)
unsigned size;
{
    register    HEADER *p, *q;		/* ptrs to current & last HEADER */
    unsigned    units;
    extern	HEADER *_avail;

    units = (size+sizeof(HEADER)-1)	/sizeof(HEADER)+1;
    q = _avail;				/* start search from here */
    for (p=q->next; ; q=p, p=p->next) {
	if (p->size >= units)		/* is this free block big enough ? */
	    return ((char *)((HEADER *) NORM(p) + 1));	/* user memory starts */
							/* after header */
	if (XEQ(p,_avail))
	    break;
    }
    return (char *)0;
}

/************************************************************************/
/* NAME                     R E A L L O C                               */
/*                                                                      */
/* FUNCTION     realloc - changes the size of the block pointed to by   */
/*              ptr to size bytes and returns a pointer to the          */
/*              (possibly moved) block.                                 */
/*                                                                      */
/* SYNOPSIS     char *realloc (ptr,size)                                */
/*              register char *ptr;                                     */
/*              unsigned size;                                          */
/*                                                                      */
/* RETURN       Pointer to the new block. NULL if out of memory or      */
/*              free list is corrupted.                                 */
/*                                                                      */
/* DESCRIPTION                                                          */
/*	Changes the size of the block pointed to by ptr to size bytes   */
/* and returns a pointer to the (possibly moved) bloc. realloc() first  */
/* frees the given block of memory.  free() does not change the content */
/* of the block.  It sets up avail so that the next call to malloc()    */
/* will begin the search from this block.  Therefore, if the new size   */
/* is smaller than the old size, realloc() will usually allocate the    */
/* same block and no copying is necessary.  realloc() will move (copy)  */
/* the block only when the new block starts at a different address than */
/* the old block.                                                       */
/************************************************************************/

#if _MCC86                              /* Don't allow inadvertent wrapping */
#define MAXSIZE   ((unsigned)(-sizeof(HEADER) - 0x10 - 1))
#else
#define MAXSIZE   ((unsigned)(-sizeof(HEADER) - 1))
#endif

void *realloc (void *ptr, size_t size)
{
    register char *old, *p;
    unsigned oldsize;

    if (size > MAXSIZE)
	return NULL;

    if (ptr == NULL)
	return malloc (size);		/* as per ANSI */

    oldsize = (*((unsigned *)ptr+(-1)) + (-1)) * sizeof(HEADER);
    free(old=ptr);			/* add block to free list */

    if (! size)				/* Just free space -- as per ANSI */
	return NULL;

    p = _xmalloc (size);	/* get the free block malloc () will return */
				/* is new free block inside old block ? */
    if (p && XLT(p,ptr) && XGT((p+size),ptr)) {
	memcpy(p,old,oldsize);	/* move block, then call malloc() */
	return (malloc(size));
	}

     if (ptr = malloc(size))
	if (XNE(ptr,old))		/* need to move the block ? */
	    memcpy(ptr,old,size > oldsize ? oldsize : size);

    return (ptr);
}
 
#endif /* EXCLUDE_realloc */
 

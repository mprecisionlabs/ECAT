#include "lib_top.h"


#if EXCLUDE_free
    #pragma option -Qws			/* Prevent compiler warning */
#else
 
/****************************************************************************/
/* THIS INFORMATION IS PROPRIETARY TO                                       */
/* MICROTEC RESEARCH, INC.                                                  */
/*--------------------------------------------------------------------------*/
/* Copyright (c) 1987, 1988, 1989, 1991, 1992 Microtec Research, Inc.	    */
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
/* MODULE		  FREE						*/
/*		Memory Alloctions Function				*/
/*									*/
/*	free - frees a block of memory previously allocated by one of   */
/*		the allocation functions. Its contents remain unchanged */
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
/*	02/02/92	jpc- split free into seperate module		*/
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

/************************************************************************/
/* NAME                         F R E E                                 */
/*                                                                      */
/* FUNCTION     free - Frees a block of memory previously               */
/*              allocated by one of the allocation functions.  Its      */
/*              contents remain unchanged                               */
/*                                                                      */
/* SYNOPSIS     void free(ptr)                                          */
/*              void *ptr ;     pointer to previously allocated block   */
/*                                                                      */
/* RETURN       no return                                               */
/*                                                                      */
/* DESCRIPTION                                                          */
/*      free() adds the given block of memory to the free block list    */
/* which is sorted by memory address, i.e. p->next > p except the end   */
/* of the list when p->next equals the head of the list.  free() also   */
/* coalesces (merges) adjacent free blocks to form larger free blocks.  */
/*      NOTE that free() does not disturb the content of the given      */
/* block except the header info.  This is necessary because some UNIX   */
/* programs require that the freed space remain unchanged.              */
/************************************************************************/


void free (void *ptr)
{
    register HEADER *p, *q;
    register HEADER *endp, *endq;

    if (ptr == NULL)
	return;
    p = (HEADER *)(_norm(_addr((char*)ptr) - sizeof(HEADER)));
    for (q=_avail; XLT(p,q) || XGT(p,q->next); q=q->next)  /* find the right */
	if (XGE(q,q->next) && (XGE(p,q)||XLE(p,q->next))) /* place to insert */
	    break;
    endp = NORM(p) + p->size;
    endq = NORM(q) + q->size;
    if (XLT(p,endq) && XGT(endp,endq) ||
	XLT(p,q->next) && XGT(endp,q->next)) {
	_badlist = 1;				/* free list corrupted */
	return;
    }
    if (XEQ(p,q->next) ||
	XGE(p,q) && XLE(endp,endq))	/* block already freed ? */
	{ _avail = q; return; }
    if (XEQ(endp,q->next)) {		/* free block on the right ? */
	p->size += q->next->size;
	p->next = q->next->next;
    }
    else  p->next = q->next;

    if (XEQ(endq,p)) {			/* free block on the left ? */
	q->size += p->size;
	q->next = p->next;
    }
    else  q->next = p;

    _avail = q;
}
 
#endif /* EXCLUDE_free */
 

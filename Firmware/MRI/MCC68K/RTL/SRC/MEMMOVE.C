#include "lib_top.h"


#if EXCLUDE_memmove
    #pragma option -Qws			/* Prevent compiler warning */
#else
 
/****************************************************************************/
/* THIS INFORMATION IS PROPRIETARY TO                                       */
/* MICROTEC RESEARCH, INC.                                                  */
/*--------------------------------------------------------------------------*/
/* Copyright (c) 1987, 1988, 1989, 1992 Microtec Research, Inc.             */
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

/****************************************************************************/
/*   Copy the 'n' characters at 's2' to location 's1'.  Make sure the copy  */
/*   is done correctly even if the two objects overlap.			    */
/*									    */
/*   History:								    */
/*			Original source received			    */
/*	6/19/90 jpc:	modified to allow use of memcpy			    */
/*			post increment and decrement on regs removed	    */
/*	6/27/90 jpc:    call memcpy whenever possible			    */
/*	8/28/90 jpc:	modified to normalize addresses for '86 compact	    */
/*			and large model					    */
/*     10/16/90 jpc:	added #include <string.h>			    */
/*     03/05/92 jpc:	_norm and _addr are now external		    */
/*									    */
/****************************************************************************/

/*	%W% %G%	*/

#include <stddef.h>
#include <string.h>

#if (_MCC86 && (_COMPACT || _LARGE))
 
#pragma option -RbOg
 
unsigned long _addr(char *p);
char *_norm(unsigned long ul);	/* normalize 32-bit real addr to seg:off */

#define NORM(x)   ((char *)_norm(_addr(x)))
#define XLT(a,b)  (_addr(a) <  _addr(b))
#define XGT(a,b)  (_addr(a) >  _addr(b))

#else   /* _MCC86 */

#define NORM(x)   (x)
#define XLT(a,b)  ((a) <  (b))
#define XGT(a,b)  ((a) >  (b))
 
#endif  /* ! _MCC86 */


void *memmove(void *s1, const void *s2, size_t n)
{
	register char	*ptr1 = (char *) s1,
			*ptr2 = (char *) s2;
	register size_t	count = n;

 /* Do the objects overlap with the source addr. less than the dest. addr.? */

	if ((XLT (ptr2, ptr1))  &&  (XGT ((ptr2 + count), ptr1))) {
		ptr1 = NORM (ptr1) + count;	/* Copy starting at the back */
		ptr2 = NORM (ptr2) + count;
		for (++count; --count;)
	    		*--ptr1 = *--ptr2;
    	}
    	else				/* memcpy copies from front */
		memcpy (s1, s2, n);	/* go for speed */

    	return(s1);		/* Always return the first ptr */
}  /* memmove() */
 
#endif /* EXCLUDE_memmove */
 

#include "lib_top.h"


#if EXCLUDE_strcmp
    #pragma option -Qws		/* Prevent compiler warning */
#else
 
_kill_warning();			/* Eliminates compiler warnings */

/*
**************************************************************************
** THIS INFORMATION IS PROPRIETARY TO					**
** MICROTEC RESEARCH, INC.						**
**----------------------------------------------------------------------**
** Copyright (c) 1991, 1993 Microtec Research, Inc.			**
** All rights reserved							**
**************************************************************************
*The software source code contained in this file and in the related
*header files is the property of Microtec Research, Inc. and may only be
*used under the terms and conditions defined in the Microtec Research
*license agreement. You may modify this code as needed but you must retain
*the Microtec Research, Inc. copyright notices, including this statement.
*Some restrictions may apply when using this software with non-Microtec
*Research software.  In all cases, Microtec Research, Inc. reserves all rights.

	TTL			'strcmp'
*
*	File name:		strcmp.src
*	Author:			Herbert Yuen
*	Date:			85-May-10 Friday
*
*	04/30/90 fu/jpc		Re-written to use long moves
*	07/06/93 bet/gafter	Performance improvement by using Lamport-Knuth 
*				algorithm
*				Above changes aimed at optimal performance
*				on 68040 processor.
*	11/30/93 jpc		Conditionalized alignment code and "align"
*				directives.
*
*	%W% %G%	*
*
*==============================================================================
*
*			s t r c m p
*
*==============================================================================
* int	strcmp(S1,S2)
*
*	Compare S1 to S2.  Return -1,0,+1 as <,=,>.
*	S1 and S2 are 0 terminated C strings.
*
*/
/*

The Lamport-Knuth algorithm is based on the observation that adding one
to a byte causes a carry out of the byte iff the byte contains 0xff.
If we actually allowed the carry to occur, it would corrupt the next
byte, so instead we detect when an carry *would* occur:  when the
high-order bit is one and adding one to the low-order 7 bits causes a
carry into the high-order bit.  Thus, if X is the given word, to test
for zero bytes we compute


method (1)


	Y = ~X
	Z = (Y & 0x7f7f7f7f) + 0x01010101
	R = (Y & Z) & 0x80808080

Then R == 0 means X contains no zero bytes (i.e., Y contains no 0xff
bytes).

Suppose we used subtraction instead of addition?

Subtracting one from a byte causes a borrow iff the byte was originally
zero.  Once again, we must detect when the borrow *would* occur:  when
the high-order bit is zero, and subtracting one from the low-order 7
bits causes a borrow from the high-order bit.  If X is the given word,
we compute

So the algorithm for Lamport-Knuth using subtraction is

method (2)


	Y = X	(i.e., no operation)
	Z = (X | 0x80808080) - 0x01010101
	R = (Y | Z) & 0x80808080

Now R == 0x80808080 means X contains no zero bytes.

*/


#include <string.h>

int     strcmp(const char * S1, const char * S2)
{
    asm ("	move.l	`S1`,a0");	/* S1 */
    asm ("	move.l	`S2`,a1");	/* S2 */

#if ! (_68040 || _68EC040)

    asm ("	move.w	a0,d0");	/* check for alignment */
    asm ("	or.l	`S2`,d0");	/* of both addresses */
    asm ("	btst	#0,d0");	/* we only require word align */
    asm ("	beq.b	short_aligned");/* we can fetch word-aligned longs */
    asm ("	bra.b	byte_aligned");	/* compare byte by byte */

#else	/* (_68040 || _68EC040) */

    asm ("	align	4");		/* force alignment */

#endif	/* (_68040 || _68EC040) */

    asm ("short_aligned:");
    asm ("long_loop:");
    asm ("	move.l  (a0)+,d0 ");     /* load a word from S1 */
    asm ("	move.l	d0,d1");         /*knuth-lamport to test for zero byte*/
    asm ("	or.l	#$80808080,d1"); /* use method 2 */
    asm ("	subi.l	#$01010101,d1");
    asm ("	or.l	d0,d1");
    asm ("	and.l	#$80808080,d1");
    asm ("	cmpi.l	#$80808080,d1");
    asm ("	beq.b	next_iter");     /* if no zero byte, continue loop */
    asm ("	subq.l	#4,a0");         /* if zero byte, use bytewise endgame*/

    asm ("byte_aligned:");
    asm ("byte_loop:");
    asm ("	move.b	(a0)+,d0");	/* *S1++ in D0 */
    asm ("	cmp.b	(a1)+,d0");	/* *S2++ == D0 ? */
    asm ("	bne.b	byte_neq");	/* Exit loop if not equal */
    asm ("	tst.b	d0");		/* End of S1 ? */
    asm ("	bne.b	byte_loop");

    asm ("byte_eq:");
    asm ("	moveq	#0,d0");	/* Return 0 for strings equal */
    asm ("	bra.b	return");

    /* here begins the "middle" of the inner loop */

#if _68040 || _68EC040

    asm("	align	4");            /* inner loop starts a new cache line */

#endif	/* _68040 || _68EC040 */

    asm("next_iter:");
    asm("	sub.l	(a1)+,d0");     /* compare longs unsigned */
    asm("	beq.b	long_loop");    /* if equal then next loop iteration */
    asm("	subx.l	d0,d0");        /* else move result from carry to d0 */
    asm("	ori.l	#1,d0");        /* make sure it is -1 or 1 */
    asm("	bra.b	return ");      /* and return that value */
    asm ("byte_neq:");
    asm("	sub.b	-(a1),d0");     /* characters differ; which is less? */
    asm("	subx.l	d0,d0");        /* move result from carry to d0 */
    asm("	ori.l	#1,d0");        /* make sure it is -1 or 1 */
    asm ("return:");
}
 
#endif /* EXCLUDE_strcmp.c */
 

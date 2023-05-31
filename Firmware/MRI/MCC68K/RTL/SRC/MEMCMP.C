#include "lib_top.h"


#if EXCLUDE_memcmp
    #pragma option -Qws			/* Prevent compiler warning */
#else
 
_kill_warning();	/* Eliminates compiler warnings */

/*
**************************************************************************
** THIS INFORMATION IS PROPRIETARY TO					**
** MICROTEC RESEARCH, INC.						**
**----------------------------------------------------------------------**
** Copyright (c) 1991 Microtec Research, Inc.				**
** All rights reserved							**
**************************************************************************
**
*The software source code contained in this file and in the related
*header files is the property of Microtec Research, Inc. and may only be
*used under the terms and conditions defined in the Microtec Research
*license agreement. You may modify this code as needed but you must retain
*the Microtec Research, Inc. copyright notices, including this statement.
*Some restrictions may apply when using this software with non-Microtec
*Research software.  In all cases, Microtec Research, Inc. reserves all rights.
**

	TTL			'memcmp'
*
*	File name:		memcmp.src
*	Author:			Herbert Yuen
*	Date:			85-May-10 Friday
*
*	History:
*	05/08/90 jpc:	rewritten to use long word compares whenever
*			possible
* %W% %G% *
*
*==============================================================================
*
*			m e m c m p
*
*==============================================================================
* int	memcmp(S1,S2,N)
*
*	Compare N bytes of memory content at S1 to memory content at S2.
*	S1 and S2 are memory addresses.
*	N is an unsigned int.
*
*
*/

#include <string.h>

int memcmp(const void *s1, const void *s2, size_t n)
{
    asm ("	move.l	`s1`,a0");	/* S1 */
    asm ("	move.l	`s2`,a1");	/* S2 */
    asm ("	move.l	`n`,d1");	/* N */
    asm ("	cmpi.l	#8,d1");	/* should we go right to byte compare */
    asm ("	bls.b	byte_aligned"); /* yes */

    asm ("	move.w	a1,d0");	/* first address */
    asm ("	move.w	a0,d1");	/* second address -- zapped byte count*/
    asm ("	eor.b	d1,d0");	/* look for different bits. */
    asm ("	move.l	`n`,d1");	/* get back the byte count */
    asm ("	btst.b	#0,d0");	/* test low bit */
    asm ("	bne.b	byte_aligned");	/* bits different -- process byte-wise*/

    asm ("	move.w	a0,d0");	/* get low bits of address again */
    asm ("	btst.b	#0,d0");	/* bits same -- are they zero */
    asm ("	beq.b	long_aligned");	/* zero -- go ahead */
    asm ("	subq.l	#1,d1");	/* decrement byte count by one */
    asm ("	cmpm.b	(a1)+,(a0)+");	/* compare bytes */
    asm ("	bne.b	not_equal");	/* different -- so jump */

    asm ("long_aligned:");
    asm ("	move.b	d1,d0");	/* get residual bits */
    asm ("	andi.l	#3,d0");	/* isolate them */
    asm ("	lsr.l	#2,d1");	/* divide by 4 */

    asm ("	subq.l	#1,d1");	/* --N >= 0 ? */
    asm ("	blt.s	long_equal");	/* N < 0 means strings are equal */
    asm ("long_loop:");
    asm ("	cmpm.l	(a1)+,(a0)+");	/* *S2++ == *S1++ ? */
    asm ("	dbne	d1,long_loop"); /* Repeat until not equal or --N < 0 */
    asm ("	bne.s	not_equal"); 	/* Branch if different */

    asm ("	subi.l  #$10000,d1");	/* Simulate borrow from high 16 bits */
    asm ("	cmpi.l  #-1,d1");	/* Is long word count == -1 */
    asm ("	bne.s   long_loop");	/* Continue loop if not done */
    asm ("long_equal:");
    asm ("	move.l	d0,d1");	/* get back residual bits. */

    asm ("byte_aligned:");
    asm ("	subq.l	#1,d1");	/* --N >= 0 ? */
    asm ("	blt.s	equal");	/* N < 0 means strings are equal */
    asm ("byte_loop:");
    asm	("	cmpm.b	(a1)+,(a0)+");	/* *S2++ == *S1++ ? */
    asm ("	dbne	d1,byte_loop");	/* Repeat until not equal or --N < 0 */
    asm ("	bne.s	not_equal");	/* Branch if different */

    asm ("	subi.l  #$10000,d1");	/* Simulate borrow from high 16 bits */
    asm ("	cmpi.l  #-1,d1");	/* Is long word count == -1 */
    asm ("	bne.s   byte_loop");	/* Continue loop if not done */

    asm ("equal:");
    asm ("	moveq	#0,d0");	/* Return 0 for strings equal */
    asm ("	bra.s	return");

    asm ("not_equal:");
    asm ("	bcs.s	ret_m1");
    asm ("	moveq	#1,d0");	/* Return a posive value (1) */
    asm ("	bra.s	return");

    asm ("ret_m1:");
    asm ("	moveq	#-1,d0");	/* Return a negative value (-1) */
    asm ("return:");
}
 
#endif /* EXCLUDE_memcmp.c */
 

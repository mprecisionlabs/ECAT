#include "lib_top.h"


#if EXCLUDE_memset
    #pragma option -Qws			/* Prevent compiler warning */
#else
 
_kill_warning();	/* Eliminates compiler warnings */

/*
;*************************************************************************
;* THIS INFORMATION IS PROPRIETARY TO					**
;* MICROTEC RESEARCH, INC.						**
;*----------------------------------------------------------------------**
;* Copyright (c) 1991 Microtec Research, Inc.				**
;* All rights reserved							**
;*************************************************************************
**
;The software source code contained in this file and in the related
;header files is the property of Microtec Research, Inc. and may only be
;used under the terms and conditions defined in the Microtec Research
;license agreement. You may modify this code as needed but you must retain
;the Microtec Research, Inc. copyright notices, including this statement.
;Some restrictions may apply when using this software with non-Microtec
;Research software.  In all cases, Microtec Research, Inc. reserves all rights.
**

	TTL			'memset'
*
*	File name:		memset.src
*	Author:			Herbert Yuen
*	Date:			85-May-10 Friday
*
*	%W% %G%	*
*
*==============================================================================
*
*			m e m s e t
*
*==============================================================================
* char	*memset(Target_Address,Chr,N)
*
*	Fill N bytes of memory starting at Target_Address with character Chr.
*
*
*/

#include <string.h>

#pragma option -nKf	/* LINK/UNLK will cause code to malfunction */

void   *memset(void * input1, int input2, size_t input3)
{
    asm ("	move.l	d2,-(sp)");	/* Save on stack */
    asm ("	move.l	8(a7),a0");	/* Target_Address in A0 */
    asm ("	move.b	15(a7),d0");	/* Chr */
    asm ("	move.l	16(a7),d1");	/* N */
    asm ("	move.l	a0,a1");	/* Save Target_Address */

    asm ("	move.w	d1,d2");	/* move for byte loop */
    asm ("	cmpi.l	#7,d1");     /* Small amt' to move, so skip hard part */
    asm ("	bls.b	byte_aligned");	/* Jump for small data */
    
    asm ("	move.b	d0,d2");	/* propogate Chr thru register */
    asm ("	lsl.l	#8,d2");	/* shift */
    asm ("	move.b	d0,d2");	/* have half register */
    asm ("	move.w	d2,d0");
    asm ("	swap	d0");
    asm ("	move.w	d2,d0");	/* Chr now propogated thru d0 */

/*	First: long word align */

    asm ("	move.w	a0,d2");	/* Move for manipulation */
    asm ("	andi.w	#3,d2");	/* # byte away from long alignment */
    asm ("	beq.b	long_aligned");	/* jump if already aligned */
    asm ("	bra.s	start_align");	/* begin alignment loop */
    asm ("align:");
    asm ("	move.b	d0,(a0)+");	/* Set one byte at a time */
    asm ("	subq.l	#1,d1");	/* Decrement count */
    asm ("start_align:");
    asm ("	dbf	d2,align");	/* Repeat until --N < 0 */

/*	We are now long word aligned -- so write longs  */

    asm ("long_aligned:");
    asm ("	move.w	d1,d2");	/* Get count */
    asm ("	andi.w	#3,d2");	/* # moves necessary when we finish */
    asm ("	lsr.l	#2,d1");	/* # long word moves to be done */
    asm ("	bra.b	start_long");	/* begin loop */
    asm ("long:");
    asm ("	move.l	d0,(a0)+");	/* write one word */
    asm ("start_long:");
    asm ("	dbf	d1,long");	/* check for termination of loop */
    asm ("	subi.l	#$10000,d1");	/* simulate borrow from high 16 bits */
    asm ("	cmpi.l	#-1,d1");	/* is long word count == -1 */
    asm ("	bne.b	long");		/* continue to loop if not done */
    asm ("	bra.b	byte_aligned");	/* go to byte loop */

/*	Now finish up by writing out the remaining bytes */

    asm ("byte:");
    asm ("	move.b	d0,(a0)+");	/* move out one byte */
    asm ("byte_aligned:");
    asm ("	dbf	d2,byte");	/* and loop */

    asm ("	move.l	(sp)+,d2");	/* restore register */
    asm ("	move.l	a1,d0");	/* Return Target_Address */
}
 
 
#endif /* EXCLUDE_memset.c */
 

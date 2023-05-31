#include "lib_top.h"


#if EXCLUDE_memcpy
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
	TTL			'memcpy'
*
*	File name:		memcpy.src
*	Author:			Herbert Yuen
*	Date:			85-May-10 Friday
*	
*	History:
*
*	04/26/90 jpc:	Re-written to use long moves
*
*	%W% %G%	*
*
*==============================================================================
*
*			m e m c p y
*
*==============================================================================
* char	*memcpy(Target_Address,Source_Address,N)
*
*	Copy N bytes of memory from Source_Address to Target_Address.
*
*
*/

#include <string.h>

#pragma option -nKf	/* LINK/UNLK will cause code to malfunction */

void *memcpy(void * input1, const void * input2, size_t input3)
{
    asm ("	move.l	4(a7),a0");	/* S1 */
    asm ("	move.l	8(a7),a1");	/* S2 */
    asm ("	move.l	12(a7),d1");	/* N */

    asm ("	cmpi.l	#7,d1");	/* small am't data  */
    asm ("	bls.b	byte_aligned"); /* just move by byte */

    asm ("	move.w	a0,d0");	/* check memory alignment */
    asm ("	or.b	(8+3)(a7),d0");	/* or low bytes addr */
    asm ("	btst	#0,d0");	/* byte aligned ? */
    asm ("	bne.b	byte_test");	/* yes */

    asm ("long_aligned:");
    asm ("	move.b	d1,d0");	/* process long data */
    asm ("	andi.l	#3,d0");	/* residual count in d0 */
    asm ("	lsr.l	#2,d1");	/* divide original count by 4 */
    asm ("	bra.b	long_lp2");	/* start the loop */
    asm ("long_lp1:");
    asm ("	move.l	(a1)+,(a0)+");	/* move a long */
    asm ("long_lp2:");
    asm ("	dbf	d1,long_lp1");	/* loop if necessary */
    asm ("	subi.l  #$10000,d1");	/* simulate borrow from high 16 bits */
    asm ("	cmpi.l	#-1,d1");	/* is long word count == -1 */
    asm ("	bne.b   long_lp1");	/* continue loop if not done */

    asm ("	move.l	d0,d1");	/* get residual */
    asm ("	beq.b	byte_finish");	/* and return if no residual */
    asm ("	bra.b	byte_lp2");	/* process residual if necessary */

    asm ("byte_lp1:");
    asm ("	move.b	(a1)+,(a0)+");	/* move a byte */
    asm ("byte_aligned:");
    asm ("byte_lp2:");
    asm ("	dbf	d1,byte_lp1");	/* loop if necessary */
    asm ("	subi.l  #$10000,d1");	/* simulate borrow from high 16 bits */
    asm ("	cmpi.l  #-1,d1");	/* is long word count == -1 */
    asm ("	bne.b   byte_lp1");	/* continue loop if not done */

    asm ("byte_finish:");
    asm ("	move.l	4(a7),d0");	/* set up return value */
    asm ("	bra	return");
    asm ("byte_test:");
    asm ("	move.w	a0,d0");	/* check memory alignment */
    asm ("	and.b	(8+3)(a7),d0");	/* a both aligned on odd addr? */
    asm ("	btst	#0,d0");	/* both odd */
    asm ("	beq.b	byte_aligned");	/* no */

    asm ("	move.b	(a1)+,(a0)+");	/* move a byte */
    asm ("	subq.l	#1,d1");	/* decrement byte count */
    asm ("	bra.b	long_aligned");	/* processed rest as long-aligned data*/
    asm ("return:");
}
 
#endif /* EXCLUDE_memcpy.c */
 

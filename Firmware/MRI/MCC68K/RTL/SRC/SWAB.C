#include "lib_top.h"


#if EXCLUDE_swab
    #pragma option -Qws			/* Prevent compiler warning */
#else
 
/*
**************************************************************************
** THIS INFORMATION IS PROPRIETARY TO					**
** MICROTEC RESEARCH, INC.						**
**----------------------------------------------------------------------**
** Copyright (c) 1991 Microtec Research, Inc.				**
** All rights reserved							**
**************************************************************************
*The software source code contained in this file and in the related
*header files is the property of Microtec Research, Inc. and may only be
*used under the terms and conditions defined in the Microtec Research
*license agreement. You may modify this code as needed but you must retain
*the Microtec Research, Inc. copyright notices, including this statement.
*Some restrictions may apply when using this software with non-Microtec
*Research software.  In all cases, Microtec Research, Inc. reserves all rights.

	TTL			'swab'
*
*	File name:		swab.src
*	Author:			Herbert Yuen
*	Date:			85-May-15 Wednesday
*
* %W% %G% *
*
*==============================================================================
*
*			s w a b
*
*==============================================================================
*  void swab(S,T,N)
*
*	Copy (N+1)/2 words in memory from source address S to target address
*	T, swapping adjacent even and odd bytes.
*
*
*/

#include <mriext.h>

#pragma option -nKf	/* LINK/UNLK will cause code to malfunction */

void        swab(char * input1, char * input2, int input3)
{
    asm ("	move.l	4(a7),a0");	/* Source */
    asm ("	move.l	8(a7),a1");	/* Target */
    asm ("	move.l	12(a7),d1");	/* Number of bytes to swap */

    asm ("	asr.l	#1,d1");	/* Number of words to swap */
    asm ("	bra.s	lp2");

    asm ("lp1:");
    asm ("	move.b	(a0)+,d0");	/* Low order byte in D0 */
    asm ("	move.b	(a0)+,(a1)+");	/* High order byte to target low */
    asm ("	move.b	d0,(a1)+");	/* Low order byte to target high */

    asm ("lp2:");
    asm ("	dbf	d1,lp1");	/* Repeat until --count < 0 */
    asm ("	subi.l  #$10000,d1");	/* Simulate borrow from high 16 bits */
    asm ("	cmpi.l  #-1,d1");	/* Is long word count == -1 */
    asm ("	bne.s   lp1");		/* Continue loop if not done */
}
 
#endif /* EXCLUDE_swab.c */
 

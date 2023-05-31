#include "lib_top.h"


#if EXCLUDE_strpbrk
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
*The software source code contained in this file and in the related
*header files is the property of Microtec Research, Inc. and may only be
*used under the terms and conditions defined in the Microtec Research
*license agreement. You may modify this code as needed but you must retain
*the Microtec Research, Inc. copyright notices, including this statement.
*Some restrictions may apply when using this software with non-Microtec
*Research software.  In all cases, Microtec Research, Inc. reserves all rights.

	TTL			'strpbrk'
*
*	File name:		strpbrk.src
*	Author:			Herbert Yuen
*	Date:			85-Jun-02 Sunday
*
*	%W% %G%	*
*
*==============================================================================
*
*			s t r p b r k
*
*==============================================================================
* char	*strpbrk (S1,S2)
*
*	Returns a pointer to the first occurrence in string S1 of any char
*	from string S2, or NULL pointer if no character from S2 exists in S1.
*
*
*/

#pragma option -nKf	/* LINK/UNLK will cause code to malfunction */

char   *strpbrk(const char * input1, const char * input2)
{
    asm ("	move.l	4(a7),a0");	/* S1 */
    asm ("	move.l	8(a7),d1");	/* S2 */
    asm ("	moveq	#0,d0");	/* Clear D0 as NULLPTR */

    asm ("lp1:");
    asm ("	move.b	(a0)+,d0");	/* Get one char from S1 */
    asm ("	beq.s	return");	/* Return NULLPTR if end of S1 */
    asm ("	move.l	d1,a1");	/* A1 -> S2 */
    asm ("lp2:");
    asm ("	cmp.b	(a1),d0");	/* *S2 == Chr ? */
    asm ("	beq.s	found");
    asm ("	tst.b	(a1)+");	/* End of S2 ? */
    asm ("	bne	lp2");		/* Scan until end of S2 */
    asm ("	bra	lp1");		/* If not found, try next char in S1 */

    asm ("found:");
    asm ("	subq.l	#1,a0");	/* Back up 1 char of ptr to S1 */
    asm ("	move.l	a0,d0");	/* Return ptr in D0 */

    asm ("return:");
}
 
#endif /* EXCLUDE_strpbrk.c */
 

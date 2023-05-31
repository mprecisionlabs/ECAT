#include "lib_top.h"


#if EXCLUDE_strrchr
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

	TTL			'strrchr'
*
*	File name:		strrchr.src
*	Author:			Herbert Yuen
*	Date:			85-May-10 Friday
*
* %W% %G% *
*
*==============================================================================
*
*			s t r r c h r
*
*==============================================================================
* char	*strrchr(String,Chr)
*
*	Search string S from right to left for character Chr.
*	Return a pointer to the position.  If not found, return a null pointer.
*	"strrchr" replaces the obsolete Unix (V7) "rindex".
*
*/

#include <string.h>

#pragma option -nKf	/* LINK/UNLK will cause code to malfunction */

char   *strrchr(const char * input1, int input2)
{
    asm ("	move.l	4(a7),a0");	/* S1 */
    asm ("	move.b	11(a7),d0");	/* Chr */
    asm ("	move.l	a0,d1");	/* Save address of S1 in D1 */

    asm ("lp1:");
    asm ("	tst.b	(a0)+");	/* Find end of S1 */
    asm ("	bne	lp1");

    asm ("	sub.l	a0,d1");	/* -(Length of S1, including 0) */
    asm ("	not.l	d1");		/* Length of S1, excluding 0 */

    asm ("lp2:");
    asm ("	cmp.b	-(a0),d0");	/* *--S1 == Chr ? */
    asm ("	dbeq	d1,lp2");	/* Repeat until found or --length < 0 */
    asm ("	beq.s	found");

    asm ("	subi.l  #$10000,d1");	/* Simulate borrow from high 16 bits */
    asm ("	cmpi.l  #-1,d1");	/* Is long word count == -1 */
    asm ("	bne.s   lp2");		/* Continue loop if not done */

    asm ("	moveq	#0,d0");	/* Return NULLPTR when not found */
    asm ("	bra	return");

    asm ("found:");
    asm ("	move.l	a0,d0");	/* Returns ptr to Chr in S1 */
    asm ("return:");
}
 
#endif /* EXCLUDE_strrchr.c */
 

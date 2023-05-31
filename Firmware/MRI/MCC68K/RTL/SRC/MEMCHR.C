#include "lib_top.h"


#if EXCLUDE_memchr
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

	TTL			'memchr'
*
*	File name:		memchr.src
*	Author:			Herbert Yuen
*	Date:			85-May-10 Friday
*
*	%W% %G%	*
*
*==============================================================================
*
*			m e m c h r
*
*==============================================================================
* char	*memchr(Target_Address,Chr,N)
*
*	Search N bytes of memory starting at Target_Address for character Chr.
*	Return pointer to position.  If not found, return null pointer.
*
*/

#include <string.h>

#pragma option -nKf	/* LINK/UNLK will cause code to malfunction */

void *memchr(const void * input1, int input2, size_t input3)
{
    asm ("	move.l	4(a7),a0");	/* S1 */
    asm ("	move.b	11(a7),d0");	/* Chr */
    asm ("	move.l	12(a7),d1");	/* N */
/* */
    asm ("	subq.l	#1,d1");	/* --N >= 0 ? */
    asm ("	blt.s	notfound");	/* If not, Chr is not found */
    asm ("loop:");
    asm ("	cmp.b	(a0)+,d0");	/* *S1++ == Chr ? */
    asm ("	dbeq	d1,loop");	/* Repeat until found or --N < 0 */
    asm ("	beq.s	found");	/* Found character */
/* */
    asm ("	subi.l  #$10000,d1");	/* Simulate borrow from high 16 bits */
    asm ("	cmpi.l  #-1,d1");	/* Is long word count == -1 */
    asm ("	bne.s   loop");		/* Continue loop if not done */
/* */
    asm ("notfound:");
    asm ("	moveq	#0,d0");	/* Return NULLPTR if Chr not found */
    asm ("	bra	return");
/* */
    asm ("found:");
    asm ("	subq.l	#1,a0");	/* Returns --S1 in D0 */
    asm ("	move.l	a0,d0");
    asm ("return:");
}
 
#endif /* EXCLUDE_memchr.c */
 

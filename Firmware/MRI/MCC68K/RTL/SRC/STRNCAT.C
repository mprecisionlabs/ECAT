#include "lib_top.h"


#if EXCLUDE_strncat
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

	TTL			'strncat'
*
*	File name:		strncat.src
*	Author:			Herbert Yuen
*	Date:			85-May-10 Friday
*
* %W% %G% *
*
*==============================================================================
*
*			s t r n c a t
*
*==============================================================================
* char	*strncat(Target_String,Append_String,N)
*
*	Append Append_String to Target_String, up to N bytes.
*	Target_String and Append_String are 0 terminated C strings.
*	N is an unsigned int.
*
*
*/

#include <string.h>

#pragma option -nKf	/* LINK/UNLK will cause code to malfunction */

char *strncat(char * input1, const char * input2, size_t input3)
{
    asm ("	move.l	4(a7),a0");	/* S1 */
    asm ("	move.l	8(a7),a1");	/* S2 */
    asm ("	move.l	12(a7),d1");	/* N */
    asm ("	move.l	a0,d0");	/* Returns S1 */
    asm ("lp1:");
    asm ("	tst.b	(a0)+");	/* Find end of S1 */
    asm ("	bne	lp1");

    asm ("	subq.l	#1,a0");	/* Start append from the NULL char */
    asm ("lp2:");
    asm ("	move.b	(a1)+,(a0)+");	/* *S1++ = *S2++ */
    asm ("	dbeq	d1,lp2");	/* Repeat until end of S2 or --N < 0 */
    asm ("	beq	found_null");	/* Found end of string */

    asm ("	subi.l  #$10000,d1");	/* Simulate borrow from high 16 bits */
    asm ("	cmpi.l  #-1,d1");	/* Is long word count == -1 */
    asm ("	bne.s   lp2");		/* Continue loop if not done */

    asm ("	clr.b	-(a0)");	/* Always NULL-terminate S1 */
    asm ("found_null:");
}
 
#endif /* EXCLUDE_strncat.c */
 

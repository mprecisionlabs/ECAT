#include "lib_top.h"


#if EXCLUDE_strncpy
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

	TTL			'strncpy'
*
*	File name:		strncpy.src
*	Author:			Herbert Yuen
*	Date:			85-May-10 Friday
*
* %W% %G% *
*
*==============================================================================
*
*			s t r n c p y
*
*==============================================================================
* char	*strncpy(Target_String,Source_String,N)
*
*	Copy Source_String to Target_String for N bytes.
*	Source_String and Target_String are 0 terminated C strings.
*	N is an unsigned int.
*
*
*/

#include <string.h>

#pragma option -nKf	/* LINK/UNLK will cause code to malfunction */

char *strncpy(char * input1, const char * input2, size_t input3)
{
    asm ("	move.l	4(a7),a0");	/* S1 */
    asm ("	move.l	8(a7),a1");	/* S2 */
    asm ("	move.l	a0,d0");	/* Returns S1 */
    asm ("	move.l	12(a7),d1");	/* N */

    asm ("	bra.s	lp2");		/* (cc already set for N) */
    asm ("lp1:");
    asm ("	move.b	(a1)+,(a0)+");	/* *S1++ = *S2++ */
    asm ("lp2:");
    asm ("	dbeq	d1,lp1");	/* Repeat until end of S2 or --N < 0 */
    asm ("        beq.s   lp4");
    asm ("        subi.l  #$10000,d1");	/* Simulate borrow from high 16 bits */
    asm ("        cmpi.l  #-1,d1");	/* Is long word count == -1 */
    asm ("        bne.s   lp1");	/* Continue loop if not done */

    asm ("        bra	return");

    asm ("lp3:");
    asm ("	clr.b	(a0)+");	/* Pad with NULL in S1 */
    asm ("lp4:");
    asm ("	dbf	d1,lp3");	/* Repeat until --N < 0 */
    asm ("	subi.l  #$10000,d1");	/* Simulate borrow from high 16 bits */
    asm ("	cmpi.l  #-1,d1");	/* Is long word count == -1 */
    asm ("	bne.s   lp3");		/* Continue loop if not done */
    asm ("return:");
}
 
#endif /* EXCLUDE_strncpy.c */
 

#include "lib_top.h"


#if EXCLUDE_strchr
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

	TTL			'strchr'
*
*	File name:		strchr.src
*	Author:			Herbert Yuen
*	Date:			85-May-10 Friday
*
* %W% %G% *
*
*==============================================================================
*
*			s t r c h r
*
*==============================================================================
* char	*strchr(String,Chr)
*
*	Search string S from left to right for character Chr.
*	Return pointer to position.  If not found, return null pointer.
*	"strchr" replaces the obsolete Unix (V7) "index".
*
*/

#include <string.h>

#pragma option -nKf	/* LINK/UNLK will cause code to malfunction */

char *strchr(const char * input1, int input2)
{
    asm ("	move.l	4(a7),a0");	/* S1 */
    asm ("	move.b	11(a7),d0");	/* Chr */
    asm ("loop:");
    asm ("	cmp.b	(a0),d0");	/* *S1 == Chr ? */
    asm ("	beq.s	found");
    asm ("	tst.b	(a0)+");	/* End of S1 ? */
    asm ("	bne	loop");

    asm ("	moveq	#0,d0");	/* Return NULLPTR if Chr not found */
    asm ("	bra	return");
    asm ("found:");
    asm ("	move.l	a0,d0");	/* Returns ptr to Chr in S1 */
    asm ("return:");
}
 
 
#endif /* EXCLUDE_strchr.c */
 

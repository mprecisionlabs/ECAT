#include "lib_top.h"


#if EXCLUDE_memccpy
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

	TTL			'memccpy'
*
*	File name:		memccpy.src
*	Author:			Herbert Yuen
*	Date:			85-May-14 Tuesday
*
*	%W% %G%	*
*
*==============================================================================
*
*			m e m c c p y
*
*==============================================================================
* char	*memccpy(Target_Address,Source_Address,Chr,N)
*
*	Copy up to N bytes of memory from Source_Address to Target_Address or
*	after Chr is copied.  Returns a pointer to target memory right after
*	Chr or NULLPTR if Chr is not found.
*
*/

#include <mriext.h>

#pragma option -nKf	/* LINK/UNLK will cause code to malfunction */

char *memccpy (char * input1, const char * input2, int input3, size_t input4)
{
    asm ("	move.l	4(a7),a0");	/* S1 */
    asm ("	move.l	8(a7),a1");	/* S2 */
    asm ("	move.b	15(a7),d0");	/* Chr */
    asm ("	move.l	16(a7),d1");	/* N */
    asm ("loop:");
    asm ("	subq.l	#1,d1");	/* --N >= 0 ? */
    asm ("	blt.s	quit");		/* Exit loop if --N < 0 */

    asm ("	move.b	(a1)+,(a0)");	/* *S1 = *S2++ */
    asm ("	cmp.b	(a0)+,d0");	/* *S1++ == Chr ? */
    asm ("	bne	loop");

    asm ("	move.l	a0,d0");	/* Return ptr to after Chr */
    asm ("	bra	return");
    asm ("quit:");
    asm ("	moveq	#0,d0");	/* Return NULLPTR if Chr not found */
    asm ("return:");
}
 
#endif /* EXCLUDE_memccpy.c */
 

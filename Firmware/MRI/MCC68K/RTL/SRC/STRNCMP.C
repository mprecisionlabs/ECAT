#include "lib_top.h"


#if EXCLUDE_strncmp
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

	TTL			'strncmp'
*
*	File name:		strncmp.src
*	Author:			Herbert Yuen
*	Date:			85-May-10 Friday
*
*	%W% %G%	*
*
*==============================================================================
*
*			s t r n c m p
*
*==============================================================================
* int	strncmp(S1,S2,N)
*
*	Compare S1 to S2 for N bytes.  Return -1,0,+1 as <,=,>.
*	S1 and S2 are 0 terminated C strings.
*	N is an unsigned int.
*
*/

#include <string.h>

#pragma option -nKf	/* LINK/UNLK will cause code to malfunction */

int strncmp(const char * input1, const char * input2, size_t input3)
{
    asm ("	move.l	4(a7),a0");	/* S1 */
    asm ("	move.l	8(a7),a1");	/* S2 */
    asm ("	move.l	12(a7),d1");	/* N */

    asm ("loop:");
    asm ("	subq.l	#1,d1");	/* --N >= 0 ? */
    asm ("	blt.s	equal");	/* N < 0 means strings are equal */

    asm ("	move.b	(a0)+,d0");	/* *S1++ in D0 */
    asm ("	cmp.b	(a1)+,d0");	/* *S2++ == D0 ? */
    asm ("	bne.s	noteq");	/* Exit loop if not equal */
    asm ("	tst.b	d0");		/* End of S1 ? */
    asm ("	bne.s	loop");		/* If not, continue */

    asm ("equal:");
    asm ("	moveq	#0,d0");	/* Return 0 for strings equal */
    asm ("	bra.s	return");

    asm ("noteq:");
    asm ("	bcs.b	ret_m1");	/* *--S1 - *--S2 */
    asm ("	moveq	#1,d0");	/* return a positive value */
    asm ("	bra.s	return");

    asm ("ret_m1:");
    asm ("	moveq	#-1,d0");	/* return a negative value */

    asm ("return:");
}
 
#endif /* EXCLUDE_strncmp.c */
 

#include "lib_top.h"


#if EXCLUDE_strcat
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

	TTL			'strcat'
*
*	File name:		strcat.src
*	Author:			Herbert Yuen
*	Date:			85-May-09 Thursday
*
*	%W% %G%	*
*
*==============================================================================
*
*			s t r c a t
*
*==============================================================================
* char	*strcat(Target_String,Append_String)
*
*	Append Append_String onto Target_String.  Return Target_String.
*	Note:  Target_String and Append_String are 0 terminated C strings.
*
*/

#include <string.h>

#pragma option -nKf	/* LINK/UNLK will cause code to malfunction */

char *strcat(char * input1, const char * input2)
{
    asm ("	move.l	4(a7),a0");	/* S1 */
    asm ("	move.l	8(a7),a1");	/* S2 */
    asm ("	move.l	a0,d0");	/* Returns S1 */
    asm ("lp1:");
    asm ("	tst.b	(a0)+");	/* Get to end of S1 */
    asm ("	bne	lp1");

    asm ("	subq.l	#1,a0");	/* Start append from the NULL char */
    asm ("lp2:");
    asm ("	move.b	(a1)+,(a0)+");	/* *S1++ = *S2++ */
    asm ("	bne	lp2");
}
 
#endif /* EXCLUDE_strcat.c */
 

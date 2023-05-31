#include "lib_top.h"


#if EXCLUDE_strlen
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

	TTL			'strlen'
*
*	File name:		strlen.src
*	Author:			Herbert Yuen
*	Date:			85-May-10 Friday
*
* %W% %G% *
*
*==============================================================================
*
*			s t r l e n
*
*==============================================================================
*  int	strlen(String)
*
*	Return the length of S
*	S is a 0 terminated C string.
*
*
*/

#include <string.h>

#pragma option -nKf	/* LINK/UNLK will cause code to malfunction */

size_t strlen(const char * input1)
{
    asm ("	move.l	4(a7),a0");	/* Address argument S */
    asm ("	move.l	a0,d0");	/* Also save in D0 */
    asm ("loop:");
    asm ("	tst.b	(a0)+");	/* Find end of S */
    asm ("	bne	loop");	

    asm ("	sub.l	a0,d0");	/* -(length of S, including 0) */
    asm ("	not.l	d0");		/* Length of S, excluding 0 */
}
 
#endif /* EXCLUDE_strlen.c */
 

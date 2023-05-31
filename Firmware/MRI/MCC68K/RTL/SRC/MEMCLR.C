#include "lib_top.h"


#if EXCLUDE_memclr
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

	TTL			'memclr'
*
*	File name:		memclr.src
*	Author:			Herbert Yuen
*	Date:			85-May-09 Thursday
*	Revision history:	05/20/85 HY - rename from memclear to memclr
*
*	%W% %G%	*
*
*==============================================================================
*
*			m e m c l r
*
*==============================================================================
* char	*memclr(Target_Address,N)
*
*	Clear N bytes of memory starting at Target_Address (fill with NULL).
*
*
*/

#include <mriext.h>

#pragma option -nKf	/* LINK/UNLK will cause code to malfunction */

char   *memclr (char * input1, size_t input2)
{
    asm ("	move.l	4(a7),a0");	/* Target_Address in A0 */
    asm ("	move.l	8(a7),d1");	/* N */
    asm ("	move.l	a0,d0");	/* Returns Target_Address */

    asm ("	bra.s	lp2");
    asm ("lp1:");
    asm ("	clr.b	(a0)+");	/* Clear one byte at a time */
    asm ("lp2:");
    asm ("	dbf	d1,lp1");	/* Repeat until --N < 0 */
    asm ("	subi.l  #$10000,d1");	/* Simulate borrow from high 16 bits */
    asm ("	cmpi.l  #-1,d1");	/* Is long word count == -1 */
    asm ("	bne.s   lp1");		/* Continue loop if not done */
}

 
#endif /* EXCLUDE_memclr.c */
 

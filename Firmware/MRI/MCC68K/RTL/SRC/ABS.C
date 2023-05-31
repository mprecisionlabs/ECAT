#include "lib_top.h"


#if EXCLUDE_abs
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
;The software source code contained in this file and in the related
;header files is the property of Microtec Research, Inc. and may only be
;used under the terms and conditions defined in the Microtec Research
;license agreement. You may modify this code as needed but you must retain
;the Microtec Research, Inc. copyright notices, including this statement.
;Some restrictions may apply when using this software with non-Microtec
;Research software.  In all cases, Microtec Research, Inc. reserves all rights.

	TTL			'abs'
*
*	File name:		abs.src
*	Author:			Herbert Yuen
*	Date:			85-May-22 Wednesday
*
* %W% %G% *
*
*==============================================================================
*
*			a b s
*
*==============================================================================
*  int	abs (X)
*
*	Return the absoulte value of integer X.
*
*/

#include <stdlib.h>

#pragma option -nKf	/* LINK/UNLK will cause code to malfunction */

int abs (int input)
{
    asm ("	move.l	4(a7),d0");	/* Get X into D0 */
    asm ("	bpl.s	return");	/* Skip if X is positive */
    asm ("	neg.l	d0");		/* X = -X */
    asm ("return:");
}
 
#endif /* EXCLUDE_abs.c */
 

#include "lib_top.h"


#if EXCLUDE_lmul
    #pragma option -Qws			/* Prevent compiler warning */
#else
 
_kill_warning();	/* Eliminates compiler warnings */

#pragma asm
 
#if _OPTION_Kt
  #informing -- The "-Kt" option is disregarded by this routine.
#endif

#if _OPTION_upd || _OPTION_us
  #error -- The "-upd" and "-us" options cannot be applied to this routine.
#endif

#if _OPTION_utn
  #error -- The "-ut<number>" option cannot be applied to this routine.
#endif

*************************************************************************/
** THIS INFORMATION IS PROPRIETARY TO					*/
** MICROTEC RESEARCH, INC.						*/
** 2350 Mission College Blvd.						*/
** Santa Clara, CA 95054						*/
** USA									*/
**----------------------------------------------------------------------*/
** Copyright (c) Microtec Research, Inc. 1989. All rights reserved. 	*/
*************************************************************************/
*The software source code contained in this file and in the related
*header files is the property of Microtec Research, Inc. and may only be
*used under the terms and conditions defined in the Microtec Research
*license agreement. You may modify this code as needed but you must retain
*the Microtec Research, Inc. copyright notices, including this statement.
*Some restrictions may apply when using this software with non-Microtec
*Research software.  In all cases, Microtec Research, Inc. reserves all rights.

*************************************************************************
*	This file contains the run time support for 32 bit 		*
*		multiplication for 68K -> 68012				*
*									*
*	Written by jack christensen					*
*************************************************************************
*
	opt	case
	section	code
*************************************************************************
*	lmult ulmul							*
*									*
*	Unsigned and signed muliply routines				*
*		In practice, they are the same function, so they are	*
*		the same entry point here.				*
*									*
*	Input args in d0 and d1						*
*	Output is in d0							*
*									*
*	Changes								*
*	6/23/89	jpc	Added entry points for __lmul & __almul		*
*	8/02/89 jpc	Removed use of Ax registers			*
*									*
*************************************************************************
*
* %W% %G% *
*
*
	xdef	lmult
	xdef	ulmult
	xdef	__lmul
	xdef	__almul
*
__almul:
	move.l	(a0),d1		move 1st argument into d1
__lmul:
lmult:
ulmult:
	movem.l	d0/d2,-(sp)	save d0 and d2 on the stack - d2 will be scratch
	move.l	d0,d2		d0 into scratch
*
	mulu.w	d1,d0		multiply two lower words
*
	clr.w	d2		zap low word of original d0
	swap	d2		high word to low
	beq.b	lmult_z1	jump if result of multiply will be zero
*
	mulu.w	d1,d2		execute multiply
	swap	d2		low word to hi
	clr.w	d2		zero low word
	add.l	d2,d0		add into d0 (the result register)
*
lmult_z1:
	move.l	(sp)+,d2	re-establish d2 -- and pop orig. d0 off stack
	clr.w	d1		clear low word of d1
	swap	d1		high word to lo
	beq.b	lmult_z2	jump if result of mult. will be zero
*
	mulu.w	d1,d2		multiply
	swap	d2		re-arrange
	clr.w	d2		zap lower word
	add.l	d2,d0		add into result register
*
lmult_z2:
	move.l	(sp)+,d2	get back d2
	rts			and return
 
#pragma endasm
 
#endif /* EXCLUDE_lmul.c */
 

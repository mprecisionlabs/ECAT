#include "lib_top.h"


#if EXCLUDE_fcmpi
    #pragma option -Qws			/* Prevent compiler warning */
#else
 
_kill_warning();	/* Eliminates compiler warnings */

#if _OPTION_Kt
  #informing -- The "-Kt" option is disregarded by this routine.
#endif

#if _OPTION_upd || _OPTION_us
  #error -- The "-upd" and "-us" options cannot be applied to this routine.
#endif

#if _OPTION_utn
  #error -- The "-ut<number>" option cannot be applied to this routine.
#endif

#pragma asm
 
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
*
*************************************************************************
*	Written by jack christensen					*
*************************************************************************
*
*************************************************************************
*	fcmpi								*
*	double precision floating point compare routine			*
*									*
*	This routine compares two double precision floating pnt numbers	*
*	D0 - high order first arg					*
*	D1 - low order first arg					*
*	D2 - high order second arg					*
*	D3 - low order second arg					*
*									*
*	Negative zero compare equal to zero				*
*	Infinities and NaN's are treated as ordered numbers and results	*
*		are returned as though they were			*
*************************************************************************
*
* %W% %G% *
*
	opt	case
	xdef	fcmpi
	section	code
*
fcmpi:
	move.l	d1,a0		save for future
	movem.l	4(sp),d1/a1	position stack arg for processing 
*
	move.l	(sp),8(sp)	put return address correctly on stack for rtn.
	addq.l	#8,sp		pop stack to correct spot
*
	bsr.b	fscmpi		compare hi order data
	bne.b	fcmpi_return	go to return
*
	tst.l	d0		hi order args equal -- are they negative
	bmi.b	fcmpi_minus	they are negative
*	
	cmp.l	a1,a0		do compare
	bra.b	fcmpi_cmp_fin	we've done the compare, now finish up
*
fcmpi_minus:
	cmp.l	a0,a1		do compare
fcmpi_cmp_fin:			
*				let's convert result to signed compare situation
	bcs.b	fcmpi_greater	greater
	beq.b	fcmpi_return	equal
	move.w	#$0,CCR		turn off all ccr bits for less than
fcmpi_return:
	rts			and return
fcmpi_greater:
	move.w	#$8,CCR		turn on N bit
	rts			and return
*
*************************************************************************
*	fscmpi								*
*	single precision floating point compare routine			*
*									*
*	This routine compares two single precision floating pnt numbers	*
*	input arguments are in d0 and d1				*
*									*
*	Negative zero compare equal to zero				*
*	Infinities and NaN's are treated as ordered numbers and results	*
*		are returned as though they were			*
*************************************************************************
	xdef	fscmpi
*
fscmpi:
	tst.l	d0		is first arg negative
	bge.b	first_pos	no -- its positive
*
	andi.l	#$7fffffff,d0	zap sign
	neg.l	d0		negate
first_pos:
	tst.l	d1		is second arg negative
	bge.b	second_pos	no -- it's positive
*
	andi.l	#$7fffffff,d1	zap sign
	neg.l	d1		negate
second_pos:
	cmp.l	d1,d0		do actual compare here
	rts			return
 
#pragma endasm
 
#endif /* EXCLUDE_fcmpi.c */
 

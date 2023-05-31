#include "lib_top.h"


#if EXCLUDE_ldiv
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
*	This file contains the run time support for 32 bit division	*
*		for 68K -> 68012					*
*									*
*	Written by jack christensen					*
*************************************************************************
*
	opt	case
	section	code
*************************************************************************
*	ldivt								*
*									*
*	Signed long division	32 bits					*
*	dividend in d0							*
*	divisor  in d1							*
*	quotient returned in d0						*
*									*
*	quotient has sign of ^ of sign of dividend and divisor		*
*									*
*	Changes								*
*	6/23/89	jpc:	Added entry pnts for __ldiv, __lrem, __uldiv,	*
*			__ulrem, __aldiv, __alrem, __auldiv, __aulrem	*
*	8/02/89 jpc:	Removed use of Ax registers.			*
*************************************************************************
*
* %W% %G% *
*
	xdef	ldivt
	xdef	__ldiv
	xdef	__aldiv
*
__aldiv:
	move.l	(a0),d1		get first argument
	exg	d0,d1		put args in right location for rest --
*
__ldiv:
ldivt:
	cmp.l	#$7fff,d1	divisor too big ??
	bgt.b	ldivt_cont	yes -- do it the hard way
	cmp.l	#$ffff8000,d1	again -- divisor too big ??
	blt.b	ldivt_cont	yes -- do it the hard way
*
	divs.w	d1,d0		try to divide
	bvs.b	ldivt_cont	didn't make it this time even -- do it hard way
*
	ext.l	d0		sign extend the result
	rts			return
*
ldivt_cont:
	move.l	d2,-(sp)	save d2
	clr.b	d2		pre-clear d2 -- will be used as flag
*
	tst.l	d0		dividend negative ??
	bge.b	ldivt_dvnd	no -- jump
*
	neg.l	d0		make positive
	not.b	d2		toggle flag
*
ldivt_dvnd:
	tst.l	d1		divisor negative
	bge.b	ldivt_dvsr	no -- jump
*
	neg.l	d1		make positive
	not.b	d2		toggle flag
*
ldivt_dvsr:
	bsr.b	gendiv		perform division
	tst.b	d2		is negation flag on
	beq.b	ldivt_ender	no -- jump
*
	neg.l	d0		negate result
*
ldivt_ender:
	move.l	(sp)+,d2	recover d2
	rts			return
	page
*************************************************************************
*	gendiv -- uldivt						*
*									*
*	generic 32 bit division routine -- used by most other division	*
*		oriented rts routines.  Almost as a matter of luck	*
*		it also handles unsigned mod with no other help		*
*		therefore the "ulmodt" entry point			*
*									*
*	dividend is in d0						*
*	divisor is in d1						*
*	remainder in d1							*
*	quotient in d0							*
*									*
*	Changes:							*
*	6/27/89	jpc:  Added trial divide to eliminate code emitted by	*
*			codegen						*
*************************************************************************
	xdef	uldivt
	xdef	__uldiv
	xdef	__auldiv
*
__auldiv:
	move.l	(a0),d1		get first argument
	exg	d0,d1		put args in right location for rest --
*
__uldiv:
uldivt:
gendiv:
	cmp.l	#$10000,d1	do we have 16 bit divisor
	bcc.b	gendiv_dvsr	no -- branch to subtraction algorithm
*
	divu.w	d1,d0		try to divide
	bvs.b	gendiv_cont	got an overflow -- we must do it the hard way
*
	moveq.l	#0,d1		pre-clear
	move.w	d0,d1		quotient to d1
	clr.w	d0		clear lo d0
	swap	d0		remainer in correct word
	exg	d0,d1		exchange d0 and d1
	rts			and return
*
gendiv_cont:
	move.l	d2,-(sp)	d2 will be scratch
	move.l	d1,d2		in goes the divisor
	move.l	d0,d1		dividend also in d1
*
	clr.w	d0		move hi word to lo
	swap	d0
	divu.w	d2,d0		all 16 bit divide
	swap	d0		re-arrange for finish
*
	swap	d1		prepare to receive remainder
	move.w	d0,d1		put in remainder
	swap	d1		make everything right
*
	divu.w	d2,d1		preliminary answer in d0
*
	move.w	d1,d0		low word of quotient into result
	clr.w	d1		clear lo word
	swap	d1		position remainder correcly
	move.l	(sp)+,d2	recover d2
	rts			return
*
gendiv_dvsr:
	movem.l	d2/d3,-(sp)	save d2 and d3 on stack
	move.l	d1,d3		position divisor for processing
	move.l	d0,d1		dividend to d1
	swap	d0		lo word to hi
	clr.w	d0		
	clr.w	d1		hi word to lo
	swap	d1
	moveq.l	#$f,d2		set up cycle count
*
gendiv_loop:
	add.l	d0,d0		shift d1:d0 regs one bit left
	addx.l	d1,d1
	cmp.l	d3,d1		check to see if we can subtract
	bcs.b	gendiv_loopend	to bottom of loop if cannot subtract
*
	sub.l	d3,d1
	addq.b	#1,d0		put 1 into quotient register
gendiv_loopend:
	dbf	d2,gendiv_loop	keep going on loop
*
	movem.l	(sp)+,d2/d3	recover d2 and d3 from stack
	rts			return
	page
*************************************************************************
*	ulmodt								*
*									*
*	Unisigned long modulo						*
*	dividend in d0							*
*	divisor	 in d1							*
*	remainder returned in d0					*
*************************************************************************
	xdef	ulmodt
	xdef	__ulrem
	xdef	__aulrem
*
__aulrem:
	move.l	(a0),d1		get argument
	exg	d0,d1		put in correct locations
*
__ulrem:
ulmodt:
	cmp.l	#$10000,d1	do we have 16 bit divisor
	bcc.b	ulmodt_cont	no -- do it the hard way
*
	divu.w	d1,d0		try to divide
	bvs.b	ulmodt_cont	got an overflow -- we must do it the hard way
*
	clr.w	d0		clear out quotient
	swap	d0		swap bytes in d0
	rts			return
*
ulmodt_cont:
	bsr.b	gendiv		execute division
	move.l	d1,d0		move remainder to correct location
	rts			and return
*
*
*************************************************************************
*	lmodt								*
*									*
*	signed modulo							*
*	dividend in d0							*
*	divisor  in d1							*
*	mod (remainder) in d0 on return					*
*									*
*	remainder has sign of dividend					*
*************************************************************************
	xdef	lmodt
	xdef	__lrem
	xdef	__alrem
*
__alrem:
	move.l	(a0),d1		get first argument
	exg	d0,d1		exchange to place correctly
*
__lrem:
lmodt:
	cmp.l	#$7fff,d1	divisor too big ??
	bgt.b	lmodt_cont	yes -- do it the hard way
	cmp.l	#$ffff8000,d1	again -- divisor too big ??
	blt.b	lmodt_cont	yes -- do it the hard way
*
	divs.w	d1,d0		try to divide
	bvs.b	lmodt_cont	didn't make it this time even -- do it hard way
*
	swap	d0		swap bytes in d0
	ext.l	d0		sign extend the result
	rts			return
*
lmodt_cont:
	move.l	d2,-(sp)	save d2
	clr.l	d2		set to zero, we'll be using it as flag
*
	tst.l	d1		test the divisor
	bge.b	lmodt_dvsr	divisor is negative
	neg.l	d1		make positive
*
lmodt_dvsr:
	tst.l	d0		test the dividend
	bge.b	lmodt_dvnd	dividend is negative
*
	neg.l	d0		negate dividend
	addq.b	#1,d2		set flag
*
lmodt_dvnd:
	bsr.w	gendiv		perform division
	move.l	d1,d0		get modulo
	tst.b	d2		test flag
	beq.b	lmodt_end	branch if no need to negate result
*
	neg.l	d0		negate result
*
lmodt_end:
	move.l	(sp)+,d2	restore d2
	rts			and return
 
#pragma endasm
 
#endif /* EXCLUDE_ldiv.c */
 

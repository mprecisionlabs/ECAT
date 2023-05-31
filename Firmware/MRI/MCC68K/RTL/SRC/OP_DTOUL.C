#include "lib_top.h"


#if EXCLUDE_op_dtoul
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
 
;************************************************************************/
;* THIS INFORMATION IS PROPRIETARY TO					*/
;* MICROTEC RESEARCH, INC.						*/
;*----------------------------------------------------------------------*/
;* Copyright (c) 1991 Microtec Research, Inc.				*/
;* All rights reserved							*/
;************************************************************************/
;The software source code contained in this file and in the related
;header files is the property of Microtec Research, Inc. and may only be
;used under the terms and conditions defined in the Microtec Research
;license agreement. You may modify this code as needed but you must retain
;the Microtec Research, Inc. copyright notices, including this statement.
;Some restrictions may apply when using this software with non-Microtec
;Research software.  In all cases, Microtec Research, Inc. reserves all rights.
;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;
;	op_DtoUL.s
;
;	Converts double a to unsigned long, and
;	return the result on top of the stack.
;	Readjust stack accordingly and returns to the calling program.
;	Supplies function  __dtoul()  that is used by the BE.
;
;	HISTORY:
;	5/19/89 mjf: Created
;	6/19/89 jpc: Written
;	7/5/89	jpc: Fixed bug -- was not isolating exponent correctly
;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
*
* %W% %G% *
*

	opt	case
	section code
	xdef	__dtoul
*
__dtoul:
	move.l	(sp)+,a1		get return address
	movem.l	(sp)+,d0/d1		get double argument
	move.l	d2,-(sp)		save d2 onto stack
*
	move.l	d0,d2			move hi order arg to get sign
	andi.l	#$80000000,d2		get sign bit
	move.l	d2,a0			squirrel away
*
	andi.l	#$7fffffff,d0		zap sign bit if there
	move.l	d0,d2			get hi order again
	swap	d2			swap words
	lsr.w	#4,d2			do allignment
	subi.w	#$3ff,d2		get rid of offset
*
	cmp.w	#-1,d2			do we have fraction ??
	ble	zero			yes
*
	andi.l	#$000fffff,d0		mask off exponent
	ori.l	#$00100000,d0		put back in invisible bit
*
	cmp.w	#20,d2			which direction to we shift
	bgt	left_entry		must shift left
*
*					handle right shifts !!!
	neg.w	d2			negate exponent
	addi.w	#20,d2			add in 20
	lsr.l	d2,d0			shift in one big swoop
	bra	finish			return
*
left_entry:
	cmp.w	#32,d2			will we have overflow ??
	bge	overflow		yes !!
*
	subi.w	#21,d2			pre-subtract
*
left_loop:
	lsl.l	#1,d1			shift d1
	roxl.l	#1,d0			shift into d0
	dbf	d2,left_loop		jump if not finished with shifting
*
	bra	finish			return
*
overflow:
	move.l	#$ffffffff,d0		load in largest value
	bra	finish			return
*
zero:	
	clr.l	d0			load in a zero
finish:
	cmp.l	#0,a0			was sign bit on
	beq	positive		no -- jump
*
	neg.l	d0			negative -- so negate the result
positive:
	move.l	(sp)+,d2		restore scratch register
	jmp	(a1)			return
 
#pragma endasm
 
#endif /* EXCLUDE_op_dtoul.c */
 

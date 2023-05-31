#include "lib_top.h"


#if EXCLUDE_op_ftoul
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
;	op_FtoUL.s
;
;	Converts float to unsigned long, and
;	return the result on top of the stack.
;	Readjust stack accordingly and returns to the calling program.
;	Supplies function __ftoul.
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
	xdef	__ftoul
*
__ftoul:
	movem.l	(sp)+,d0/d1		get return address && argument
	move.l	d0,a1			return address to a0
*
	move.l	d1,d0			move for processing
	andi.l	#$80000000,d0		getting the sign bit
	move.l	d0,a0			save for end
*
	andi.l	#$7fffffff,d1		zap sign bit if there
	move.l	d1,d0			put arg in d0 for later
*
	swap	d1
	lsr.w	#7,d1			do allignment
	subi.w	#$7f,d1			get rid of offset
*
	cmp.w	#-1,d1			do we have fraction ??
	ble	zero			yes
*
	andi.l	#$007fffff,d0		mask off exponent
	ori.l	#$00800000,d0		put back in invisible bit
*
	cmp.w	#23,d1			which direction to we shift
	bgt	left_entry		must shift left
*
*					handle right shifts !!!
	neg.w	d1			negate exponent
	addi.w	#23,d1			add in 20
	lsr.l	d1,d0			shift in one big swoop
	bra	finish			return
*
left_entry:
	cmp.w	#32,d1			will we have overflow ??
	bge	overflow		yes !!
*
	subi.w	#23,d1			pre-subtract
	lsl.l	d1,d0			shift d0 left
	bra	finish			return
*
overflow:
	move.l	#$ffffffff,d0		load in largest value
	bra	finish			return
*
zero:	
	clr.l	d0			load in a zero
finish:
	cmp.l	#0,a0			is negation flag on ??
	beq	return			no -- just return
	neg.l	d0			on -- so negate result
return:
	jmp	(a1)			return
 
#pragma endasm
 
#endif /* EXCLUDE_op_ftoul.c */
 

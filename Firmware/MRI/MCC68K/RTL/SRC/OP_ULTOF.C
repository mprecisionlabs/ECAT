#include "lib_top.h"


#if EXCLUDE_op_ultof
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
;	op_ULtoF.s
;
;	Converts unsigned long a to float, and
;	return the result on top of the stack.
;	Readjust stack accordingly and returns to the calling program.
;
;	HISTORY:
;	5/19/89 mjf: Created
;	6/16/89 jpc: Written
;	5/21/91 jpc: Corrected rounding problem
;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
*
* %W% %G% *
*

	opt case
	section code
	xdef	__ultof
	xdef	__fldul
__fldul:
	move.l	(a0),d0			get argument and place in correct reg.
*
__ultof:
	tst.l	d0			is input arg zero ??
	beq.b	input_zero		yes -- go home
*
	move.l	#$00800000,a0		load in comparison word
	move.l	#$01000000,a1		load in comparison word
*
	move.l	#($7f+23)*$800000,d1	initialize the exponent register
	cmp.l	a1,d0			do we shift right or left
	bcs.b	shift_left		must shift the number left
*
	movem.l	d2/d3,-(sp)		save scratch registers
	clr.b	d2			to be used as round bit
	clr.b	d3			to be used as guard bit
right_loop:
	or.b	d2,d3			adjust guard bit
	move.b	d0,d2			get next round bit
	lsr.l	#1,d0			shift word a bit
	add.l	a0,d1			bump exponent
	cmp.l	a1,d0			do we shift again
	bcc.b	right_loop		yes -- do again
*
	andi.l	#$007fffff,d0		zap invisible bit
	or.l	d1,d0			or in the exponent
*					now round up if necessary
	andi.b	#1,d2			is round bit on
	beq.b	no_round		off -- so do not round
	or.b	d0,d3			round on -- do we really bump ??
	andi.b	#1,d3			have computed guard || low bit
	beq.b	no_round		no
	addq.l	#1,d0			bump answer (actually round up)
*
no_round:
	movem.l	(sp)+,d2/d3		recover scratch registers
	bra.b	finish
*
left_loop:
	lsl.l	#1,d0			shift left one
	sub.l	a0,d1			subtract one from exponent
shift_left:
	cmp.l	a0,d0
	bcs.b	left_loop		finished shifting left
*
	andi.l	#$007fffff,d0		zap invisible bit
	or.l	d1,d0			or in the exponent
*
finish:
input_zero:
	move.l	(sp),a0			get return address
	move.l	d0,(sp)			put answer and the stack
	jmp	(a0)			and now return
 
#pragma endasm
 
#endif /* EXCLUDE_op_ultof.c */
 

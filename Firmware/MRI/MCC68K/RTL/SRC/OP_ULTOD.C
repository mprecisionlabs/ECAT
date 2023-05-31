#include "lib_top.h"


#if EXCLUDE_op_ultod
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
;	op_ULtoD.s
;
;	Converts unsigned long a to double, and
;	returns the result on top of the stack.
;	Readjust stack accordingly and returns to the calling program.
;
;	HISTORY:
;	5/19/89 mjf: Created
;	6/19/89 jpc: Written
;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
*
* %W% %G% *
*

	opt case
	section code
	xdef	__ultod
	xdef	__dldul
__dldul:
	move.l	(a0),d0			get argument
*
__ultod:
	clr.l	d1			pre-clear d1 for answer
	tst.l	d0			is input arg zero
	beq.b	zero_in			yes -- go home
*
	move.l	#$00100000,a0		load in comparison word
	move.l	#$00200000,a1		load in comparison word
	move.l	d2,-(sp)		save d2
*
	move.l	#($3ff+20)*$00100000,d2	initialize the exponent register
	cmp.l	a1,d0			do we shift right or left
	bcs.b	shift_left		must shift the number left
*
right_loop:
	lsr.l	#1,d0			shift word a bit
	roxr.l	#1,d1			shift in carried out bit
	add.l	a0,d2			bump exponent
	cmp.l	a1,d0			do we shift again
	bcc.b	right_loop		yes -- do again
	bra.b	finish
*
left_loop:
	lsl.l	#1,d0			shift left one
	sub.l	a0,d2			subtract one from exponent
shift_left:
	cmp.l	a0,d0
	bcs.b	left_loop		finished shifting left
*
finish:
	andi.l	#$000fffff,d0		zap invisible bit
	or.l	d2,d0			or in the exponent
	move.l	(sp)+,d2		restore d2
zero_in:
	move.l	(sp),a0			get return address
	move.l	d1,(sp)			put out lo order answer
	move.l	d0,-(sp)		put out hi order answer
	jmp	(a0)			and now return
 
#pragma endasm
 
#endif /* EXCLUDE_op_ultod.c */
 

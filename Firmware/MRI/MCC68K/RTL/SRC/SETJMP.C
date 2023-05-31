#include "lib_top.h"


#if EXCLUDE_setjmp
    #pragma option -Qws			/* Prevent compiler warning */
#else
 
_kill_warning();	/* Eliminates compiler warnings */

#if _OPTION_Kt
  #informing -- The "-Kt" option is disregarded by longjmp.
#endif

#if _OPTION_upd || _OPTION_us
  #error -- The "-upd" and "-us" options cannot be applied to this routine.
#endif

#if _OPTION_utn
  #error -- The "-ut<number>" option cannot be applied to this routine.
#endif

/*
;***********************************************************************;
;  THIS INFORMATION IS PROPRIETARY TO					;
;  MICROTEC RESEARCH, INC.						;
;-----------------------------------------------------------------------;
;  Copyright (c) 1990 Microtec Research, Inc.				;
;  All rights reserved							;
;***********************************************************************;
;The software source code contained in this file and in the related
;header files is the property of Microtec Research, Inc. and may only be
;used under the terms and conditions defined in the Microtec Research
;license agreement. You may modify this code as needed but you must retain
;the Microtec Research, Inc. copyright notices, including this statement.
;Some restrictions may apply when using this software with non-Microtec
;Research software.  In all cases, Microtec Research, Inc. reserves all rights.
;
;***********************************************************************;
;									;
;	setjmp								;
;		Store registers so they can be restored by longjmp	;
;		This particular flavor of setjmp does not save floating	;
;		point registers.					;
;	longjmp								;
;		Restore registers set up by previous setjmp call.	;
;		Return argument in D0.  Return to return location of	;
;		previous setjmp call.					;
;									;
;	Revision History						;
;	02/06/90 jpc:	Written						;
;***********************************************************************;
;	%G% %W%
;
*/

#include <setjmp.h>

#pragma option -nKf	/* LINK/UNLK will cause code to malfunction */

int  setjmp(jmp_buf input1)
{
    asm ("	move.l	4(sp),a0");	/* Get address of storage structure */
    asm ("	move.l	(sp),(a0)+");	/* Store the return address */
    asm ("	movem.l	d2-d7/a2-a7,(a0)");	/* Save the world */
    asm ("	clr.l	d0");			/* Clear for return */
}

#pragma	asm

	opt	case
	section	code
	xdef	_longjmp
_longjmp:
	move.l	8(sp),d0		Get return value and place correctly
	bne.b	not_zero		Make sure arg is not zero
	addq.b	#1,d0			Make 1 if arg was zero
not_zero:
	move.l	4(sp),a0		Get address of structure
	move.l	(a0)+,a1		Get old return address
	movem.l	(a0),d2-d7/a2-a7	Restore regs
	addq.l	#4,a7			Pop "return" off stack
	jmp	(a1)			And return
last:
	DEBUG   68,'longjmp',_longjmp,last-_longjmp,exit-_longjmp
 
#pragma endasm
 
#endif /* EXCLUDE_setjmp.c */
 

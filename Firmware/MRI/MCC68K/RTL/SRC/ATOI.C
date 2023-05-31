#include "lib_top.h"


#if EXCLUDE_atoi
    #pragma option -Qws			/* Prevent compiler warning */
#else

_kill_warning();	/* Eliminates compiler warnings */
 
#pragma asm
 
#if _OPTION_Kt
  #informing -- The "-Kt" option is disregarded by this routine.
#endif

#if _OPTION_upd || _OPTION_us
  #error -- The "-upd" and "-us" options cannot be applied to this routine.
  #error -- Use the atoi.c and atol.c routines in the "vanilla" directory
#endif

#if _OPTION_utn
  #error -- The "-ut<number>" option cannot be applied to this routine.
  #error -- Use the atoi.c and atol.c routines in the "vanilla" directory
#endif

*************************************************************************/
** THIS INFORMATION IS PROPRIETARY TO					*/
** MICROTEC RESEARCH, INC.						*/
**----------------------------------------------------------------------*/
** Copyright (c) 1991 Microtec Research, Inc.				*/
** All rights reserved							*/
*************************************************************************/
*The software source code contained in this file and in the related
*header files is the property of Microtec Research, Inc. and may only be
*used under the terms and conditions defined in the Microtec Research
*license agreement. You may modify this code as needed but you must retain
*the Microtec Research, Inc. copyright notices, including this statement.
*Some restrictions may apply when using this software with non-Microtec
*Research software.  In all cases, Microtec Research, Inc. reserves all rights.

	TTL			'atoi'
*
*	File name:		atoi.src
*	Author:			Herbert Yuen
*	Date:			85-May-22 Wednesday
*
* %W% %G% *
*
*==============================================================================
*
*			a t o i ,  a t o l
*
*==============================================================================
*  int	atoi(Str)
*  long	atol(Str)
*
*	Convert the 0 terminated string Str in ASCII to integer (long).  The
*	first unrecognized character ends the string.
*
	SECTION	code
	XDEF	_atoi
	XDEF	_atol
*
_atoi:
_atol:
	move.l	4(a7),a0		; Address of String
	move.l	d2,-(sp)		; Save D2

	moveq	#0,d0			; Initialize sum to 0 in D0
	moveq	#0,d1
	moveq	#0,d2			; Assume positive number
	move.l	d0,a1
white:
	move.b	(a0)+,d1		; Get one char into D1
	cmpi.b	#' ',d1			; Skip leading "white spaces"
	beq	white
	cmpi.b	#13,d1			; White space include HT,LF,VT,FF,CR
	bgt.s	sign
	cmpi.b	#9,d1			; Is it >= tab ?
	bge	white
sign:
	cmpi.b	#'+',d1			; Is there a sign ?
	beq.s	nextc			; Skip to the next char
	cmpi.b	#'-',d1			; Is it negative ?
	bne.s	digit			;   No, start to check for digits
	moveq	#1,d2			; Remember the number is negative
nextc:
	move.b	(a0)+,d1		; Get next char
digit:					; Find the first non-zero digit
	subi.b	#'0',d1			; Integer value of this digit
	beq	nextc			; Skip leading zeros
	bcs.s	return			; Value is 0, return
	cmpi.b	#9,d1			; Test for valid digits
	bgt.s	return			; Value is 0, return
loop:
	add.l	d1,d0			; Add value of this digit to sum
	move.b	(a0)+,d1		; Next char
	subi.b	#'0',d1			; Integer value of this digit
	bcs.s	done			; No more digits
	cmpi.b	#9,d1			; Test for valid digits
	bgt.s	done
			; multiply value in D0 by 10 using shifts and add
	add.l	d0,d0			; Sum * 2
	move.l	d0,a1			; Save a copy in A1
	lsl.l	#2,d0			; Sum * 8
	add.l	a1,d0			; Sum * 10
	bra	loop
done:
	tst.b	d2			; Is the number negative ?
	beq.s	return
	neg.l	d0
return:
	move.l	(sp)+,d2		; Restore D2
	rts				; Return value in D0
*
 
#pragma endasm
 
#endif /* EXCLUDE_atoi.c */
 

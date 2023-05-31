#include "lib_top.h"


#if EXCLUDE_xitoa
    #pragma option -Qws			/* Prevent compiler warning */
#else
 
_kill_warning();	/* Eliminates compiler warnings */

#if _OPTION_Kt
  #informing -- The "-Kt" option is disregarded by this routine.
#endif

#if _OPTION_upd || _OPTION_us
  #error -- The "-upd" and "-us" options cannot be applied to this routine.
  #error -- Use itoa.c, ltoa.c, and xltoa.c from the "vanilla" directory.
#endif

#if _OPTION_utn
  #error -- The "-ut<number>" option cannot be applied to this routine.
  #error -- Use itoa.c, ltoa.c, and xltoa.c from the "vanilla" directory.
#endif

#pragma asm
 
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

	TTL			'xitoa'
*
*	File name:		itoa.a86
*	Author:			Herbert Yuen
*	Date:			85-Jun-07 Friday
*
* %W% %G% *
*
*==============================================================================
*
*			i t o a ,  l t o a
*
*==============================================================================
*  int	itoa (N, Str)   int  N; char *Str;
*  int	ltoa (N, Str)	long N; char *Str;
*
*	Convert the (signed) integer N to a null-terminated ASCII string
*	of decimal digits and place in the area pointed to by Str.  If N is
*	negative, the output string begins with a '-'.  No leading blanks or
*	zeros are produced.  The function returns the number of characters
*	placed in the output string, excluding the null terminator.
*
	SECTION	code
	XDEF	__itoa
	XDEF	__ltoa
*
__itoa:
__ltoa:
	move.l	d2,-(sp)		; Save D2
	move.l	12(a7),a0		; A0 -> Str
	move.l	8(a7),d0		; N
	bpl.s	plus			; OK if N is positive
	neg.l	d0			; N is negative: make it positive
	move.b	#'-',(a0)+		; Output a '-' sign
plus:
	move.b	#0,-(sp)		; Null-terminator for output string
mainlp:					; The main loop
	cmpi.l	#65535,d0		; Is hi word of number equal 0 ?
	bhi.s	not0
	moveq	#10,d1
div16:					;   yes, we can do 16-bit div
	cmp.l	d1,d0			; Is number < 10 ?
	bcs.s	done			;   if so, we are done
	divu	d1,d0			; Divide number by 10
	swap	d0			; Quotient in hi, remainder in lo
	addi.b	#'0',d0			; Convert from binary to ASCII
	move.b	d0,-(sp)		; Save the remainder on stack
	andi.b	#0,d0			; Clear low order byte
	swap	d0			; Quotient back to low order word
	bra	div16
not0:					; Hi word != 0, prepare 32-bit div
	moveq	#0,d1			; Initialize remainer to 0
	moveq	#31,d2
div32:					; 32/16-bit division
	lsl.l	#1,d0
	roxl.l	#1,d1			; 64-bit left shift
	cmpi.b	#10,d1			; Remainder > divisor ?
	bcs.s	continue
	subi.b	#10,d1			;   if so, subtract it
	addq.b	#1,d0			; add 1 to quotient
continue:
	dbf	d2,div32
	addi.b	#'0',d1			; Convert from binary to ASCII
	move.b	d1,-(sp)		; Save the remainder on stack
	bra	mainlp
done:					; Last digit not pushed on stack
	addi.b	#'0',d0			; Convert from binary to ASCII
	move.b	d0,(a0)+
rev:					; Loop for reversing the digits
	move.b	(sp)+,(a0)+		; Copy output from stack to Str
	bne	rev
	move.l	a0,d0			; Ptr to Str after the null char
	subq.l	#1,d0
	sub.l	12(a7),d0		; Number of char output, excluding 0
	move.l	(sp)+,d2		; Restore D2
	rts
*
 
#pragma endasm
 
#endif /* EXCLUDE_xitoa.c */
 

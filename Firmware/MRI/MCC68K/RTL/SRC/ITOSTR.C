#include "lib_top.h"


#if EXCLUDE_itostr
    #pragma option -Qws			/* Prevent compiler warning */
#else
 
_kill_warning();	/* Eliminates compiler warnings */

#if _OPTION_Kt
  #informing -- The "-Kt" option is disregarded by this routine.
#endif

#if _OPTION_upd || _OPTION_us
  #error -- The "-upd" and "-us" options cannot be applied to this routine.
  #error -- Use itostr.c, ltostr.c, and xltostr.c from the "vanilla" directory.
#endif

#if _OPTION_utn
  #error -- The "-ut<number>" option cannot be applied to this routine.
  #error -- Use itostr.c, ltostr.c, and xltostr.c from the "vanilla" directory.
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

	TTL			'itostr'
*
*	File name:		itostr.a86
*	Author:			Herbert Yuen
*	Date:			85-Jun-10 Monday
*
* %W% %G% *
*
*==============================================================================
*
*			i t o s t r ,  l t o s t r
*
*==============================================================================
*  int	itostr (N, Str, Base)   unsigned  N; char *Str; int Base;
*  int	ltostr (N, Str, Base)	unsigned long N; char *Str; int Base;
*
*	Convert the unsigned (long) integer N to a null-terminated ASCII string
*	of number in base Base and place the output in the area pointed
*	to by Str.  The specified base must be between 2 and 36, otherwise,
*	base 10 is assumed.  No leading blanks or zeros are produced.
*	In addition to numeric digits the letters 'A' through 'Z' are used
*	for numbers of base greater than 10.
*
*	The function returns the number of characters placed in the
*	output string, excluding the null terminator.
*
	SECTION	code
	XDEF	_itostr
	XDEF	_ltostr
*
_itostr:
_ltostr:
	move.l	4(a7),d0		; N
	move.l	8(a7),a0		; A0 -> Str
	move.l	12(a7),d1		; Base
	movem.l	d2/d3,-(sp)		; Save D2, D3
*
	move.b	#0,-(sp)		; Null-terminator for output string
	cmpi.l	#2,d1			; Is Base < 2 ?
	bcs.s	base10			;   if so, change to 10
	cmpi.l	#36,d1			; Is Base > 36 ?
	bls.s	mainlp
base10:
	moveq	#10,d1			; Default base
*
mainlp:					; The main loop
	cmpi.l	#65535,d0		; Is hi word of number equal 0 ?
	bhi.s	not0
div16:					;   yes, we can do 16-bit div
	cmp.l	d1,d0			; Is number < Base ?
	bcs.s	done			;   if so, we are done
	divu	d1,d0			; Divide number by Base
	swap	d0			; Quotient in hi, remainder in lo
	addi.b	#'0',d0			; Convert from binary to ASCII
	move.b	d0,-(sp)		; Save the remainder on stack
	andi.b	#0,d0			; Clear low order byte
	swap	d0			; Quotient back to low order word
	bra	div16
not0:					; Hi word != 0, prepare 32-bit div
	moveq	#0,d3			; Initialize remainer to 0
	moveq	#31,d2
div32:					; 32/16-bit division
	lsl.l	#1,d0
	roxl.l	#1,d3			; 64-bit left shift
	cmp.b	d1,d3			; Remainder > divisor ?
	bcs.s	continue
	sub.b	d1,d3			;   if so, subtract it
	addq.b	#1,d0			; add 1 to quotient
continue:
	dbf	d2,div32
	addi.b	#'0',d3			; Convert from binary to ASCII
	move.b	d3,-(sp)		; Save the remainder on stack
	bra	mainlp
done:					; Last digit
	addi.b	#'0',d0			; Convert from binary to ASCII
	move.b	d0,-(sp)
rev:					; Loop for reversing the digits
	move.b	(sp)+,d0		; Get one digit from stack
	cmpi.b	#'9',d0			; Is digit > '9' ?
	bls.s	store
	addq.b	#7,d0			; Use 'A' thru 'Z' for bases > 10
store:
	move.b	d0,(a0)+		; Output 1 digit to Str
	bne	rev
	movem.l	(sp)+,d2/d3		; Restore D2, D3
	move.l	a0,d0			; Ptr to Str after the null char
	subq.l	#1,d0
	sub.l	8(a7),d0		; Number of char output, excluding 0
	rts
*
 
#pragma endasm
 
#endif /* EXCLUDE_itostr.c */
 

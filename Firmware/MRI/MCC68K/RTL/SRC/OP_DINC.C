#include "lib_top.h"


#if EXCLUDE_op_dinc
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

/************************************************************************/
/* THIS INFORMATION IS PROPRIETARY TO					*/
/* MICROTEC RESEARCH, INC.						*/
/*----------------------------------------------------------------------*/
/* Copyright (c) 1991, 1992 Microtec Research, Inc.                     */
/* All rights reserved							*/
/************************************************************************/
/*
;The software source code contained in this file and in the related
;header files is the property of Microtec Research, Inc. and may only be
;used under the terms and conditions defined in the Microtec Research
;license agreement. You may modify this code as needed but you must retain
;the Microtec Research, Inc. copyright notices, including this statement.
;Some restrictions may apply when using this software with non-Microtec
;Research software.  In all cases, Microtec Research, Inc. reserves all rights.
*/


#include <string.h>
#include "asm_addr.h"

/*
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;
;	op_Dinc.s
;
;	tos + 1 -> tos  (double precision inc)
;	Readjust stack accordingly and return to the calling program.
;	Supplies function  __dinc()  that is used by the BE.
;	Calls faddi
;
;	HISTORY:
;	5/25/89 mjf: Created
;	6/9/89	jpc: Eliminated use of D3 -- just uses stack now
;	7/24/89 jpc: Eliminated use of Ax registers
;	9/12/90 jpc: allow used of long disp. in pc rel. addr.
;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
*
* %W% %G% *
*
*/

#pragma	asm

	OPT CASE
	SECTION code
	XREF	faddi
	XDEF	__dinc_e2
	XDEF	__dinc
__dinc:
	move.l	#$3FF00000,d0	double prec 1.0
__dinc_e2:
	move.l	#0,d1		rest of it
	move.l	8(sp),-(sp)	lo order arg to top of stack
	move.l  8(sp),-(sp)	hi order arg to top of stack
	jsr	C_ADDR(faddi)	double ADD routine (arg 2 on tos)
	move.l	d1,8(sp)	answer (low)
	move.l	d0,4(sp)	answer (high)
	rts			and go home

#pragma	endasm
 
#endif /* EXCLUDE_op_dinc */
 

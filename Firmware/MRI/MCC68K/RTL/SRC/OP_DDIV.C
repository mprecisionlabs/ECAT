#include "lib_top.h"


#if EXCLUDE_op_ddiv
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
;	op_Ddiv.s
;
;	ST(1) / tos -> tos  (double precision divide)
;	Readjust stack accordingly and return to the calling program.
;	Supplies function  __ddiv()  that is used by the BE.
;	Calls fdivi
;
;	HISTORY:
;	5/25/89 mjf: Created
;	6/12/89 jpc: Eliminated use of Ax registers
;	9/12/90 jpc: allow used of long disp. in pc rel. addr.
;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
*
* %W% %G% *
*
*/

#pragma	asm

	opt	case
	section	code
	xref	fdivi
	xdef	__ddiv
__ddiv:
	move.l	12(sp),d0	; most significant 32 bits
	move.l	16(sp),d1	; least
	move.l	(sp)+,8(sp)	; pop and save my return address
	jsr	C_ADDR(fdivi)	; double div routine (arg 2 on tos)
				; deallocates 8 bytes (arg 2)
	move.l	(sp),-(sp)	; my return address
	move.l	d0,4(sp)
	move.l	d1,8(sp)
	rts

#pragma	endasm
 
#endif /* EXCLUDE_op_ddiv */
 

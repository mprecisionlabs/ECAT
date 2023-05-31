#include "lib_top.h"


#if EXCLUDE_op_fmul
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
;	op_Fmul.s
;
;	ST(1) * tos -> tos  (single precision mul)
;	Readjust stack accordingly and return to the calling program.
;	Supplies function  __fmul()  that is used by the BE.
;	Calls fmuli
;
;	HISTORY:
;	5/25/89 mjf: Created
;	6/12/89	jpc: Eliminated use of Ax registers
;	9/12/90 jpc: allow use of long disp. in pc rel. addr.
;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
*
* %W% %G% *
*
*/

#pragma	asm

	opt	case
	section	code
	xref	fsmuli
	xdef	__fmul
__fmul:
	move.l	8(sp),D0	; arg 1
	move.l	4(sp),D1	; arg 2
	move.l	(sp)+,(sp)	; my return
*
	jsr	C_ADDR(fsmuli)	; D0 op D1 -> D0
*
	move.l	d0,4(sp)	; store result
	rts

#pragma	endasm
 
#endif /* EXCLUDE_op_fmul */
 

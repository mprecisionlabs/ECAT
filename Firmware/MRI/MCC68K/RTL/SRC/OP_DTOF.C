#include "lib_top.h"


#if EXCLUDE_op_dtof
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
;	op_DtoF.s
;
;	(double)tos -> (float) tos  (convert double to float)
;	Readjust stack accordingly and return to the calling program.
;	Supplies function  __dtof()  that is used by the BE.
;	Calls fsinglei
;
;	HISTORY:
;	5/25/89 mjf: Created
;	9/12/90 jpc: allow use of long disp. in pc rel. addr.
;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
*
* %W% %G% *
*
*/

#pragma	asm

	OPT CASE
	SECTION code
	XREF	fsinglei
	XDEF	__dtof
__dtof:
	move.l	4(SP),D0	; double, most significant
	move.l	8(SP),D1	; least significant
	jsr	C_ADDR(fsinglei);convert D0:D1 (double) to D0 (float)
	movea.l	(SP)+,A0	; my return address
	move.l	(SP)+,D1	; deallocate
	move.l	D0,(SP)
	jmp	(A0)

#pragma	endasm
 
#endif /* EXCLUDE_op_dtof */
 

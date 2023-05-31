#include "lib_top.h"


#if EXCLUDE_op_ltof
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
;	op_LtoF.s
;
;	(long)D0 -> (float) tos  (convert long to float)
;	(long)(A0) -> (float) tos  (convert long to float)
;	Readjust stack accordingly and return to the calling program.
;	Supplies functions __fldl and __ltof()  used by the BE.
;	Calls ffltis
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
	XREF	ffltis
	XDEF	__fldl
	XDEF	__ltof
__fldl:
	move.l	(A0),D0		; long
__ltof:
	jsr	C_ADDR(ffltis)	; convert D0 (long) to D0 (float)
	movea.l	(SP),A0		; my return address
	move.l	D0,(SP)
	jmp	(A0)

#pragma	endasm
 
#endif /* EXCLUDE_op_ltof */
 

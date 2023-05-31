#include "lib_top.h"


#if EXCLUDE_op_dtol
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
;	op_DtoL.s
;
;	(double)tos -> (long) D0 (convert double to long)
;	Readjust stack accordingly and return to the calling program.
;	Supplies function __dtol()  used by the BE.
;	Calls ffixi
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
	XREF	ffixi
	XDEF	__dtol
;
__dtol:
	move.l	4(SP),D0	; high
	move.l	8(SP),D1	; low
	jsr	C_ADDR(ffixi)	; convert D0:D1 (double) to D0 (long)
	movea.l	(SP)+,A0	; my return address
	addq.l	#8,SP		; deallocate
	jmp	(a0)		; now go home -- jpc

#pragma	endasm
 
#endif /* EXCLUDE_op_dtol */
 

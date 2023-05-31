#include "lib_top.h"


#if EXCLUDE_op_dcmp0
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
;	op_Dcmp0.s
;
;	tos - 0.0 -> cc  (double precision compare with 0.0)
;	Readjust stack accordingly and return to the calling program.
;	Supplies function  __dcmp0()  that is used by the BE.
;	Calls fcmpi
;
;	HISTORY:
;	5/25/89 mjf: Created
;	6/8/89  jpc: Removed pop of extra stuff left on stack by "fcmpi"
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
	XREF	fcmpi
	XDEF	__dcmp0
__dcmp0:
	move.l	4(SP),D0	; arg 1, most significant 32 bits
	move.l	8(SP),D1	; least
	move.l	(SP)+,4(SP)	; save return address over arg 1
	clr.l	(SP)		; overwrite part of arg 1 with 0.0
	clr.l	-(SP)		; push rest of 0.0 (arg 2)
	jsr	C_ADDR(fcmpi)	; double cmp routine
	rts

#pragma	endasm
 
#endif /* EXCLUDE_op_dcmp0 */
 

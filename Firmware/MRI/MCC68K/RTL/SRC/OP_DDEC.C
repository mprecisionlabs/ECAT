#include "lib_top.h"


#if EXCLUDE_op_ddec
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
;	op_Ddec.s
;
;	tos - 1 -> tos  (double precision dec)
;	Readjust stack accordingly and return to the calling program.
;	Supplies function  __ddec()  that is used by the BE.
;	Jumps to __dinc_e2
;
;	HISTORY:
;	5/25/89 mjf: Created
;	6/9/89	jpc: Now we just jump to __dinc_e2 (entry 2)
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
	
	XDEF	__ddec
	XREF	__dinc_e2
__ddec:
	move.l	#$BFF00000,d0		; double prec -1.0
	jmp	C_ADDR(__dinc_e2)	; finish out as though a dinc 

#pragma	endasm
 
#endif /* EXCLUDE_op_ddec */
 

#include "lib_top.h"


#if EXCLUDE_op_dswap
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

#pragma asm
 
;************************************************************************/
;* THIS INFORMATION IS PROPRIETARY TO					*/
;* MICROTEC RESEARCH, INC.						*/
;*----------------------------------------------------------------------*/
;* Copyright (c) 1991 Microtec Research, Inc.				*/
;* All rights reserved							*/
;************************************************************************/
;The software source code contained in this file and in the related
;header files is the property of Microtec Research, Inc. and may only be
;used under the terms and conditions defined in the Microtec Research
;license agreement. You may modify this code as needed but you must retain
;the Microtec Research, Inc. copyright notices, including this statement.
;Some restrictions may apply when using this software with non-Microtec
;Research software.  In all cases, Microtec Research, Inc. reserves all rights.
;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;
;	op_Dswap.s
;
;	Swaps two 64 bit doubles on stack.
;
;	HISTORY:
;	5/19/89 mjf: Created
;	6/12/89 jpc: Actually implemented
;	7/5/89	jpc: Use of A0 removed
;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
*
* %W% %G% *
*

	opt case
	section code
	xdef	__dswap

__dswap:
	movem.l	4(sp),d0/d1		save b
*
	move.l	12(sp),4(sp)		copy lo word of a to b
	move.l	16(sp),8(sp)		copy hi word of a to b
*
	movem.l	d0/d1,12(sp)		copy b to a
	rts				return
 
#pragma endasm
 
#endif /* EXCLUDE_op_dswap.c */
 

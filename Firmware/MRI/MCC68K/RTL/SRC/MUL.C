#include "lib_top.h"


#if EXCLUDE_mul
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
; COPYRIGHT 1981, 1982, RICHARD E. JAMES III
; 9/12/90 jpc: allow use of long disp. in pc rel. addr.
; THIS MODULE PERFORMS SINGLE PRECISION MULTIPLY FOR THE M68000.
*
* %W% %G% *
*

; revision history for MUL:
; 28-Aug-86 fhw: change FSMULI to fsmuli
; 12-Aug-86 kjk: changed comments to reflect Fu's change to interface
; 16-May-86 ling: using pc relative 

; ENTRY CONDITIONS
;	OPERAND 1 IN D0
;	OPERAND 2 IN D1
; EXIT CONDITIONS
;	PRODUCT IN D0

; INTERNAL REGISTER CONVENTIONS:
;       D0      OPND1/UPPER1
;       D1      OPND2/UPPER2
;       D2      9/LOWER1
;       D3      LOWER2
;       D4      EXP 
;       D5      SIGN 
*/
 
#pragma	asm

        section code
        XDEF  fsmuli

NSAVED  EQU     6*4

        XREF  F_RCP
fsmuli
;------
; LOAD RETURN ADDR AND OPNDS
        MOVEM.L D0-D5,-(SP)    ;SAVE
; (END OF INTERFACE)
;       D0,D1:  OPERANDS
;
; SAVE SIGN OF RESULT
        MOVE.L  D0,D5
        EOR.L   D1,D5
        ASL.L   #1,D0          ;TOSS SIGN
        ASL.L   #1,D1          ;EEMMMMM0
        CMP.L   D1,D0
; ORDER OPERANDS
        BLS.S   ESWAP
        EXG     D0,D1          ;D1=LARGER
; EXTRACT AND CHECK EXPONENTS
ESWAP   ROL.L   #8,D0
        ROL.L   #8,D1          ;MMMMM0EE
        CLR.W   D4
        MOVE.B  D0,D4
        CLR.W   D3
        MOVE.B  D1,D3
        ADD.W   D3,D4          ;RSLT EXP
        CMP.B   #$FF,D1
        BEQ.S   OFL            ;INF OR NAN
        TST.B   D0
        BEQ.S   UFL            ;0 OR GU (DENORMALIZED)
; CLEAR EXP; SET HIDDEN BIT
        MOVE.B  #1,D0
        ROR.L   #1,D0
BACK    MOVE.B  #1,D1
        ROR.L   #1,D1
; SPLIT MANTISSAS INTO 2 PIECES
        MOVE.W  D0,D2          ;LOWER
        MOVE.W  D1,D3
        SWAP    D0             ;UPPER
        SWAP    D1
; MULTIPLY THE PIECES
        MULU    D0,D3          ;U1*L2
        MULU    D1,D2          ;U2*L1
        MULU    D1,D0          ;U2*U1
; ADD TOGETHER
        ADD.L   D2,D3          ;MIDDLE PRODS
        ADDX.B  D3,D3          ;CARRY TO BIT 0
        ANDI.W  #1,D3          ;TOSS REST
        SWAP    D3             ;QUICK ROT 16
        ADD.L   D3,D0          ;(CANT OFL)
        SUBI.W  #126,D4        ;TOSS XTRA BIAS
        JSR     C_ADDR(F_RCP)	;ling: using pc relative
                               ;      RND,CK,PACK

; BUILD ANSWER
MBUILD  ROR.L   #8,D0
        ROXL.L  #1,D5
        ROXR.L  #1,D0          ;APPEND SIGN

;       D0 = ANSWER
; (EXIT INTERFACE:)
MEXIT   ADDQ.W  #4,SP
        MOVEM.L (SP)+,D1-D5    ;RESTORE REGISTERS
        RTS

; EXCEPTION HANDLING
 
OFL     CLR.B   D1
        TST.L   D1             ;LARGER MANTISSA
        BNE.S   MNI            ;USE LARGER NAN
        TST.L   D0             ;SMALLER VALUE
        BEQ.S   GENNAN         ;0*INF
MNI     MOVE.B  #$FF,D1        ;INF OR NAN
        MOVE.L  D1,D0
        BRA.S   MBUILD

UFL     TST.L   D0             ;MANTISSA OF SMALLER
        BEQ.S   MBUILD         ;0*
; NORMALIZING MODE IS EMBODIED IN THE NEXT FEW LINES:      ;NORM
        BMI.S   BACK                                       ;NORM
NORMDEN SUBQ.W  #1,D4          ;ADJ EXP                    ;NORM
        LSL.L   #1,D0          ;NORMALIZE DENORMAL NUMBER  ;NORM
        BPL.S   NORMDEN                                    ;NORM
        BRA.S   BACK                                       ;NORM

GENNAN  MOVE.L  #$7F800002,D0  ;NAN 2
        BRA.S   MEXIT
SIGNED0 CLR.L   D0 
        BRA.S   MBUILD

        END

#pragma	endasm
 
#endif /* EXCLUDE_mul */
 

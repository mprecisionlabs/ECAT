#include "lib_top.h"


#if EXCLUDE_add
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
 
; COPYRIGHT 1981, RICHARD E. JAMES III
*************************************************************************/
** THIS INFORMATION IS PROPRIETARY TO					*/
** MICROTEC RESEARCH, INC.						*/
**----------------------------------------------------------------------*/
** Copyright (c) 1991 Microtec Research, Inc.				*/
** All rights reserved							*/
*************************************************************************/
;
;The software source code contained in this file and in the related
;header files is the property of Microtec Research, Inc. and may only be
;used under the terms and conditions defined in the Microtec Research
;license agreement. You may modify this code as needed but you must retain
;the Microtec Research, Inc. copyright notices, including this statement.
;Some restrictions may apply when using this software with non-Microtec
;Research software.  In all cases, Microtec Research, Inc. reserves all rights.
;
; THIS MODULE HANDLES SINGLE PRECISION ADD AND SUBTRACT FOR THE M68000.
;
; revision history for ADD:
; 28-Aug-86 fhw: change upper case to lower case
; 12-Aug-86 kjk: changed comments to reflect Fu's change to interface
; 9/12/90 jpc: allow use of long disp. in pc rel. addr.
*
* %W% %G% *
*

        section code
        XDEF  fsaddi
        XDEF  fssubi

;   ENTRY CONDITION:
;	OPERAND 1 IN D0
;	OPERAND 2 IN D1
;   EXIT CONDITION: 
;       RESULT IN D0
;
; INTERNAL REGISTER CONVENTIONS.
;       D0      OPND1    
;       D1      OPND2/SUM
;       D2      SIGN OF OPND1
;       D3      SIGN OF OPND2
;       D4      EXP OF OPND1
;       D5      EXP OF OPND2
;       D6      DIFF OF EXPS
;       D7      RESERVED FOR STICKY

NSAVED  EQU     7*4            ;7 REGS SAVED

fssubi
;------
        BCHG    #31,D1
fsaddi
;------
; SAVE REGS AND LOAD OPNDS
        MOVEM.L D0-D6,-(SP) ;SAVE    
; (END OF INTERFACE)
;       D0 = SECOND OPERAND (WITH SIGN FLIPPED FOR SUB)
;       D1 = FIRST OPERAND
; EXTRACT SIGNS
        ASL.L   #1,D0          ;SIGN -> C
        SCS     D2
        ASL.L   #1,D1          ;SIGN -> C
        SCS     D3
; REORDER
        CMP.L   D1,D0
        BLS.S   ADD2           ;GET LARGER
        EXG     D0,D1
        EXG     D2,D3
; EXTRACT EXPONENTS
ADD2    ROL.L   #8,D1
        ROL.L   #8,D0
        CLR.L   D5
        MOVE.B  D1,D5          ;LARGER EXP
        CLR.W   D4
        MOVE.B  D0,D4          ;SMALLER EXP
        BNE.S   NUNFL          ;NOT 0 OR DENORMALIZED
; SMALLER IS 0 OR DENORMALIZED     
        TST.L   D0             ;IF SMALLER=0
        BEQ.S   USEL           ;USE LARGER (SIGN OF 0-0 UNPREDICTABLE)
        TST.B   D5             ;LARGER EXP
        BNE.S   GU1            ;NOT GU
; BOTH ARE DENORMALIZED
        CMP.B   D3,D2          ;COMPARE SIGNS
        BNE.S   GUSUB
        ADD.L   D0,D1          ;ADD
        ADDX.B  D1,D1          ;INCR EXP TO 1 IF OVERFLOW
        BRA.S   ASBUILD
GUSUB   SUB.L   D0,D1          ;SUBTRACT
        BRA.S   ASBUILD
; NEITHER IS DENORMALIZED
NUNFL   MOVE.B  #1,D0          ;CLR EXP AND
        ROR.L   #1,D0
GU1     MOVE.B  #1,D1          ;ADD HIDDEN BIT
        ROR.L   #1,D1
GU2     CMPI.B  #$FF,D5
        BEQ.S   OVFL           ;INF/NAN
; ALIGN SMALLER
        MOVE.W  D5,D6
        SUB.W   D4,D6          ;DIFFERENCE OF EXPS
SHL     CMPI.W  #8,D6
        BLT.S   SHX            ;EXIT WHEN <8 *S
        SUBQ.W  #8,D6
        LSR.L   #8,D0          ;ALIGN
        BNE.S   SHL            ;LOOP
SHX     LSR.L   D6,D0          ;FINISH ALIGN
        CMP.B   D3,D2          ;CMP SIGNS
        BNE.S   DIFF           ;DECIDE WHETHER TO ADD OR SUBTRACT

; ADD THEM
        ADD.L   D0,D1          ;SUM
        BCC.S   ENDAS          ;NO C, OK
        ROXR.L  #1,D1          ;PULL IN OVERFLOW *S
        ADDQ.W  #1,D5
        CMP.W   #$FF,D5
        BLT.S   ENDAS          ;NO OFL
        BRA.S   GENINF

; SUBTRACT THEM
DIFF    SUB.L   D0,D1
        BMI.S   ENDAS          ;IF NORMORMALIZED
        BEQ.S   CANCEL         ;RESULT=0
NORM    ASL.L   #1,D1          ;NORMALIZE
        DBMI    D5,NORM        ;DEC EXP
        SUBQ.W  #1,D5
        BGT.S   ENDAS          ;NOT GU
        BEQ.S   NORM2
        CLR.W   D5
        LSR.L   #1,D1          ;GRAD UND
NORM2   LSR.L   #1,D1

ENDAS
; ROUND (NOT FULLY STANDARD)
        ADDI.L  #$80,D1        ;ROUND
        BCC.S   ROFL           ;ROUND DID NOT CAUSE MANTISSA TO OFL
        ROXR.L  #1,D1
        ADDQ.W  #1,D5
        CMPI.W  #$FF,D5
        BEQ.S   GENINF         ;ROUND CAUSED EXP TO OVERFLOW
; REBUILD ANSWER
ROFL    LSL.L   #1,D1          ;TOSS HIDDEN
USEL    MOVE.B  D5,D1          ;INSERT EXP
ASBUILD ROR.L   #8,D1
        ROXR.B  #1,D3
        ROXR.L  #1,D1          ;APPLY SIGN
;       D1 = ANSWER
; (EXIT INTERFACE:)
ASEXIT  MOVE.L  D1,D0
        ADDQ.W  #4,SP
        MOVEM.L (SP)+,D1-D6    ;RESTORE REGISTERS
        RTS

; EXCEPTION CASES

; LARGER EXPONENT = 255:
OVFL    LSL.L   #1,D1
        TST.L   D1             ;LARGER MANT
        BNE.S   USEL           ;LARGER=NAN
        CMP.B   D4,D5          ;EXPS
        BNE.S   USEL           ;LARGER=INF
; AFFINE MODE ASSUMED IN THIS IMPLEMENTATION
        CMP.B   D3,D2          ;SIGNS
        BEQ.S   USEL           ;INF+INF=INF
GENNAN  MOVE.L  #$7F800001,D1     
        BRA.S   ASEXIT 
 
; COMPLETE CANCELLATION
CANCEL  CLR.L   D1             ;RESULT=0
; (NEED MINUS 0 FOR (-0)+(-0), ROUND TO -INF)
        BRA.S   ASEXIT
; RESULT OVERFLOWS:
GENINF  MOVE.L  #$FF,D1        ;MAX EXP
        BRA.S   ASBUILD


 
#pragma endasm
 
#endif /* EXCLUDE_add */
 

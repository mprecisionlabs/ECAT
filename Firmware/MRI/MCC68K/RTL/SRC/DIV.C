#include "lib_top.h"


#if EXCLUDE_div
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
; THIS ROUTINE PERFORMS A SINGLE PRECISION DIVIDE.

*
* %W% %G% *
*

; revision history for DIV:
; 28-Aug-86 fhw: change FSDIVI to fsdivi
; 12-Aug-86 kjk: changed comments to reflect Fu's change to interface
; 16-May-86 ling: using pc relative
; 9/12/90 jpc: allow use of long disp. in pc rel. addr.

*/

#pragma	asm
        section code
        XDEF  fsdivi

; ENTRY CONDITIONS:
;	OPERAND 1 IN D0
;	OPERAND 2 IN D1
; EXIT CONDITIONS
;	QUOTIENT IN D0

; INTERNAL REGISTER CONVENTIONS
;       D0      TOP; AB; RQ
;       D1      BOT; C
;       D2           Q 
;       D3      BOTTOM EXP; D
;       D4      TOP/FINAL EXP
;       D5      SIGN

NSAVED  EQU     6*4

        XREF  F_RCP
fsdivi
;------
        MOVEM.L D0-D5,-(SP)    ;SAVE REGISTERS 
; (END OF INTERFACE)
;       D0 = DIVIDEND (TOP)
;       D1 = DIVISOR (BOTTOM)

; DETERMINE SIGN
        ROL.L   #1,D0
        ROL.L   #1,D1
        MOVE.B  D0,D5
        EOR.B   D1,D5          ;SIGN IN BIT 0
; SPLIT OUT EXPONENT
        ROL.L   #8,D0
        ROL.L   #8,D1
        CLR.W   D3
        CLR.W   D4
        MOVE.B  D0,D4          ;EXP OF TOP
        MOVE.B  D1,D3          ;EXP OF BOT
        ANDI.W  #$FE00,D0      ;CLR S,EXP
        ANDI.W  #$FE00,D1      ;CLR S,EXP
; TEST EXPONENTS
        ADDQ.B  #1,D4     
        SUBQ.W  #1,D4          ;TOP
        BLE.S   TOPERR
        ADDQ.B  #1,D0          ;HIDDEN BIT
BACKTOP ADDQ.B  #1,D3     
        SUBQ.W  #1,D3          ;BOT
        BLE.S   BOTERR
        ADDQ.B  #1,D1          ;HIDDEN BIT
; POSITION MANTISSAS
BACKBOT ROR.L   #2,D1          ;01X...                     ;NORM
        ROR.L   #4,D0          ;0001X...
; COMPUTE TENTATIVE EXPONENT
        SUB.W   D3,D4
; TO COMPUTE AB/CD: 
;   FIRST DO AB/C -> Q, REMAINDER=R
        MOVE.W  D1,D3          ;SAVE D
        SWAP    D1             ;GET C
        DIVU    D1,D0          ;AB/C 29/15->15
        MOVE.W  D0,D2          ;SAVE Q
        MULU    D2,D3          ;Q*D
        CLR.W   D0             ;R IN TOP
        SUB.L   D3,D0          ;R-Q*D = +-31
        ASR.L   #2,D0          ;AVOID OFL
        DIVS    D1,D0          ;MORE QUOTIENT
        EXT.L   D2             ;Q
        EXT.L   D0             ;SECOND QUOT
        SWAP    D2
        ASL.L   #2,D0
        ADD.L   D2,D0          ;30-31 BITS
        ASL.L   #1,D0
; ADJ EXP, ROUND, CHECK EXTREMES, PACK
        ADDI.W  #127,D4
        JSR     C_ADDR(F_RCP)
; REPOSITION AND APPEND SIGN
DREPK   ROR.L   #8,D0
        ROXR.B  #1,D5          ;SIGN->X
        ROXR.L  #1,D0          ;INSERT SIGN

;       D0 = QUOTIENT
; EXIT INTERFACE
DEXIT   ADDQ.W  #4,SP
        MOVEM.L (SP)+,D1-D5
        RTS


; EXCEPTION HANDLING

TOPERR  BNE.S   TNGU
; TOP IS 0 OR GU, NORMALIZE AND RETURN                     ;NORM
NORMT   SUBQ.W  #1,D4                                      ;NORM
        ROL.L   #1,D0                                      ;NORM
        BHI.S   NORMT          ;LOOP TIL NORM, FALL IF 0   ;NORM
        ADDQ.W  #1,D4                                      ;NORM
        BRA.S   BACKTOP        ;0 OR GU

; TOP IS INF OR NAN
TNGU    CMP.B   D3,D4
        BEQ.S   DINVOP         ;BOTH INF/NAN: NAN
        TST.L   D0
        BEQ.S   GENINF         ;INF/... = +-INF
        BRA.S   DINVOP         ;NAN/... =   NAN

BOTERR  BEQ.S   BOTLOW         ;.../0|GU
                               ;.../INF|NAN
        TST.L   D1
        BNE.S   DINVOP         ;.../NAN = NAN
        CLR.L   D0             ;.../INF = +-0              ;NORM
        BRA.S   DREPK                                      ;NORM

BOTLOW  TST.L   D1
        BEQ.S   BOT0           ;.../0
                               ;.../GU:
NORMB   SUBQ.W  #1,D3                                      ;NORM
        ROL.L   #1,D1                                      ;NORM
        BCC.S   NORMB          ;LOOP TIL NORMALIZED        ;NORM
        ADDQ.W  #1,D3                                      ;NORM
        BRA.S   BACKBOT                                    ;NORM

; BOTTOM IS ZERO
BOT0    TST.L   D0
        BEQ.S   GENNAN         ;      0/0 =   NAN
                               ;NONZERO/0 = +-INF
; GENERATE INFINITY FOR ANSWER
GENINF  MOVE.L  #$FF,D0
        BRA.S   DREPK

; INVALID OPERAND/OPERATION
DINVOP  CMP.L   D0,D1
        BCS.S   USENAN         ;USE LARGER NAN
        TST.L   D1
        BEQ.S   GENNAN         ;BOTH ARE INF, GEN A NAN
        EXG     D1,D0          ;LARGER NAN
USENAN  LSR.L   #8,D0
        LSR.L   #1,D0
        BRA.S   BLDNAN         ;RETURN NAN

GENNAN  MOVEQ   #4,D0          ;NAN 4
BLDNAN  ORI.L   #$7F800000,D0
        BRA.S   DEXIT

        END

#pragma	endasm
 
#endif /* EXCLUDE_div */
 

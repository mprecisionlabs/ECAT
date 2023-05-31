#include "lib_top.h"


#if EXCLUDE_dmul
    #pragma option -Qws			/* Prevent compiler warning */
#else
 
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
; THIS MODULE PERFORMS DOUBLE PRECISION
; MULTIPLY FOR THE M68000.
*
* %W% %G% *
*

; revision history for DMUL:
; 28-Aug-86 fhw: change FMULI to fmuli
; 12-Aug-86 kjk: changed comments to reflect Fu's change to interface
; 16-May-86 ling: using pc relative 
; 9/12/90 jpc: allow use of long disp. in pc rel. addr.

; ENTRY CONDITIONS:
;	D0 - OPERAND 1, MOST SIGNIFICANT 32 BITS
;	D1 - OPERAND 1, LEAST SIGNIFICANT 32 BITS
;	ON STACK - OPERAND 2 (8 BYTES)

; EXIT CONDITIONS:
;	D0 - PRODUCT, MOST SIGNIFICANT 32 BITS
;	D1 - PRODUCT, LEAST SIGNIFICANT 32 BITS

; INTERNAL REGISTER CONVENTIONS:
;       D0-D3   OPERANDS OR PIECES OF RESULT
;       D5      EXPONENT OF LARGER
;       D5      TEMP FOR MULTIPLYING
;       D6      SIGN & EXPONENT
;       D7      EXPONENT OF SMALLER
;       D7      ZERO
 
*/

#pragma	asm
        section code

NSAVED  EQU     8*4

        XDEF  fmuli
        XREF  D_EXTE,D_NRCP,D_USEL
fmuli
;------
; LOAD RETURN ADDR AND OPNDS
        MOVEM.L D0-D7,-(SP)    ;SAVE
        MOVEM.L NSAVED+4(SP),D2-D3
; (END OF ENTRY INTERFACE)
; REGISTERS NOW CONTAIN:
;       D0      SECOND ARGUMENT, MOST SIGNIFICANT 32 BITS
;       D1      SECOND ARGUMENT, LEAST SIGNIFICANT 32 BITS
;       D2      FIRST ARGUMENT, MOST SIGNIFICANT 32 BITS
;       D3      FIRST ARGUMENT, LEAST SIGNIFICANT 32 BITS

; SAVE SIGN OF RESULT
        MOVE.L  D0,D5
        EOR.L   D2,D5          ;SIGN OF RESULT
        ASL.L   #1,D0          ;TOSS SIGN
        ASL.L   #1,D2   ;EEMMMMM0
        CMP.L   D2,D0
; ORDER OPERANDS (AT LEAST EXPONENTS)
        BLS.S   ESWAP
        EXG     D0,D2          ;D2/D3=LARGER
        EXG     D1,D3
; EXTRACT AND CHECK EXPONENTS
ESWAP   JSR     C_ADDR(D_EXTE)
        MOVE.W  D6,D5          ;LARGER EXP
        MOVE.L  D5,D6
        ADD.W   D7,D6          ;RESULT EXP (AND SIGN)
        CMP.W   #$7FF,D5       ;CK LARGER
        BEQ     OFL            ;INF OR NAN
        TST.W   D7             ;CK SMALLER
        BEQ     UFL            ;0 OR GU
; SET HIDDEN BITS
        BSET    #31,D0
BACK    BSET    #31,D2
; SPLIT MANTISSAS INTO 4 PIECES
        MOVEM.L D0-D3,-(SP)    ;STORE 8 WORDS
        MOVEM.W (SP),D0-D3     ;RELOAD ONE OPERAND, SPREAD OUT
        MOVE.W  4*2(SP),D5     ;0
        MULU    D5,D0          ;00
        MULU    D5,D1          ;   01
        MULU    D5,D2          ;      02
        MULU    D5,D3          ;         03
        CLR.L   D7             ;USED FOR ADDX INSTRUCTON

        MOVE.W  2*2(SP),D5
        MULU    5*2(SP),D5
        ADD.L   D5,D3          ;         12
        ADDX.L  D7,D1

        MOVE.W  1*2(SP),D5 
        MULU    6*2(SP),D5 
        ADD.L   D5,D3          ;         21
        ADDX.L  D7,D1

        MOVE.W     (SP),D5 
        MULU    7*2(SP),D5 
        ADD.L   D5,D3          ;         30
        ADDX.L  D7,D1

        MOVE.W  1*2(SP),D5
        MULU    5*2(SP),D5
        ADD.L   D5,D2          ;      11
        ADDX.L  D7,D0

        MOVE.W     (SP),D5 
        MULU    6*2(SP),D5 
        ADD.L   D5,D2          ;      20
        ADDX.L  D7,D0

        SWAP    D0
        MOVE.W     (SP),D5 
        MULU    5*2(SP),D5 
        ADD.L   D5,D1          ;   10
        ADDX.W  D7,D0
        SWAP    D0
        ADDA.W  #4*4,SP
; ADD TOGETHER
                               ;01--  IN D0
                               ;-12-  IN D1
                               ;--23  IN D2
                               ;---34 IN D3
        MOVE.W  D1,D3
        SWAP    D3             ;--23
        CLR.W   D1
        SWAP    D1             ;-1--
        ADD.L   D2,D3          ;--23
        ADDX.L  D7,D0
        ADD.L   D1,D0          ;01--
; PUT TOGETHER
        MOVE.L  D0,D2
        SUBI.W  #1022,D6       ;TOSS XTRA BIAS
        JSR     C_ADDR(D_NRCP)	;NORM,RND,CK,PACK
MSIGN   ROXL.L  #1,D6
        ROXR.L  #1,D2          ;APPEND SIGN
; PUT ANSWER ON STACK AND EXIT

; ANSWER IS NOW IN
;       D2      MOST SIGNIFICANT 32 BITS
;       D3      LEAST SIGNIFICANT 32 BITS
; (EXIT INTERFACE:)
MEXIT   MOVE.L  D2,NSAVED+4(SP)
        MOVE.L  D3,NSAVED+8(SP)
        MOVEM.L (SP)+,D0-D7
        MOVE.L  4(SP),D0
        MOVE.L  8(SP),D1
        MOVE.L  (SP)+,(SP)     ;RETURN ADDR
        MOVE.L  (SP)+,(SP)
        RTS


; EXCEPTION HANDLING
 
OFL     MOVE.L  D2,D5          ;LARGER MANTISSA, IF IT IS NAN, USE IT
        OR.W    D7,D5          ;SMALLER EXP
        OR.L    D0,D5
        OR.L    D1,D5          ;SMALLER VALUE
        BEQ.S   GENNAN         ;INF * 0
;  ELSE  NAN*X  OR  INF*NONZERO:
        MOVE.W  #$7FF,D6       ;INF OR NAN
        JSR     C_ADDR(D_USEL)
        BRA.S   MSIGN

UFL     MOVE.L  D0,D7
        OR.L    D1,D7          ;MANTISSA OF SMALLER
        BEQ.S   SIGNED0        ;0 * NUMBER
NORMU   SUBQ.W  #1,D6                                      ;NORM
        LSL.L   #1,D1          ;ADJ DENORMALIZED NUMBER
        ROXL.L  #1,D0
        BPL.S   NORMU          ;NORM A DENORM              ;NORM  
        ADDQ.W  #1,D6                                      ;NORM
        BRA     BACK                                       ;NORM
; (IF BOTH ARE DENORMALIZED, ANSWER WILL BE ZERO ANYWAY)   ;NORM

GENNAN  MOVE.L  #$7FF00002,D2
        BRA.S   MEXIT

SIGNED0 CLR.L   D2
        CLR.L   D3
        BRA.S   MSIGN

        END

#pragma	endasm
 
#endif /* EXCLUDE_dmul */
 

#include "lib_top.h"


#if EXCLUDE_dadd
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
; COPYRIGHT 1981, RICHARD E. JAMES III
*
* %W% %G% *
*
;
; THIS MODULE HANDLES DOUBLE PRECISION
;   ADD AND SUBTRACT FOR THE M68000.

; revision history for DADD:
; 28-Aug-86 fhw: change upper case to lower case
; 12-Aug-86 kjk: changed comments to reflect Fu's changes to interface
; 16-May-86 ling: using pc relative
; 9/12/90 jpc: allow use of long disp. in pc rel. addr.

; ENTRY CONDITION:
;	D0 - OPERAND 1, MOST SIGNIFICANT 32 BITS
;	D1 - OPERAND 1, LEAST SIGNIFICANT 32 BITS
;	ON STACK - OPERAND 2 (8 BYTES)

; EXIT CONDITION: 
;	D0 - RESULT, MOST SIGNIFICANT 32 BITS
;	D1 - RESULT, LEAST SIGNIFICANT 32 BITS

; INTERNAL REGISTER CONVENTIONS:
;       D0/D1   SMALLER OPERAND (D0=MOST SIGNIFICANT)
;       D2/D3   LARGER OPERAND
;       D4      11 OR MASK OF 11 BITS
;       D5      SIGNS: SIGN OF .W =SIGN OF ANSWER
;                      SIGN OF .B =COMPARISON OF SIGNS
;       D6      EXPONENT OF LARGER
;       D7      EXPONENT OF SMALLER
*/

#pragma	asm

        section code

NSAVED  EQU     8*4            ;8 REGS SAVED

        XDEF    faddi
        XDEF    fsubi
        XREF    D_EXTE,D_NORM,D_RCP,D_USEL
fsubi:
;------
        BCHG    #7,4(SP)
faddi:
;------
; SAVE REGS AND LOAD OPNDS
        MOVEM.L D0-D7,-(SP)    ;SAVE    
        MOVEM.L NSAVED+4(SP),D2-D3
; (END OF ENTRY INTERFACE)
; REGISTERS NOW CONTAIN:
;       D0      SECOND ARGUMENT, MOST SIGNIFICANT 32 BITS
;       D1      SECOND ARGUMENT, LEAST SIGNIFICANT 32 BITS
;       D2      FIRST ARGUMENT, MOST SIGNIFICANT 32 BITS
;       D3      FIRST ARGUMENT, LEAST SIGNIFICANT 32 BITS

; EXTRACT SIGNS
        ASL.L   #1,D0          ;SIGN -> C
        SCS     D4           
        ASL.L   #1,D2          ;SIGN -> C
        SCS     D5
; REORDER
        CMP.L   D2,D0    
        BLS.S   DADD1          ;GET LARGER INTO D2/D3
        EXG     D0,D2
        EXG     D1,D3 
        EXG     D4,D5
DADD1   EXT.W   D5             ;SIGN OF LARGER
        EOR.B   D4,D5          ;COMPARISON OF SIGNS
; EXTRACT EXPONENTS
        JSR     C_ADDR(D_EXTE)	;LARGER -> D2/D3,D6; SMALLER -> D0/D1,D7
        TST.W   D7
        BNE.S   NUNFL          ;NOT ZERO OR DENORMALIZED
; SMALLER IS 0 OR DENORMALIZED     
        MOVE.L  D0,D4
        OR.L    D1,D4
        BEQ     USELN          ;IF SMALLER = 0, USE LARGER
; (SIGN OF 0-0 UNPREDICTABLE)
        LSL.L   #1,D1
        ROXL.L  #1,D0
        TST.W   D6             ;LARGER EXP
        BNE.S   GU1            ;NOT GU
        LSL.L   #1,D3
        ROXL.L  #1,D2
        BRA.S   SHZ            ;BOTH GU, NO HIDDEN OR ALIGN NEEDED
NUNFL   BSET    #31,D0         ;ADD HIDDEN BIT
GU1     CMPI.W  #$7FF,D6
        BEQ     OVFL           ;INF/NAN
        BSET    #31,D2
; ALIGN SMALLER
        SUB.W   D6,D7          
        NEG.W   D7             ;D7 = DIFF OF EXPS
SHL     SUBQ.W  #8,D7   
        BLT.S   SHX            ;EXIT WHEN <8
        MOVE.B  D0,D1          ;SHIFT 8 BITS DOWN
        ROR.L   #8,D1
        LSR.L   #8,D0
        BNE.S   SHL            ;LOOP
        TST.L   D1
        BNE.S   SHL            ;LOOP WHILE NOT 0
        BRA.S   SHZ            ;ALL SIGNIFICANCE SHIFTED OFF
SHX     ADDQ.W  #7,D7
        BMI.S   SHZ
SHY     LSR.L   #1,D0          ;SHIFT ONE BIT AT A TIME
        ROXR.L  #1,D1
        DBRA    D7,SHY         ;FINAL PART OF ALIGNMENT
SHZ
; DECIDE WHETHER TO ADD OR SUBTRACT
        TST.B   D5             ;CMP SIGNS
        BMI.S   DIFF
; ADD THEM
        ADD.L   D1,D3          ;SUM
        ADDX.L  D0,D2
        BCC.S   ENDAS          ;NO C, OK
        ROXR.L  #1,D2          ;PULL OFL IN
        ROXR.L  #1,D3
        ADDQ.W  #1,D6     
        CMP.W   #$7FF,D6
        BLT.S   ENDAS          ;NO OFL
        BRA.S   GENINF
; SUBTRACT THEM
DIFF    SUB.L   D1,D3          ;SUBTRACT LOWERS
        SUBX.L  D0,D2          ;SUBTRACT UPPERS
        BCC.S   DIFF2
;  CANCELLED DOWN INTO 2ND WORD BUT GOT WRONG SIGN:
        NOT.W   D5             ;FLIP RESULT SIGN
        NEG.L   D3
        NEGX.L  D2             ;NEGATE VALUE
DIFF2   JSR     C_ADDR(D_NORM)
; REJOIN, ROUND                             
ENDAS   JSR     C_ADDR(D_RCP)	;ROUND, CHECK, AND PACK
ASSGN   LSL.W   #1,D5          ;GET SIGN
        ROXR.L  #1,D2          ; SEU     PUT IN SIGN
; EXIT
; ANSWER IS NOW IN
;       D2      MOST SIGNIFICANT 32 BITS
;       D3      LEAST SIGNIFICANT 32 BITS
; (EXIT INTERFACE:)
ASEXIT  MOVE.L  D2,NSAVED+4(SP)
        MOVE.L  D3,NSAVED+8(SP)
        MOVEM.L (SP)+,D0-D7
        MOVE.L  4(SP),D0
        MOVE.L  8(SP),D1
        MOVE.L  (SP)+,(SP)
        MOVE.L  (SP)+,(SP)
        RTS


; EXCEPTION CASES

USELN   BSET    #31,D2         ;HIDDEN BIT, IN CASE LARGER IS NUMBER
USEL    JSR     C_ADDR(D_USEL)	;USE THE LARGER
        BRA.S   ASSGN

; LARGER EXPONENT = 1023:
OVFL    MOVE.L  D2,D4          ;LARGER MANTISSA
        OR.L    D3,D4
        BNE.S   USEL           ;LARGER=NAN, USE IT
        CMP.W   D6,D7          ;EXPS
        BNE.S   USEL           ;LARGER=INF AND SMALLER=NUMBER
;(NEED NAN FOR PROJECTIVE, ...)
        TST.B   D5             ;COMPARISON OF SIGNS
        BPL.S   USEL           ;INF+INF=INF
                               ;INF-INF=NAN
        MOVE.L  #$7FF00001,D2  ;NAN (NOT-A-NUMBER)
        CLR.L   D3
        BRA.S   ASEXIT 
 
; RESULT OVERFLOWS:
GENINF  MOVE.L  #$FFE00000,D2
        CLR.L   D3
        BRA.S   ASSGN

        END

#pragma	endasm
 
#endif /* EXCLUDE_dadd */
 

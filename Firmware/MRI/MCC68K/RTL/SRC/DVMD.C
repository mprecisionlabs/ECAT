#include "lib_top.h"


#if EXCLUDE_dvmd
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
; THIS SUBROUTINE PERFORMS DOUBLE PRECISION DIVIDE AND MOD FOR M68000.
*
* %W% %G% *
*

; revision history for DVMD:
; 28-Aug-86 fhw: change FDIVI to fdivi
; 12-Aug-86 kjk: modified comments to reflect Fu's change to interface
; 16-May-86 ling: using pc relative 
; 19-jul-89 jpc: handle nan's w. bit in low word correctly
; 9/12/90 jpc: allow use of long disp. in pc rel. addr.


; ENTRY CONDITIONS:
;	D0 - OPERAND 1, MOST SIGNIFICANT 32 BITS
;	D1 - OPERAND 1, LEAST SIGNIFICANT 32 BITS
;	ON STACK - OPERAND 2 (8 BYTES)

; EXIT CONDITIONS:
;	D0 - QUOTIENT, MOST SIGNIFICANT 32 BITS
;	D1 - QUOTIENT, LEAST SIGNIFICANT 32 BITS

; INTERNAL REGISTER CONVENTIONS:
;       D0/D1   DIVISOR (D0=UPPER)
;       D2/D3   DIVIDEND
;       D4      COUNT
;       D5      SIGN & EXPONENT
;       D6      RESULT EXPONENT
;       D6/D7   QUOTIENT
*/

#pragma	asm
 
        section code
        XDEF  fdivi

NSAVED  EQU     8*4

        XREF  D_NRCP,D_NORM,D_RCP
fdivi
;------
; LOAD RETURN ADDR AND OPNDS
        MOVEM.L D0-D7,-(SP)    ;SAVE
        MOVEM.L NSAVED+4(SP),D2-D3
        EXG     D0,D2
        EXG     D1,D3
; (END OF ENTRY INTERFACE)
; REGISTERS NOW CONTAIN:
;       D0      DIVISOR, MOST SIGNIFICANT 32 BITS
;       D1      DIVISOR, LEAST SIGNIFICANT 32 BITS
;       D2      DIVIDEND, MOST SIGNIFICANT 32 BITS
;       D3      DIVIDEND, LEAST SIGNIFICANT 32 BITS

; SAVE SIGN OF RESULT
        MOVE.L  D0,D5
        EOR.L   D2,D5          ;SIGN OF RESULT
        CLR.L   D4             ;FLAG FOR DIV
        BSR.S   EXTREM
; COMPUTE RESULTING EXPONENT
        MOVE.W  D6,D5
        SUB.W   D7,D5          ;EXP (AND SIGN)
; DO TOP 30-31 BITS OF DIVIDE (D_RCP WILL TO POST NORMALIZE)
        MOVE.W  #30,D4         ;COUNT 30..0
        BSR     SHSUB          ;SHIFT AND SUBTRACT LOOP
        MOVE.L  D7,D6          ;TOP OF ANSWER
; DO NEXT 22 BITS OF DIVIDE
        MOVE.W  #23,D4         ;COUNT 23..0 (TOTAL = 54-55 BITS)
        BSR     SHSUB
; PUT TOGETHER ANSWER
        LSL.L   #8,D7          ;LINE UP BOTTOM
        OR.L    D3,D2
        BEQ.S   DD4
        BSET    #1,D7          ;STICKY BIT ON IF REMAINDER <> 0
DD4     LSL.L   #1,D7
        ROXL.L  #1,D6          ;ONE NORM
; NORMALIZE (ONCE), ROUND, CHECK RESULT EXP, PACK
        MOVE.L  D6,D2          ;UPPER
        MOVE.L  D7,D3          ;LOWER
        MOVE.W  D5,D6          ;EXPONENT
        ADDI.W  #1022+1,D6     ;RE-BIAS
        JSR     C_ADDR(D_NRCP)	;NORM ONCE,RND,CK,PACK
        BRA     DMSIGN
        PAGE    ;--------------
; SUBROUTINE FOR UNPACKING ONE OPERAND, AND NORMALIZING A DENORMAL.
; INPUT
;       D0/D1   NUMBER 

; OUTPUT
;       D0/D1   MANTISSA
;       D7.W    EXPONENT
;       Z       ON IFF MANTISSA IS ZERO

; UNCHANGED
;       D4      BOTTOM = $07FF

UNP
;------
        MOVE.L  D0,D7          ;START GETTING EXP          ;NORM
        ANDI.L  #$000FFFFF,D0  ;CLEAR OUT SIGN AND EXP     ;NORM
        SWAP    D7                                         ;NORM
        LSR.W   #16-1-11,D7                                ;NORM
        AND.W   D4,D7          ;EXPONENT                   ;NORM
        BNE.S   UNPXIT         ;NORMAL NUMBER              ;NORM
; DENORMALIZED NUMBER OR ZERO:                             ;NORM
        TST.L   D0             ;UPPER                      ;NORM
        BNE.S   UNP2                                       ;NORM
        TST.L   D1             ;LOWER                      ;NORM
        BEQ.S   UNPXIT         ;ZERO                       ;NORM
UNP2    ADDQ.W  #1,D7                                      ;NORM
UNPL    SUBQ.W  #1,D7                                      ;NORM
        LSL.L   #1,D1                                      ;NORM
        ROXL.L  #1,D0          ;NORMALIZE                  ;NORM
        BTST    #20,D0                                     ;NORM
        BEQ.S   UNPL           ;LOOP UNTIL NORMALIZED      ;NORM
UNPXIT  RTS                                                ;NORM


; SUBROUTINE FOR EXTRACTING AND TAKING CARE OF 0/GU/INF/NAN.

; INPUT
;       D0/D1, D2/D3
;       D4      + FOR DIV; - FOR MOD

; OUTPUT
;       D0/D1 (BOTTOM) AND D2/D3 (TOP) CONVERTED TO
;               000XXXXX/XXXXXXXX
;       D6      EXPONENT OF TOP
;       D7      EXPONENT OF BOTTOM

; UNCHANGED
;       D5      SIGN

EXTREM
;------
        MOVE.W  #$7FF,D4       ;MASK, SIGN.L UNTOUCHED     ;NORM
        EXG     D2,D0                                      ;NORM
        EXG     D3,D1                                      ;NORM
        BSR.S   UNP            ;UNPACK TOP                 ;NORM
        EXG     D0,D2                                      ;NORM
        EXG     D1,D3                                      ;NORM
        EXG     D7,D6                                      ;NORM
        BEQ.S   TOPZERO        ;TOP IS ZERO                ;NORM
        CMP.W   D4,D6
        BEQ.S   TOPBIG         ;TOP IS INF OR NAN
        BSET    #20,D2         ;SET HIDDEN BIT
TOPZERO BSR.S   UNP            ;UNPACK BOTTOM              ;NORM
        BEQ.S   BOTZERO        ;BOTTOM IS 0                ;NORM
        CMP.W   D4,D7
        BEQ.S   BOTBIG         ;BOTTOM IS INF OR NAN
        BSET    #20,D0
        LSL.L   #1,D1          ;LEFT SHIFT BOTTOM
        ROXL.L  #1,D0
        RTS

; EXCEPTION HANDLING
                                                           ;NORM
; TOP IS INF OR NAN
TOPBIG  BSR.S   UNP            ;UNPACK BOTTOM              ;NORM
        TST.L   D4                                         ;NORM
        BMI.S   ISNAN          ;DMOD(INF OR NAN, ...) = NAN
                               ;INF OR NAN / ...
                                                           ;NORM
        CMP.W   D6,D7
        BEQ.S   ISNAN		;BOTH INF/NAN
        TST.L   D2		;TOP
        BNE.S   ISNAN		;INF / ... = INF	-- jpc
        TST.L   D3		;TOP			-- jpc
        BNE.S   ISNAN		;INF / ... = INF	-- jpc
        BRA.S   GENINF		;NAN / ... = NAN	-- jpc

; BOTTOM IS 0                                              ;NORM
BOTZERO TST.L   D4                                         ;NORM
        BMI.S   GENNAN         ;DMOD(..., 0) = NAN         ;NORM
        OR.L    D2,D3          ;TOP                        ;NORM
        BEQ.S   GENNAN         ;0 / 0 = NAN
                               ;NONZERO/0 = INF
; GENERATE INFINITY FOR ANSWER
GENINF  MOVE.L  #$7FF00000*2,D2  ;INFINITY
        BRA.S   CLRBOT         ;= INF

BOTBIG  TST.L   D0
        BNE.S   ISNAN          ;BOTTOM = NAN, RESULT = NAN
	TST.L   D1	       ;TEST LOW HALF -- jpc
        BNE.S   ISNAN          ;BOTTOM = NAN, RESULT = NAN
        TST.L   D4
        BPL.S   GENZERO        ;... / INF = 0
        ADDQ.W  #4,SP
        BRA     USETOP         ;DMOD(TOP, INF) = TOP

; INVALID OPERAND/OPERATION
ISNAN   CMP.W   D7,D6
        BNE.S   ISN2           ;EXPONENTS EQUAL
        CMP.L   D0,D2
ISN2    BGE.S   ISN4
        MOVE.W  D7,D6          ;GET LARGER NUMBER
        MOVE.L  D0,D2
	MOVE.L	D1,D3	       ;NAN FIX -- jpc
ISN4    SWAP    D2
        LSL.W   #16-11-1,D6
        OR.W    D6,D2          ;PUT BACK TOGETHER
        SWAP    D2
        LSL.L   #1,D2
        CMPI.L  #$7FF00000*2,D2
        BHI.S   SIGN		;USE LARGER NAN
	TST.L	D3		;BITS ON LOW LONG WORD -- NAN FIX -- jpc
	BGT	SIGN

GENNAN  MOVE.L  #$7FF00004*2,D2
        TST.L   D4
        BPL.S   GOTNAN         ;NAN 4 FOR DIV
        ADDQ.L  #2,D2          ;NAN 5 FOR MOD
GOTNAN  CLR.L   D5             ;GENERATE ONLY +NAN
        BRA.S   CLRBOT

GENZERO CLR.L   D2             ;RESULT = 0

CLRBOT  CLR.L   D3
SIGN    ADDQ.W  #4,SP          ;POP
        BRA     DMSIGN
        PAGE    ;--------------
; THE SHSUB SUBROUTINE DOES A SHIFT-SUBTRACT LOOP.
; THAT IS THE HEART OF DIVIDE AND MOD.
;   THE ALGORITHM IS A SIMPLE SHIFT AND SUBTRACT LOOP, 
;     BUT IT ADDS WHEN IT OVERSHOOTS.
;   WHY NOT USE THE DIVS/DIVU INSTRUCTIONS?  THAT APPROACH IS SLOWER!

; REGISTERS
;       D2/D3   CURRENT DIVIDEND (UPDATED)
;       D0/D1   DIVISOR (UNCHANGED)
;       D4.W    (INPUT) NUMBER OF ITERATIONS -1, AND BIT NUMBER
;       D5/D6   -UNTOUCHED-
;       D7      QUOTIENT BEING DEVELOPED (IGNORED BY MOD)

SHSUB
;------
        CLR.L   D7             ;QUOTIENT
; SHIFT ONCE, SEE IF SUBTRACT NEEDED
SHS1    ADD.L   D3,D3
        ADDX.L  D2,D2          ;(64-BIT LEFT SHIFT)
        CMP.L   D0,D2
        DBGE    D4,SHS1        ;LOOP WHILE DIVIDEND IS SMALL
; TALLY QUOTIENT AND SUBTRACT
        BSET    D4,D7          ;QUOTIENT BIT
        SUB.L   D1,D3
        SUBX.L  D0,D2          ;64-BIT SUBTRACT
        DBMI    D4,SHS1        ;LOOP D4 TIMES
; NOW ONE OF THREE THINGS HAS HAPPENED:
;  1.  COUNT EXHAUSTED AND EXTRA SUBTRACT DONE (FIRST DB HIT COUNT)
;  2.  COUNT EXHAUSTED IN SECOND DB
;  3.  OVERSHOT BECAUSE COMPARE DIDN'T CHECK LOWER PARTS
        BPL.S   SHS7           ;CASE 2
        ADD.L   D1,D3          ;TAKE CARE OF OVERSHOOT
        ADDX.L  D0,D2
        BCLR    D4,D7
        TST.W   D4
        DBLT    D4,SHS1        ;CASE 3
                               ;CASE 1
SHS7    RTS
        PAGE    ;--------------

        XDEF  D_MOD
; THIS ROUTINE PERFORMS A DOUBLE DMOD.
; IT IS PASCAL CALLABLE, IN THE SENSE THAT ROOM IS LEFT ON THE
;   STACK FOR THE RESULT (BY THE CALLER).  

;   DEFINITION:
;       DMOD(TOP,BOT) = RSLT ,  SUCH THAT:
;       N * BOT + RSLT = TOP  FOR SOME INTEGER.
;       ABS(RSLT) < ABS(BOT)
;       RSLT HAS SAME SIGN AS TOP (OR IS ZERO).
;       DMOD(TOP,BOT) = DMOD(TOP,ABS(BOT))
;       DMOD(-TOP,BOT) = -DMOD(TOP,BOT)

; INPUT:
;       ROOM FOR RESULT
;       TWO NUMBERS
; OUTPUT:  REMAINDER ON STACK.

; REGISTER CONVENTIONS
;       D0/D1   BOT
;       D2/D3   TOP
;       D4      FLAG AND COUNT
;       D5      SIGN OF TOP AND RESULT
;       D6      EXPONENT
;       D7      EXPONENT

D_MOD
;------
        MOVE.L  (SP)+,4*4(SP)  ;STASH AWAY RETURN ADDR
        BSR.S   DMOD
        MOVE.L  4(SP),3*4(SP)  ;BOTTOM
        MOVE.L  2*4(SP),4(SP)  ;RETURN ADDR
        MOVE.L  (SP)+,4(SP)    ;TOP
        RTS

DMOD
        MOVEM.L D0-D7,-(SP)    ;PRESERVE ALL REGS THAT WILL BE USED
        MOVEM.L NSAVED+4(SP),D0-D3  ;OPERANDS
; (END OF ENTRY INTERFACE)
; REGISTERS NOW CONTAIN:
;       D0      SECOND ARGUMENT, MOST SIGNIFICANT 32 BITS
;       D1      SECOND ARGUMENT, LEAST SIGNIFICANT 32 BITS
;       D2      FIRST ARGUMENT, MOST SIGNIFICANT 32 BITS
;       D3      FIRST ARGUMENT, LEAST SIGNIFICANT 32 BITS

        MOVE.L  D2,D5          ;SAVE SIGN OF TOP FOR RESULT
        MOVEQ   #-1,D4         ;MOD FLAG
        BSR     EXTREM         ;UNPACK AND CK END CASES
        MOVE.W  D6,D4          ;TOP EXP
        SUBQ.W  #1,D7
        SUB.W   D7,D4          ;NUMBER OF ITERATIONS = TOP - BOT +1
        BLE.S   USETOP         ;TOP IS THE ANSWER
        LSL.L   #1,D1          ;ADJUST BOT SO THAT BOT>TOP
        ROXL.L  #1,D0
        MOVE.W  D7,D6          ;RESULT USES BOTTOM EXP
        SUBQ.W  #1,D6
        BSR.S   SHSUB          ;DO MAIN WORK
USETOP  ADDI.W  #11,D6
        JSR     C_ADDR(D_NORM)	;NORMALIZE
        JSR     C_ADDR(D_RCP)	;ADJUST EXP, CK EXTREMES, PACK

; COMMON EXIT FOR DIVIDE AND MOD:
; APPEND SIGN
DMSIGN  ROXL.L  #1,D5          ;SIGN->X
        ROXR.L  #1,D2          ;INSERT SIGN
; ANSWER IS NOW IN
;       D2      MOST SIGNIFICANT 32 BITS
;       D3      LEAST SIGNIFICANT 32 BITS
; (EXIT INTERFACE:)
DMEXIT  MOVE.L  D2,NSAVED+4(SP)
        MOVE.L  D3,NSAVED+8(SP)
        MOVEM.L (SP)+,D0-D7
        MOVE.L  4(SP),D0
        MOVE.L  8(SP),D1
        MOVE.L  (SP)+,(SP)
        MOVE.L  (SP)+,(SP)
        RTS

        END

#pragma	endasm
 
#endif /* EXCLUDE_dvmd */
 

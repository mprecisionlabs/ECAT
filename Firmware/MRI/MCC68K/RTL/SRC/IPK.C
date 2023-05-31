#include "lib_top.h"


#if EXCLUDE_ipk
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
; 8-28-86 fhw: change FFLTIS, FSFIXI to ffltis, fsfixi
; 5-16-86 ling: using pc relative
; 9/12/90 jpc: allow use of long disp. in pc rel. addr.
; THIS MODULE PERFORMS VARIOUS TYPE COERCIONS FOR FORTRAN.
; ALSO INCLUDED ARE THE ROUNDING AND TRUNCATING OPERATIONS.
*
* %W% %G% *
*

; THE PATTERN OF THE ENTRY POINTS FOR TYPE COERCION IS
;       X_2_Y
; WHERE
;       X IN [I,F] IS THE SOURCE TYPE
;       Y IN [I,F] IS THE DESTINATION TYPE
;       I AND F FOR INTEGER AND SINGLE, RESPECTIVELY.

; THE CALLING SEQUENCE IS
;       ENTER WITH STACK CONTAINING
;               ARGUMENT (4 BYTES)
;               RETURN ADDRESS
;       EXIT WITH STACK CONTAINING
;               RESULT (4 BYTES)
;       ALL REGISTERS ARE PRESERVED.

; THE LOGIC FOR EACH IS
;       SAVE REGISTERS.
;       LOAD ARGUMENT.
;       UNPACK INTO INTERNAL FORM.
;       REPACK INTO DESTINATION FORM.
;       STORE RESULT ON STACK.
;       RESTORE REGISTERS AND RETURN.

*/

#pragma	asm
        section code

NSAVED  EQU     5*4            ;SAVE D0-D4

        XDEF  ffltis,fsfixi

        XREF  F_PACK,F_UNPK
        XREF  U_LOAD,U_STOR

ffltis                          ; FLOAT, REAL
;------
        MOVEM.L D0-D4,-(SP)
        MOVE.L  D0,D1
        BSR.S   I_UNPK
        JSR     C_ADDR(F_PACK)
        MOVE.L  D1,D0
        ADDQ.W  #4,SP
        MOVEM.L (SP)+,D1-D4
        RTS
fsfixi                         ; IFIX, INT
;------
        MOVEM.L D0-D4,-(SP)
        MOVE.L  D0,D1
        JSR     C_ADDR(F_UNPK)
        BSR.S   I_PACK
        MOVE.L  D1,D0
        ADDQ.W  #4,SP
        MOVEM.L (SP)+,D1-D4
        RTS
        PAGE    ;--------------
        XDEF  I_UNPK,I_PACK
;
; UTILITIES FOR CONVERTING BETWEEN INTERNAL UNPACKED FORM AND INTEGER.

; REGISTERS
;       D1      INTEGER
;          VS.
;       D0/D1   NUMBER (64-BIT INTEGER)
;       D2.W    EXPONENT
;       D3.W    SIGN IN SIGN BIT
;       D3.B    KIND (3)

I_UNPK
;------
        CLR.W   D2             ;EXPONENT
        MOVEQ   #3,D3          ;SIGN=+, KIND=NUMBER
        TST.L   D1             ;LOWER
        BPL.S   IUPOS
        NEG.L   D1             ;(LARGEST NEG NUM WILL BE OK!)
        BSET    #15,D3         ;NEGATIVE
IUPOS   CLR.L   D0             ;UPPER
        RTS

I_PACK
;------
        CMPI.B  #4,D3
        BGE.S   BIGI           ;INF OR NAN
        BSR.S   GNSH           ;SHIFT BY THE EXPONENT
        OR.W    D2,D0
        TST.L   D0
        BNE.S   BIGI           ;>= 2**32 OR GNSH COUNDN'T DO IT ALL
        TST.L   D1             ;LOWER
        BMI.S   BIGI           ;>= 2**31
        TST.W   D3             ;SIGN
        BPL.S   IPZ
        NEG.L   D1             ;CHANGE SIGN (2'S COMPL)
IPZ     RTS

BIGI    MOVE.L  #$80000000,D1  ;DO SOMETHING FOR BIG REALS
        RTS
        PAGE    ;--------------
        XDEF  PG_NSH
; NOMINALLY SHIFT THE MANTISSA BY THE AMOUNT IN THE EXPONENT.
; IF RESULT WILL NOT FIT IN 64 BITS, THE EXPONENT IS LEFT NON-ZERO,
;     GARBAGE IN UPPER/LOWER.

; INPUT:
;       ADDR OF RECORD
; REGISTERS: A0, D0-D4 DESTROYED

PG_NSH
;------
        JSR     C_ADDR(U_LOAD)
        BSR.S   GNSH
        JMP     C_ADDR(U_STOR)

; INPUT AND OUTPUT: D0-D3  UNPACKED RECORD.
; OUTPUT: D2.L=0  IF SHIFT COULD BE DONE.

        XDEF  G_NSH
GNSH
G_NSH
;------
        MOVEQ   #32,D4
        TST.W   D2             ;EXPONENT
        BMI.S   SHR            ;RIGHT SHIFT
; LEFT SHIFT
LL      CMP.W   D4,D2
        BLT.S   SIMPL          ;SIMPLE LEFT
        TST.L   D0
        BNE.S   NSHOFL         ;TOO BIG
        SUB.W   D4,D2
        EXG     D1,D0          ;SHIFT 32
        BRA.S   LL     
SIMPL   ROL.L   D2,D0
        ROL.L   D2,D1
        MOVEQ   #1,D4
        ASL.L   D2,D4
        SUBQ.L  #1,D4          ;FORM MASK
        MOVE.L  D0,D2
        AND.L   D4,D2          ;OFL PART
        BNE.S   NSHOFL
        AND.L   D1,D4          ;BETWEEN PARTS
        EOR.L   D4,D1          ;CLEAR BOTTOM
        OR.L    D4,D0          ;INTO TOP
        BRA.S   NSHXIT         ;EXIT

; SHIFT RIGHT
SHR     NEG.W   D2             ;-EXP
LR      CMP.W   D4,D2
        BLT.S   SIMPR
        MOVE.L  D0,D1          ;SHIFT 32
        CLR.L   D0
        SUB.W   D4,D2
        BRA.S   LR
SIMPR   MOVEQ   #1,D4
        ASL.L   D2,D4
        SUBQ.L  #1,D4          ;FORM MASK
        NOT.L   D4
        AND.L   D4,D1          ;LOWER, 0
        AND.L   D0,D4          ;UPPER, 0
        EOR.L   D4,D0          ;0, MID
        OR.L    D0,D1          ;LOWER, MID
        ROR.L   D2,D4          ;0, UPPER
        ROR.L   D2,D1          ;MID, LOWER
        MOVE.L  D4,D0          ;UPPER
NSHXIT  CLR.L   D2
NSHOFL
        RTS                    ;RETURN FROM G_NSH
        PAGE    ;--------------
        XDEF  I_NINT,F_ANINT
        XDEF  G_NINT

; ROUTINES TO FIND THE NEAREST INTEGER.
; GENERAL DESIGN:
;       LOAD ARGUMENT
;       UNPACK INTO INTERNAL FORM
;       IF EXPONENT IS POSITIVE THEN
;               SHIFT TO FIND BIT AFTER DECIMAL POINT
;               ROUND UP IF 1
;       REPACK
;       STORE RESULT ON STACK AND RETURN

; THE CALLING SEQUENCE IS
;       ENTER WITH STACK CONTAINING
;               ROOM FOR ANSWER (4 BYTES)
;               ARGUMENT (4 BYTES)
;               RETURN ADDRESS
;       EXIT WITH STACK CONTAINING
;               RESULT (4 BYTES)
;       REGISTERS D0-D4 ARE DESTROYED.

G_NINT                        ; (INTERNAL ROUTINE)
;------
        TST.W   D2             ;EXPONENT
        BPL.S   NORND          ;DO NOT NEED TO ADD .5
        ADDQ.W  #1,D2
        BSR.S   GNSH           ;SHIFT TO GET XXXXX.X
        ASR.L   #1,D0          ;DIV 2
        ROXR.L  #1,D1
        ADDX.L  D2,D1          ;ADDS ONE IF BIT SHOVED OFF
        ADDX.L  D2,D0          ;NOTE: GNSH LEFT D2.L=0
NORND   RTS

O_ARG   EQU     4              ;WHERE ARGUMENT IS ON STACK
O_RTN   EQU     8              ;WHERE TO PUT RESULT ON STACK
;       O_RTN = 8 ASSUMES PASCAL CALLING SEQUENCE
;       O_RTN = 4 ASSUMES REUSE OF STACK

I_NINT                        ; NINT ( REAL ) -> INTEGER
;------
        MOVE.L  O_ARG(SP),D1
        JSR     C_ADDR(F_UNPK)
        BSR.S   G_NINT
        BSR     I_PACK
        MOVE.L  D1,O_RTN(SP)
        MOVE.L  (SP)+,(SP)
        RTS
F_ANINT                       ; ANINT ( REAL ) -> REAL
;------
        MOVE.L  O_ARG(SP),D1
        JSR     C_ADDR(F_UNPK)
        BSR.S   G_NINT
        BRA.S   AIXIT
        PAGE    ;--------------
        XDEF  F_AINT
        XDEF  G_INT         ;FOR INTERNAL USE ONLY
; TRUNCATION IS LIKE ROUNDING WITHOUT THE ROUND.

G_INT
;------
        TST.W   D2             ;EXPONENT
        BMI     GNSH 
        RTS

F_AINT                        ; AINT ( REAL ) -> REAL
;------
        MOVE.L  O_ARG(SP),D1
        JSR     C_ADDR(F_UNPK)
        BSR.S   G_INT

AIXIT   JSR     C_ADDR(F_PACK)
        MOVE.L  D1,O_RTN(SP)
        MOVE.L  (SP)+,(SP)
        RTS

        END

#pragma	endasm
 
#endif /* EXCLUDE_ipk */
 

#include "lib_top.h"


#if EXCLUDE_dpk
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
; COPYRIGHT 1981, 1982 RICHARD E. JAMES III
; 1-22-88 ajt: added routine ffixi1 for round function
; 8-28-86 fhw: change upper case to lower case
; 5-16-86 ling: using pc relative
; 9/12/90 jpc: allow use of long disp. in pc rel. addr.
; THIS MODULE CONTAINS DOUBLE PRECISION ROUNDING, CONVERSION, AND
;   TRUNCATING OPERATIONS, PLUS OTHER UTILITIES FOR FORTRAN INTRINSICS.
*
* %W% %G% *
*

; TYPE COERCION
;   THE PATTERN OF THE ENTRY POINTS IS
;       X_2_Y
;   WHERE
;       X IN [I,F,D] IS THE SOURCE TYPE
;       Y IN [I,F,D] IS THE DESTINATION TYPE
;       I, F, D FOR INTEGER, SINGLE, DOUBLE RESPECTIVELY.

; THE CALLING SEQUENCE IS
;       ENTER WITH STACK CONTAINING
;               ARGUMENT (4 OR 8 BYTES)
;               RETURN ADDRESS
;       EXIT WITH STACK CONTAINING
;               RESULT
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

NSAVED  EQU     4*5            ;SAVE D2-D3

        XDEF    fflti,fsinglei,ffixi,fdoublei,ffixi1

        XREF    I_PACK,I_UNPK
        XREF    F_PACK,F_UNPK
        XREF    G_NINT,G_INT
	XREF	faddi

fflti:                         ; DBLE ( INTEGER )
;------
;       MOVE.L  (SP),-(SP)
;       MOVE.L  8(SP),4(SP)    ;MAKE EXTRA ROOM ON STACK
        MOVEM.L D0-D4,-(SP)
        MOVE.L  D0,D1
        JSR     C_ADDR(I_UNPK)
        BRA.S   XIT_2_D
fdoublei:                         ; DOUBLE ( REAL )
;------
;       MOVE.L  (SP),-(SP)
;       MOVE.L  8(SP),4(SP)
        MOVEM.L D0-D4,-(SP)
        MOVE.L  D0,D1
        JSR     C_ADDR(F_UNPK)
XIT_2_D BSR     D_PACK
        ADDQ.W  #8,SP
        MOVEM.L (SP)+,D2-D4
        RTS
ffixi:                         ; INT ( DOUBLE )
;------
        MOVEM.L D0-D4,-(SP)
        BSR     D_UNPK
        JSR     C_ADDR(I_PACK)
        BRA.S   D_2_XIT
ffixi1:                        ; INT ( DOUBLE )
;------
        CMPI.L	#0,D0
	BEQ.S	TRUNC
	BGT.S	PLUS
	MOVE.L	0,-(SP)
	MOVE.L	#-1075838976,-(SP)
	JMP	ADDIT
PLUS	MOVE.L	0,-(SP)
	MOVE.L	#1071644672,-(SP)
ADDIT	JSR	C_ADDR(faddi)
TRUNC	JSR	C_ADDR(ffixi)
	RTS

fsinglei:                         ; REAL ( DOUBLE )
;------
        MOVEM.L D0-D4,-(SP)
        BSR.S   D_UNPK
        JSR     C_ADDR(F_PACK)
D_2_XIT MOVE.L  D1,D0
        ADDQ.W  #4,SP
        MOVEM.L (SP)+,D1-D4
        RTS
        PAGE    ;--------------
        XDEF    I_IDNINT,D_DNINT,D_DINT

; ROUTINES TO FIND THE NEAREST INTEGER (ROUNDED/TRUNCATED).

; THE CALLING SEQUENCE IS
;       ENTER WITH STACK CONTAINING
;               ROOM FOR ANSWER (4 OR 8 BYTES)
;               ARGUMENT (4 OR 8 BYTES)
;               RETURN ADDRESS
;       EXIT WITH STACK CONTAINING
;               RESULT (4 OR 8 BYTES)
;       REGISTERS D0-D4 ARE DESTROYED.

; GENERAL DESIGN:
;       LOAD ARGUMENT
;       UNPACK INTO INTERNAL FORM
;       IF EXPONENT IS POSITIVE THEN
;               SHIFT TO FIND BIT AFTER DECIMAL POINT
;               ROUND UP IF 1
;       REPACK

O_ARG   EQU     4              ;WHERE ARGUMENT IS ON STACK
O_RTN   EQU     8              ;WHERE TO PUT RESULT ON STACK
;       O_RTN = 8 ASSUMES PASCAL CALLING SEQUENCE
;       O_RTN = 4 ASSUMES REUSE OF STACK
I_IDNINT:                      ; IDNINT ( DOUBLE ) -> INTEGER
;------
        MOVEM.L O_ARG(SP),D0/D1
        BSR.S   D_UNPK
        JSR     C_ADDR(G_NINT)
        JSR     C_ADDR(I_PACK)
        MOVE.L  D1,O_RTN+4(SP)
        MOVE.L  (SP)+,(SP)     ;ADJUST STACK
        MOVE.L  (SP)+,(SP)     ;ADJUST STACK
        RTS
D_DNINT:                       ; DNINT ( DOUBLE ) -> DOUBLE
;------
        MOVEM.L O_ARG(SP),D0/D1
        BSR.S   D_UNPK
        JSR     C_ADDR(G_NINT)
        BRA.S   DIXIT
D_DINT:                        ; DINT ( DOUBLE ) -> DOUBLE
;------
        MOVEM.L O_ARG(SP),D0/D1
        BSR.S   D_UNPK
        JSR     C_ADDR(G_INT)
DIXIT   BSR.S   D_PACK
        MOVEM.L D0/D1,O_RTN+4(SP)                  ;RTN
        MOVE.L  (SP)+,(SP)
        MOVE.L  (SP)+,(SP)
        RTS
        PAGE    ;---------------------
        XDEF    PD_UNPK,D_UNPK
        XREF    U_STOR
; SPLIT A DOUBLE PRECISION NUMBER INTO ITS
;   SIGN, EXPONENT, AND MANTISSA.
                              
; INPUT:
;       FLOATING VALUE
;       ADDR OF RECORD
; REGISTERS: A0,D0-D4 DESTROYED.

; FORMAT OF UNPACKED RECORD:
;       RECORD
;       D0/D1   MANTISSA: ARRAY[2] OF LONGINT;
;       D2.W    EXPONENT: -32768..32767;
;       D3.W    SIGN:     (BIT 15)
;       D3.B    TYPE:     1..5               
;       END;

PD_UNPK
;------
        MOVE.L  8(SP),A0       ;ADDR OF NUMBER
        MOVEM.L (A0),D0/D1     ;NUMBER
        BSR.S   D_UNPK
        JMP     C_ADDR(U_STOR)

; INPUT:  D0/D1  NUMBER
; OUTPUT: D0-D3  UNPACKED RECORD
; REGISTERS DESTROYED:  D0-D4

D_UNPK:
;------
        MOVE.L  #$FFF00000,D2  ;MASK FOR SIGN AND EXPONENT
        MOVE.L  D0,D3
        SWAP    D3             ;SIGN
        AND.L   D0,D2          ;EXTRACT EXPONENT
        EOR.L   D2,D0          ;TOP OF MANTISSA CLEARED OUT
        MOVE.L  D1,D4
        OR.L    D0,D4          ;NON-ZERO IFF MANTISSA NON-ZERO
        LSL.L   #1,D2          ;TOSS SIGN
        BNE.S   UNP2           ;NOT 0/GU
        MOVE.B  #1,D3          ;--ZERO   
        TST.L   D4                     
        BEQ.S   UNP8                   
        MOVE.B  #2,D3          ;--GU     
        BRA.S   UNP8
UNP2    SWAP    D2
        LSR.W   #16-11,D2      ;EXP TO BOTTOM OF REG
        CMP.W   #$7FF,D2       ;INF/NAN       
        BNE.S   UNP3           ;PLAIN
        MOVE.W  #$6000,D2
        MOVE.B  #4,D3          ;--INF    
        TST.L   D4                     
        BEQ.S   UNP9                   
        MOVE.B  #5,D3          ;--NAN    
        BRA.S   UNP9                   
UNP3    BSET    #20,D0         ;HIDDEN BIT
        SUBQ.W  #1,D2                  
        MOVE.B  #3,D3          ;--NUM    
UNP8    SUBI.W  #1022+52,D2
UNP9    RTS
        PAGE    ;-----------------------
        XDEF    PD_PACK,D_PACK
        XREF    U_LOAD

; RECONSTRUCT A DOUBLE PRECISION NUMBER FROM A RECORD CONTAINING
;   ITS PIECES.

; INPUT
;       ADDRESS OF WHERE TO PUT ANSWER
;       ADDRESS OF RECORD
; REGISTERS: A0, D0-D4 DESTROYED
; USAGE:
;       D2      UPPER
;       D3      LOWER                  
;       D6      EXPONENT               

; ERROR CONDITIONS
;   IF THE NUMBER IS TOO BIG FOR DOUBLE PRECISION,
;     INFINTY IS GENERATED.

PD_PACK
;------
        JSR     C_ADDR(U_LOAD)
        BSR.S   D_PACK
        MOVEM.L D0/D1,(A0)     ;STORE RESULT
        RTS

; INPUT:  D0-D3  UNPACKED RECORD
; OUTPUT: D0/D1  NUMBER
; REGISTERS DESTROYED:  D0-D4

D_PACK
;------                                             
        CMPI.B  #4,D3          ;TYPE         
        BLT.S   DPK0                   
        OR.L    D1,D0                                      ;PKNAN
        LSL.L   #1,D0                                      ;PKNAN
        ORI.L   #$FFE00000,D0
        BRA.S   DPKSGN         ;NAN OR INF    
DPK0
        ADDI.W  #1022+52+12,D2
        EXG     D0,D2          ;UPPER : EXP
        EXG     D0,D6          ;NOW D2=UPPER, D6=EXP, D6 SAVED IN D0
        EXG     D1,D3          ;LOWER : SIGN/TYPE
        BSR.S   D_NORM        ;NORMALIZE
        BSR     D_RCP                 
        MOVE.L  D0,D6          ;RESTORE D6
        MOVE.L  D2,D0
        EXG     D3,D1
DPKSGN  LSL.W   #1,D3          ;SIGN
        ROXR.L  #1,D0          ;APPEND SIGN   
        RTS
        PAGE    ;---------------
        XDEF    D_EXTE
; EXTRACT EXPONENTS FROM 2 DOUBLE PRECISION NUMBERS.

; INPUT
;       D0/D1   ONE OPERAND
;       D2/D3   OTHER OPERAND

; OUTPUT
;       D0/D1   MANTISSA, WAITING FOR HIDDEN BIT TO BE TURNED ON
;       D2/D3   OTHER MANTISSA
;       D6      EXPONENT CORRESPONDING TO D2/D3
;       D7      EXPONENT CORRESPONDING TO D0/D1

; DESTROYS D4

D_EXTE
;------
        MOVEQ   #11,D4         ;SIZE OF EXPONENT
; TRANSFORM EACH FROM          ; EU0 ML  IN D2/D3 (AND D0/D1)
        ROL.L   D4,D0
        ROL.L   D4,D2          ; U0E
        ROL.L   D4,D1
        ROL.L   D4,D3          ;     LM
        MOVE.L  #$7FF,D6       ;EXPONENT-SIZED MASK
        MOVE.L  D6,D7
        AND.L   D2,D6          ; 00E     EXPONENT
        EOR.L   D6,D2          ; U00
        MOVE.L  D7,D4
        AND.L   D3,D4          ; 00M     CARRY
        EOR.L   D4,D3          ;     L0
        LSR.L   #1,D2          ; 0U0
        OR.L    D4,D2          ; 0UM
; END OF TRANSFORM OF LARGER   ; 0UM L0  IN D2/D3, PLUS EXP IN D6
        MOVE.L  D7,D4
        AND.L   D0,D7          ; 00E
        EOR.L   D7,D0          ; U00
        AND.L   D1,D4          ; 00M
        EOR.L   D4,D1          ;     L0
        LSR.L   #1,D0          ; 0U0
        OR.L    D4,D0          ; 0UM
; END OF TRANSFORM OF SMALLER  ; 0UM L0  IN D0/D1, EXP IN D7
        RTS
        PAGE    ;--------------
        XDEF    D_NORM
; NORMALIZE A DOUBLE PRECISION NUMBER.

; INPUT
;       D2/D3   MANTISSA
;       D6      EXPONENT

D_NORM
;------ 
        TST.L   D2
        BNE.S   NORM1          ;UPPER IS NON-ZERO          ;GUBUG
        CMPI.W  #32,D6
        BLT.S   NORM           ;ABOUT TO BE DENORMALIZED
        SUBI.W  #32,D6
        EXG     D3,D2          ;SHIFT 32
        TST.L   D2
        BEQ.S   CANCELN        ;IF RESULT = 0
NORM1   BMI.S   NORMXIT        ;IF ALREADY NORMALIZED      ;GUBUG
NORM    LSL.L   #1,D3          ;NORMALIZE
        ROXL.L  #1,D2
        DBMI    D6,NORM        ;LOOP UNTIL NORMALIZED
        DBPL    D6,NORMXIT     ;MAKE SURE D6 DECREMENTED   ;GUBUG
NORMXIT RTS

CANCELN MOVE.W  #-2222,D6      ;EXP=0 FOR ZERO
        RTS
        PAGE    ;----------------------
        XDEF    D_NRCP,D_RCP,D_USEL  
; ROUND, CHECK FOR OVER/UNDERFLOW, AND PACK IN THE EXPONENT.

; D_NRCP DOES ONE NORMALIZE AND THEN CALLS D_RCP.
; D_RCP ROUNDS THE DOUBLE VALUE AND PACKS THE EXPONENT IN,
;   CATCHING INFINITY, ZERO, AND DENORMALIZED NUMBERS.
; D_USEL PUTS TOGETHER THE LARGER ARGUMENT.

; INPUT:
;       D2/D3   MANTISSA (- IF NORM)   
;       D6      BIASED EXPONENT        
;       (NEED SIGN, STICKY)            
; OUTPUT:
;       D2/D3   MOST OF NUMBER,        
;               NO SIGN OR HIDDEN BIT,
;               WAITING TO SHIFT SIGN IN.
; OTHER:
;       D4      LOST
;       D5      UNCHANGED

D_NRCP: 
;------
        TST.L   D2
        BMI.S   D_RCP         ;ALREADY NORMALIZED
        SUBQ.W  #1,D6
        LSL.L   #1,D3          ;DO EXTRA NORMALIZE (FOR MUL/DIV)
        ROXL.L  #1,D2

D_RCP:
;------
        TST.W   D6
        BGT.S   DRCPOK
; EXPONENT IS NEG; DENORMALIZE BEFORE ROUNDING
        CMP.W   #-53,D6
        BLT.S   SIGNED0        ;GO ALL THE WAY TO ZERO
        NEG.W   D6
DENL    LSR.L   #1,D2          ;DENORMALIZE
        ROXR.L  #1,D3
        DBRA    D6,DENL        ;DECR EXPONENT AND LOOP
        CLR.W   D6
; ROUND
DRCPOK  ADDI.L  #$400,D3       ;CRUDE ROUND             
	BCC.S	DRCP1	       ;RND DIDNT OFL
        ADDQ.L  #1,D2          ;CARRY
        BCC.S   DRCP1
        ROXR.L  #1,D2
        ROXR.L  #1,D3
        ADDQ.W  #1,D6
DRCP1   CMPI.W  #$7FF,D6
        BGE.S   DRCPBIG        ;EXP TOO BIG
D_USEL:
;------
; REBUILD ANSWER         D2/D3 = HUM LX  H=HIDDEN, U=UPPER, M=MIDDLE
                               ;          L=LOWER, X=TRASH
USEL    MOVE.L  #$FFFFF800,D4  ;         MASK
        AND.L   D4,D3          ;     L0
        AND.L   D2,D4          ; HU0
        EOR.L   D4,D2          ;     0M  PART TO MOVE
        OR.L    D2,D3          ;     LM
        MOVE.L  D4,D2          ; HU0
        LSL.L   #1,D2          ; U00     TOSS HIDDEN BIT (IF ANY)
        BCS.S   DRA            ;         HIDDEN            ;NORM
        CMP.W   #$7FF,D6                                   ;NORM
        BEQ.S   DRA            ;         INF/NAN           ;NORM
        CLR.W   D6             ;         ASSUME 0 IF NO HID;NORM
DRA     MOVEQ   #11,D4
        ROR.L   D4,D3          ;     ML
        OR.W    D6,D2          ; U0E     PUT IN EXPONENT
        ROR.L   D4,D2          ; EU0
        RTS                            
                                                           ;NORM
SIGNED0 CLR.L   D2                     
        CLR.L   D3
        RTS                            

DRCPBIG MOVE.L  #$FFE00000,D2  ;INFINITY      
        CLR.L   D3
        RTS                            
        PAGE    ;-------------------
        XDEF    D_SIGN

; TRANSFER THE SIGN OF THE SECOND ARGUMENT TO THE FIRST.
; PASCAL CALLABLE.

D_SIGN:
;------
        ROXL    12(SP)         ;KILL SIGN IN ARG1
        ROXL    4(SP)          ;GET SIGN OF SECOND
        ROXR    12(SP)         ;PUT SIGN ON
        MOVE.L  16(SP),24(SP)  ;RETURN VALUE
        MOVE.L  12(SP),20(SP)
        MOVE.L  (SP)+,12(SP)   ;RETURN ADDR
        ADDA.W  #12,SP
        RTS

        END

#pragma	endasm
 
#endif /* EXCLUDE_dpk */
 

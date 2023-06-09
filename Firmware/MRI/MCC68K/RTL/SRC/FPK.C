#include "lib_top.h"


#if EXCLUDE_fpk
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
 
; COPYRIGHT 1981, 1982 RICHARD E. JAMES III
*************************************************************************/
** THIS INFORMATION IS PROPRIETARY TO					*/
** MICROTEC RESEARCH, INC.						*/
**----------------------------------------------------------------------*/
** Copyright (c) 1991 Microtec Research, Inc.				*/
** All rights reserved							*/
*************************************************************************/
;The software source code contained in this file and in the related
;header files is the property of Microtec Research, Inc. and may only be
;used under the terms and conditions defined in the Microtec Research
;license agreement. You may modify this code as needed but you must retain
;the Microtec Research, Inc. copyright notices, including this statement.
;Some restrictions may apply when using this software with non-Microtec
;Research software.  In all cases, Microtec Research, Inc. reserves all rights.

; 9/12/90 jpc: allow use of long disp. in pc rel. addr.

; THIS MODULE CONTAINS SEVERAL SINGLE
; PRECISION UTILITIES FOR FORTRAN INTRINSICS.
*
* %W% %G% *
*

; FORMAT OF UNPACKED RECORD:
;     RECORD
;       D0/D1   +0/+4   MANTISSA: ARRAY[2]OF INT; (UPPER/LOWER)
;       D2.W    +8      EXPONENT: -32768..32767;
;       D3.W    +10     SIGN:     (IN SIGN BIT);
;       D3.B    +11     TYPE:     1..5               
;     END;

        PAGE    ;----------------
        section code
        XDEF  PF_UNPK,F_UNPK
        XDEF  U_STOR

; SPLIT A SINGLE PREC. NUMBER INTO ITS SIGN, EXPONENT, AND MANTISSA.
 
; INPUT:
;       FLOATING VALUE
;       ADDR OF RECORD
; REGISTERS: A0,D0,D1 DESTROYED.

PF_UNPK
;------
        MOVE.L  8(SP),D1       ;NUMBER
        BSR.S   F_UNPK

U_STOR                        ;JUMPED TO
;------
        MOVE.L  4(SP),A0       ;RECORD ADDR
        SWAP    D2
        MOVE.W  D3,D2
        MOVEM.L D0-D2,(A0)     ;STORE RECORD
        MOVE.L  (SP)+,A0       ;RETURN ADDR
        ADDQ.L  #8,SP
        JMP     (A0)           ;RETURN

F_UNPK
;------
        MOVE.L  D1,D3
        SWAP    D3             ;SIGN IN BIT 15
        LSL.L   #1,D1          ;TOSS SIGN
        ROL.L   #8,D1
        CLR.W   D2
        MOVE.B  D1,D2          ;EXPONENT
        BNE.S   UNP2           ;NOT 0/GU        
        MOVE.B  #1,D3          ;--ZERO   
        TST.L   D1                     
        BEQ.S   UNP8                   
        MOVE.B  #2,D3          ;--DENORMALIZED     
        BRA.S   UNP8
UNP2    CMP.B   #255,D2        ;INF/NAN       
        BNE.S   UNP3           ;PLAIN
        MOVE.W  #$6000,D2       
        CLR.B   D1             ;ERASE EXP
        MOVE.B  #4,D3          ;--INF
        TST.L   D1                     
        BEQ.S   UNP9                   
        MOVE.B  #5,D3          ;--NAN    
        BRA.S   UNP9                   
UNP3    MOVE.B  #1,D1          ;HIDDEN BIT
        SUBQ.W  #1,D2                  
        MOVE.B  #3,D3          ;--NUM    
UNP8    SUBI.W  #126+23,D2
UNP9    ROR.L   #1,D1
        LSR.L   #8,D1
        CLR.L   D0             ;UPPER
        RTS
        PAGE    ;---------------
        XDEF  PF_PACK,F_PACK
        XDEF  U_LOAD

; RECONSTRUCT A SINGLE PRECISION NUMBER FROM A RECORD CONTAINING
; ITS PIECES.

; INPUT  (PF_... ENTRIES)
;       ADDRESS OF WHERE TO PUT ANSWER
;       ADDRESS OF RECORD
; REGISTERS: A0, D0-D3 DESTROYED
; USAGE:
;       D0      UPPER/ANSWER           
;       D1      LOWER                  
;       D2      RESERVED *S
;       D3      SIGN AND TYPE
;       D4      EXPONENT               

; ERROR CONDITIONS
;   IF THE NUMBER IS TOO BIG FOR 
;   SINGLE PRECISION, INFINTY IS GENERATED.

U_LOAD
;------
        MOVE.L  2*4(SP),A0
        MOVEM.L (A0),D0-D2
        MOVE.W  D2,D3          ;SIGN, TYPE
        SWAP    D2             ;EXPONENT
        MOVE.L  3*4(SP),A0     ;ADDR FOR RESULT
        RTS

PF_PACK
;------
        BSR.S   U_LOAD
        BSR.S   F_PACK
        MOVE.L  D1,(A0)        ;STORE RESULT
        MOVE.L  (SP)+,A0
        ADDQ.L  #8,SP
        JMP     (A0)           ;RETURN

F_PACK
;------
        MOVE.W  D2,D4          ;EXP
        CMPI.B  #4,D3          ;TYPE : INF/NAN
        BLT.S   SPK0                   
        OR.L    D0,D1          ;NAN CODE, OR 0 FOR INF     ;PKNAN
        ORI.L   #$7F800000,D1  ;EXP FOR NAN/INF            ;PKNAN
        LSL.L   #1,D1                                      ;PKNAN
        BRA.S   SPKSGN
SPK0
;       CLR.B   D2             ;FOR STICKY
        TST.L   D0
        BEQ.S   SPK2
; SHIFT FROM UPPER INTO LOWER
SPK1    
;       OR.B    D1,D2          ;STICKY
        MOVE.B  D0,D1
        ADDQ.W  #8,D4          ;ADJ EXP
        ROR.L   #8,D1
        LSR.L   #8,D0
        BNE.S   SPK1           ;LOOP TIL TOP=0
SPK2    MOVE.L  D1,D0
        BEQ.S   SPKSGN 
; FIND TOP BIT
        BMI.S   SPK5           ;IF ALREADY NRM
SPK4    SUBQ.W  #1,D4          ;ADJ EXP       
        LSL.L   #1,D0          ;NORMALIZE     
        BPL.S   SPK4
SPK5    ADDI.W  #126+23+9,D4        
        BSR.S   F_RCP                 
        ROR.L   #8,D0
        MOVE.L  D0,D1
SPKSGN  LSL.W   #1,D3
        ROXR.L  #1,D1          ;APPEND SIGN   
        RTS


        XDEF  F_SIGN

; INPUT:
;       SPACE FOR RETURN VALUE
;       ARG TO HAVE SIGN APPLIED TO 
;       ARG WITH SIGN
;       RETURN ADDR

F_SIGN
;------
        ROXL    8(SP)          ;KILL SIGN IN ARG1
        ROXL    4(SP)          ;GET SIGN OF SECOND
        ROXR    8(SP)          ;PUT SIGN ON
        MOVE.L  8(SP),12(SP)   ;RETURN VALUE
        MOVE.L  (SP)+,(SP)     ;RETURN ADDR
        MOVE.L  (SP)+,(SP)     ;RETURN ADDR
        RTS
        PAGE    ;-----------
        XDEF  F_RCP                 
; ROUND, CHECK FOR OVER/UNDERFLOW, AND PACK IN THE EXPONENT.

; INPUT:
;       D0      MANTISSA (- IF NORM)  *S
;       D4      BIASED EXPONENT        
; OUTPUT:
;       D0      MOST OF NUMBER, NO SIGN OR HIDDEN BIT,
;               EXP IS IN BOTTOM.      

F_RCP
;------
        TST.L   D0
        BMI.S   RCP1
; DO EXTRA NORMALIZE (FOR MUL/DIV)
        SUBQ.W  #1,D4
        LSL.L   #1,D0          ;DO ONE NORMALIZE
RCP1
        TST.W   D4
        BGT.S   RCP2
; UNDERFLOW
        CMPI.W  #-24,D4                
        BLT.S   SIGNED0                
        NEG.B   D4             
        ADDQ.B  #1,D4                  
        LSR.L   D4,D0          ;DENORMALIZE   
        CLR.W   D4             ;EXP=0
RCP2
        ADDI.L  #$80,D0        ;CRUDE ROUND   
        BCC.S   RCP4                   
; ROUND OVERFLOWED
        ROXR.L  #1,D0
        ADDQ.W  #1,D4          ;ADJ EXP       
RCP4
        CMPI.W  #$FF,D4                
        BGE.S   RCPBIG                 
        LSL.L   #1,D0          ;TOSS HIDDEN   
        SCS     D0             ;NO HIDDEN IMPLIES 0/DENORM ;NORM
        AND.B   D4,D0          ;                           ;NORM
        RTS                            
                                                           ;NORM
SIGNED0 CLR.L   D0                     
        RTS                            

RCPBIG  MOVE.L  #$FF,D0        ;INFINITY      
        RTS                            
 

 
#pragma endasm
 
#endif /* EXCLUDE_fpk.c */
 

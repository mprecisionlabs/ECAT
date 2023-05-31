#include "lib_top.h"


#if EXCLUDE_strtok
    #pragma option -Qws			/* Prevent compiler warning */
#else
 
_kill_warning();	/* Eliminates compiler warnings */

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
*
*       File name:              strtok.a86
*       Author:                 Herbert Yuen
*       Date:                   85-Jun-12 Wednesday
*
* %W% %G% *
*
*       revision history:       
*       5-16-86 ling: using pc relative
*	9/13/90 jpc : quick mod to allow 32 bit displacements on 020
*	1/17/94 jpc : Improved macro definitions to function better with
*		      library customizer.
*==============================================================================
*
*                       s t r t o k
*
*==============================================================================
* char  *strtok (S1,S2)
*
*       strtok considers the string S1 to consist of a sequence of zero or
*       more text tokens separated by spans of one or more characters from
*       the separator string S2.  The first call (with S1 specified)
*       returns a pointer to the first character of the first token, and
*       will have written a null character into S1 immediately following
*       the returned token.  The function keeps track of its position in
*       the string between separate calls, so that on subsequent calls
*       (with the first argument a NULL pointer) will work through the
*       string S1 immediately following that token until no tokens remain.
*       The separator string S2 may be different from call to call.  When
*       no token remains in S1, a NULL pointer is returned.
*
*/


#pragma option -nKf	/* LINK/UNLK will cause code to malfunction */

#pragma	asm

	SECTION	code,,C

        XDEF    _strtok
*
_strtok:
        movem.l d2/d3/a0/a1/a2,-(sp)    ; Save D2,D3,A0,A1,A2 on stack
        moveq   #0,d0                   ; Clear D0 as NULLPTR
        move.l  24(a7),a0
        tst.l   24(a7)
        bne.s   begin
        move.l  D_ADDR(__lastp),a0      ; S1
	move.l	a0,d1			; move for test
        tst.l   d1			; test
        bne.s   begin
        movem.l (sp)+,d2/d3/a0/a1/a2    ; Restore D2,D3,A0,A1,A2
        rts                             ; Return NULLPTR if lastp(a5) is NULLPTR
begin:
        move.l  28(a7),a1               ; S2
        move.l  a1,d1                   ; Save a copy of A1 in D1
        move.l  d1,d2
        move.l  d0,a2                   ; A2 = NULLPTR for no token found
lp0:
        tst.b   (a1)+                   ; Find end of S2
        bne     lp0
        sub.l   a1,d2                   ; -(length of S2, including 0)
        not.l   d2                      ; D2 = length of S, excluding 0
lp1:
        move.b  (a0)+,d0                ; Get one char from S1
        beq.s   done                    ; Return NULLPTR if end of S1
        move.l  d1,a1                   ; A1 -> S2
        move.l  d2,d3                   ; D3 = length of S2
lp1a:
        cmp.b   (a1)+,d0                ; *S2++ == Chr ?
        dbeq    d3,lp1a                 ; Repeat until found or end of S2
        beq     lp1                     ; Continue to skip chars from S2
        move.l  a0,a2                   ; A2 = ptr to token + 1
        subq.l  #1,a2                   ; A2 = ptr to token
lp2:
        move.b  (a0)+,d0                ; Get one char from S1
        beq.s   done                    ; If end of S1, 'tis the last token
        move.l  d1,a1                   ; A1 -> S2
        move.l  d2,d3                   ; D3 = length of S2
lp2a:
        cmp.b   (a1)+,d0                ; *S2++ == Chr ?
        dbeq    d3,lp2a                 ; Repeat until found or end of S2
        bne     lp2                     ; Continue to scan chars in token
*
        move.l  a0,d0                   ; Ptr for next call
        clr.b   -(a0)                   ; Write a NULL after token
done:
        lea     D_ADDR(__lastp),a0      ; ling: using a5 relative
        move.l  d0,(a0)                 ; orig: move.l  d0,(__lastp).w(a5)
                                        ;       Remember the ptr for next call
        move.l  a2,d0                   ; Return ptr to token in D0
        movem.l (sp)+,d2/d3/a0/a1/a2 ; Restore D2,D3,A0,A1,A2
        rts
*
        SECTION zerovars,,D
        XREF    __lastp                ; Static data to remember the ptr
*

#pragma	endasm
 
#endif /* EXCLUDE_strtok */
 

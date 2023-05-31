#include "lib_top.h"


#if EXCLUDE_msg_syms
    #pragma option -Qws			/* Prevent compiler warning */
#else
 
#pragma option -nf			/* override -f option; -f will */
					/* cause "xref ____FPU which will */
					/* cause problems in assembler. */

_kill_warning();			/* Eliminates compiler warnings */

/*
; ----------------------------------------------------------------------------
; Module: msg_syms.c - Defines public symbols to resolve messenger symbols.
;
; Creation Date: 09Oct92
;
; Author: Cooper G. Urie
;
; THIS INFORMATION IS PROPRIETARY TO
; MICROTEC RESEARCH, INC.
; 2350 Mission College Blvd.
; Santa Clara, CA 95054
; USA
; ----------------------------------------------------------------------------
; Copyright (c) 1992  Microtec Research, Inc.
; All Rights Reserved
; ----------------------------------------------------------------------------
;
;The software source code contained in this file and in the related
;header files is the property of Microtec Research, Inc. and may only be
;used under the terms and conditions defined in the Microtec Research
;license agreement. You may modify this code as needed but you must retain
;the Microtec Research, Inc. copyright notices, including this statement.
;Some restrictions may apply when using this software with non-Microtec
;Research software.  In all cases, Microtec Research, Inc. reserves all rights.
;
; Revisions: (latest revision on top)
; Name Date     Description
; ---- -------	-------------------------------------------------------
; cgu  09Oct92	Creation date.
; jpc  07Jan94  eliminate "extern ___FPU;" add "#pragma option -nf"
; ----------------------------------------------------------------------------
;
; Description:  The compiler will generate an external reference that will be
; resolved by the definitions in this file.  An instance of a messenger symbol
; will tell XRAY that the application was built will a given option or feature.
;
; ----------------------------------------------------------------------------
*/
/*	%W% %G%	*/

#pragma asm 

       xdef ____FPU
____FPU  equ  0

#pragma endasm

#endif /* EXCLUDE_msg_syms */
 

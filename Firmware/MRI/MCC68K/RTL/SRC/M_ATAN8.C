#include "lib_top.h"


#if EXCLUDE_m_atan8 || (! _FPU)
    #pragma option -Qws			/* Prevent compiler warning */
#else
 
_kill_warning();	/* Eliminates compiler warnings */

/*
; ----------------------------------------------------------------------------
; Module: m_atan8.c - FPU atan library function.
;
; Creation Date: 07Oct92
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
; cgu  07Oct92	Creation date.
; jpc  12/06/93	Modified to be used w. -Kq
; ----------------------------------------------------------------------------
;
; Description:
;
; ----------------------------------------------------------------------------
*/
/*	%W% %G%	*/

#pragma option -f

#include <errno.h>
#include <math.h>
#include "mri_fpu.h"

double atan (double arg)
{
register unsigned long ESB_bits;

#if __OPTION_VALUE ("-Kq")
    asm( "	fatan.s `arg`,fp0 ");
#else
    asm( "	fatan.d `arg`,fp0 ");
#endif

asm( "	fmove FPSR,`ESB_bits` ");
asm( " 	and.l #$0FF00,`ESB_bits` ");

if (ESB_bits & OPERR)
   errno = EDOM;
else 
   if (ESB_bits & OVFL_DZ)
      errno = ERANGE;

return;
}

#endif /* EXCLUDE_m_atan8 */
 

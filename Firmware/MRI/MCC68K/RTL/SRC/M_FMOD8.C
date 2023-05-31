#include "lib_top.h"


#if EXCLUDE_m_fmod8 || (! _FPU)
    #pragma option -Qws			/* Prevent compiler warning */
#else
 
_kill_warning();	/* Eliminates compiler warnings */

/*
;*************************************************************************
;* THIS INFORMATION IS PROPRIETARY TO					**
;* MICROTEC RESEARCH, INC.						**
;*----------------------------------------------------------------------**
;* Copyright (c) 1991 Microtec Research, Inc.				**
;* All rights reserved							**
;*************************************************************************
;
;The software source code contained in this file and in the related
;header files is the property of Microtec Research, Inc. and may only be
;used under the terms and conditions defined in the Microtec Research
;license agreement. You may modify this code as needed but you must retain
;the Microtec Research, Inc. copyright notices, including this statement.
;Some restrictions may apply when using this software with non-Microtec
;Research software.  In all cases, Microtec Research, Inc. reserves all rights.
;
;***********************************************************************;
;									;
;	fmod881								;
;		interrim routine to be used until the fmod funtion	;
;		is expanded by the codegen in-line.  This routine 	;
;		assumes the presence of an '881 and simply executes	;
;		an fmod instruction.					;
;									;
;	History								;
;	11/14/89 jpc:	Created						;
;       10/09/92 cgu:   Added errno setting and use of backquote 	;
;  		     	variables.					;
; 	12/06/93 jpc	Modified to be used w. -Kq
;									;
;***********************************************************************;
;	%W% %G%	;
*/

#pragma option -f

#include <errno.h>
#include <math.h>
#include "mri_fpu.h"

double fmod (double input1, double input2)
{
register unsigned long ESB_bits;

#if __OPTION_VALUE ("-Kq")
    asm ("	fmove.s	`input1`,fp0");	/* place first argument in fp register*/
    asm ("	fmod.s	`input2`,fp0");	/* perform fmod function */
#else
    asm ("	fmove.d	`input1`,fp0");	/* place first argument in fp register*/
    asm ("	fmod.d	`input2`,fp0");	/* perform fmod function */
#endif

if (ESB_bits & OPERR)
   errno = EDOM;
else
   if (ESB_bits & OVFL_DZ)
      errno = ERANGE;

return;
}
 
#endif /* EXCLUDE_m_fmod8 */
 

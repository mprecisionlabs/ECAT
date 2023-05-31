/*
; ----------------------------------------------------------------------------
; Header: mri_fpu.h - Macros used in FPU trig functions for checking FPSR
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
; ----------------------------------------------------------------------------
;
; Description: These Macros masks are used to chech the Exception Status Byte
; portion of the FPSR after a floating point instruction.
; ----------------------------------------------------------------------------
*/
/*	%W% %G%	*/


#define OPERR	0x2000		/* illegal operand for setting EDOM */
#define OVFL_DZ 0x1400		/* this includes overflow and divide by zero */
		        	/* for setting ERANGE */

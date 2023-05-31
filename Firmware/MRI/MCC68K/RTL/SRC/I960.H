/*
; ----------------------------------------------------------------------------
; Header: i960.h - Dummy header file to keep "gen_make" happy.
;
; THIS INFORMATION IS PROPRIETARY TO
; MICROTEC RESEARCH, INC.
; 2350 Mission College Blvd.
; Santa Clara, CA 95054
; USA
; ----------------------------------------------------------------------------
; Copyright (c) 1993  Microtec Research, Inc.
; All Rights Reserved
; ----------------------------------------------------------------------------
;The software source code contained in this file and in the related
;header files is the property of Microtec Research, Inc. and may only be
;used under the terms and conditions defined in the Microtec Research
;license agreement. You may modify this code as needed but you must retain
;the Microtec Research, Inc. copyright notices, including this statement.
;Some restrictions may apply when using this software with non-Microtec
;Research software.  In all cases, Microtec Research, Inc. reserves all rights.
;
; Description:	Because csys.c contains a "#include i960.h" statement, 
;		gen_make will always try to find "i960.h" and will issue a
;		warning when it cannot.  This file suppresses that warning.
;		This file is also necessary to build libraries using a
;		makefile.  For that reason, this file should not be removed.
;
; ----------------------------------------------------------------------------
*/



;************************************************************************
;* THIS INFORMATION IS PROPRIETARY TO					*
;* MICROTEC RESEARCH, INC.						*
;* 2350 Mission College Blvd.						*
;* Santa Clara, CA 95054						*
;* USA									*
;*----------------------------------------------------------------------*
;* Copyright (c) 1993  Microtec Research, Inc.				*
;* ALL RIGHTS RESERVED							*
;************************************************************************
;The software source code contained in this file and in the related
;header files is the property of Microtec Research, Inc. and may only be
;used under the terms and conditions defined in the Microtec Research
;license agreement. You may modify this code as needed but you must retain
;the Microtec Research, Inc. copyright notices, including this statement.
;Some restrictions may apply when using this software with non-Microtec
;Research software.  In all cases, Microtec Research, Inc. reserves all rights.
;
;
;
; 		MCC68K Default Linker Command File
;
listabs	publics,internals
listmap	publics
format	ieee
extern	ENTRY			; force load of initialization routine
;
; If linking with MCC68K 3.x objs, enable the next 4 lines:
;
;   alias code,9		; 9  was default value of $CODESEG in 3.x
;   alias zerovars,14		; 14 was default value of $DATASEG in 3.x
;   alias vars,13		; 13 was default value of $INITSEG in 3.x
;   alias heap,15		; 15 was heap section in 3.x
;
sectsize	heap=$8000	; set size of heap
sectsize	stack=$1000	; set size of stack
common		stack=$f000	; set starting address of stack section
alignmod	ioports=4	; adjust alignment of ioports to long word
initdata	vars, tags	; initialize vars and tags sections
;
order	stack					; RAM -- stack section
order   ehscfn,ehsftbl				; ROM -- ehs sections for C++
order	initfini,pixinit			; ROM -- other C++ sections
order	literals,strings,const,code		; ROM -- C sections
order	??INITDATA				; ROM -- initialization values
order	vars,zerovars,tags,ioports,heap		; RAM -- C sections
;
; In the following linker commands, "<lib_dir>" should be set to
; "/usr/mri/lib/" for UNIX and "C:\mcc68k\680XX\" for MSDOS.
;
; If using A5-relative data addressing, enable the next 2 lines:
;
; index	?a5,vars,$8000		      ; A5 reg must be set to ?A5 in entry.c
; load  <lib_dir>mcc68ka5XX.lib	      ; Modify to access correct library

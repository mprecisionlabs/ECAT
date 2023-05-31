#include "lib_top.h"


#if EXCLUDE_outchr
    #pragma option -Qws			/* Prevent compiler warning */
#else
 
/************************************************************************/
/* THIS INFORMATION IS PROPRIETARY TO					*/
/* MICROTEC RESEARCH, INC.						*/
/* 2350 Mission College Blvd.						*/
/* Santa Clara, CA 95054						*/
/* USA									*/
/*----------------------------------------------------------------------*/
/* Copyright (c) Microtec Research, Inc. 1986-89, 1991.			*/
/* All rights reserved.							*/
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


/************************************************************************
*									*
*  char *_OUTCHR(c)							*
*									*
*   outchr output one char to _simulated_output				*
*									*
*	Memory mapped I/O: Write to the variable "_simulated_output."	*
*	The debugger is sensitive to the name of this variable, and will*
*	automatically issue this command upon startup:			*
*									*
*		OUTPORT &_simulated_output, STD.			*
*									*
*       MONITOR_WRITE_SYMBOL is used by the debugger to aid in          *
*       the placing of a breakpoint to allow "outport" commands to      *
*       function in emulator and monitor environments.                  *
*									*
************************************************************************/
/*	%W% %G%	*/

#include "s_info.mch"

volatile extern unsigned char _simulated_output;

_OUTCHR(c)
char c;
{
      _simulated_output = c;
      _MONITOR_WRITE_SYMBOL;
}
 
#endif /* EXCLUDE_outchr */
 

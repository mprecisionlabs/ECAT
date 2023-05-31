#include "lib_top.h"


#if EXCLUDE_inchrw
    #pragma option -Qws			/* Prevent compiler warning */
#else
 

/************************************************************************/
/* THIS INFORMATION IS PROPRIETARY TO					*/
/* MICROTEC RESEARCH, INC.						*/
/* 2350 Mission College Blvd.						*/
/* Santa Clara, CA 95054						*/
/* USA									*/
/*----------------------------------------------------------------------*/
/* Copyright (c) Microtec Research, Inc. 1986-91. All rights reserved. 	*/
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
*  char *_INCHRW()							*
*									*
*  inchrw read in one char from _simulated_input			*
*									*
*       Read from the variable "_simulated_input."  The debugger is     *
*       sensitive to the name of this variable, and will automatically  *
*       issue this command upon startup:                                *
*                                                                       *
*              INPORT &_simulated_input, STD.                           *
*                                                                       *
*	_INCHRW should return (int) -1 on error.			*
*									*
*       MONITOR_READ_SYMBOL is used by the debugger to aid in           *
*       the placing of a breakpoint to allow "inport" commands to       *
*       function in emulator and monitor environments.                  *
*									*
************************************************************************/
/*	%W% %G%	*/

#include "s_info.mch"

volatile extern unsigned char _simulated_input;

int _INCHRW()
{
      _MONITOR_READ_SYMBOL;
      return (_simulated_input);
}
 
#endif /* EXCLUDE_inchrw */
 

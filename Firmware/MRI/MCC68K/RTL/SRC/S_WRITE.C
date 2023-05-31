#include "lib_top.h"


#if EXCLUDE_s_write
    #pragma option -Qws			/* Prevent compiler warning */
#else
 

/************************************************************************/
/* THIS INFORMATION IS PROPRIETARY TO					*/
/* MICROTEC RESEARCH, INC.						*/
/* 2350 Mission College Blvd.						*/
/* Santa Clara, CA 95054						*/
/* USA									*/
/*----------------------------------------------------------------------*/
/* Copyright (c) Microtec Research, Inc. 1986-91, 1992.			*/
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


/*	%W% %G%	*/

/************************************************************************
*                                                                       *
*  int write (int fd, char *buffer, unsigned nbyte)			*
*                                                                       *
*  write characters to output device					*
*									*
*  write:								*
*	Write "nbyte" characters to output device indicated by the	*
*	file descriptor "fd".  The characters to be written are in	*
*	"buffer".  "fd" is the file descriptor of the device being	*
*	written to.							*
*                                                                       *
*	"Write" returns -1 on error.					*
*                                                                       *
*       This version of write is designed to be used with a debugger	*
*       by writing characters to the variable _simulated_output.  The   *
*       debugger is sensitive to the name of the variable, and will     *
*       automatically issue this command upon startup:                  *
*                                                                       *
*               OUTPORT &_simulated_output,STD.                         *
*                                                                       *
*       A breakpoint should be set to allow the OUTPORT command to work *
*       in emulator and monitor environments:                           *
*                                                                       *
*		bi __sim_out_break; \					*
*			outport(&_simulated_output,1,_simulated_output)	*
*									*
************************************************************************/

#include <stdio.h>
#include <errno.h>
 
/************************************************************************
*       eoln is used to indicate the end of line characters for the     *
*       host system.                                                    *
*                                                                       *
*       eoln =  0       carriage return (13) as end of line, ex :       *
*                       Microtec 68000 simulator                        *
*            =  1       carriage return (13) followed by line feed      *
*                       (10) as end of line, ex : MSDOS, debugger       *
*            =  2       line feed (10) as end of line, ex : UNIX        *
*                                                                       *
************************************************************************/
 
#define eoln 2
 
#ifdef _MCC68K
	/* No code inserted here */
#else

    #include "s_info.mch"

    #ifdef _MCC386
	#pragma options -NZioports

	volatile char _simulated_output;	/* for simulator's output */
    #else
	volatile extern unsigned char _simulated_output;
    #endif

    _OUTCHR(c)
    char c;
    {
	_simulated_output = c;
	_MONITOR_WRITE_SYMBOL;
    }

#endif

int write (int fd, char *buffer, unsigned nbyte)
{
    register int cnt= 0;
 
    while (--nbyte != -1) {
#if   eoln == 0
        if (*buffer == '\n') *buffer = '\r';		/* replace by '\r' */
#elif eoln == 1
        if (*buffer == '\n') _OUTCHR('\r');		/* output a '\r' first*/
#endif
        _OUTCHR (*(buffer++));              		/* output one char */
        cnt++;
    }   
    return (cnt);
}
 
#endif /* EXCLUDE_s_write */
 

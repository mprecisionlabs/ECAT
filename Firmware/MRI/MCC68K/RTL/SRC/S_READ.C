#include "lib_top.h"


#if EXCLUDE_s_read
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
*  int read (int fd, char *buffer, unsigned nbyte)			*
*                                                                       *
*  read characters from input device					*
*									*
*  read:								*
*	Read "nbyte" characters from the file indicated by the 		*
*	file descriptor "fd".  These characters are placed in the	*
*	buffer pointed to by "buffer".					*
*                                                                       *
*	"Read" returns the number of bytes actually read.  If input	*
*	is occurring from a terminal, "read" will read at most one line	*
*	of input.  If an error occurs "read" returns a -1.  If end of	*
*	file occurs, "read" returns 0.					*
*                                                                       *
*       This version of read is designed to be used with a debugger	*
*       by reading characters from the variable _simulated_input.  The  *
*       debugger is sensitive to the name of this variable, and will    *
*       automatically issue this command upon startup:                  *
*                                                                       *
*              INPORT &_simulated_input,STD.                            *
*                                                                       *
*       A breakpoint should be set to allow the INPORT command to work  *
*       in emulator and monitor environments:                           *
*                                                                       *
*           bi __sim_in_break;inport(&_simulated_input,1)		*
*                                                                       *
************************************************************************/

#include <stdio.h>
#include <errno.h>

/************************************************************************
*									*
*	eoln is used to indicate the end of line characters for the	*
*	host system.							*
*									*
*	eoln =  0	carriage return (13) as end of line, ex :	*
*			Microtec 68000 simulator			*
*	     =  1	carriage return (13) followed by line feed	*
*			(10) as end of line, ex : MSDOS, debugger	*
*	     =  2	line feed (10) as end of line, ex : UNIX	*
*									*
*	This routine assumes that file descriptor 0 is receiving input	*
*	from a terminal and no others are, at least for input. This	*
*	assumption may not be valid in the actual application. Normally	*
*	the read function would be able to tell if it is communicating	*
*	with a terminal.  This particular implementation cannot.	*
*									*
*	The read is terminated when either "nbyte" characters have been	*
*	copied, or if fd == 0, when a '\r' or '\n' is read.		*
*									*
*	If you do not wish to have '\b' delete the previous character,	*
*	and translate '\r' to '\n', and echo input characters to the	*
*	"terminal" screen, define the preprocessor symbol		*
*	"EXCLUDE_TERMINAL_SIMULATION" before compiling this module.	*
*									*	
*	Note:  file descriptor 0 is generally stdin.			*
*									*
************************************************************************/

#define eoln 2	

#ifdef _MCC68K

    #define  GET	_INCHRW()
    #define  PUT(x)	_OUTCHR(x)

#else

    #include "s_info.mch"

    #ifdef _MCC386
	#pragma options -NZioports

	volatile char _simulated_input;		/* for simulator's input */
    #else
	volatile extern unsigned char _simulated_input;
    #endif

    #define  GET	(_MONITOR_READ_SYMBOL, _simulated_input)
    #define  PUT(x)	(*temp=(x), write (1, temp, 1))

#endif

#if EXCLUDE_TERMINAL_SIMULATION			/* Character echo only if */
						/* terminal simulation is */
    #undef	PUT				/* used */
    #define	PUT(x)	

#endif

int read (int fd, char *start_buffer, unsigned nbyte)
{
    register char *buffer = start_buffer;
    register int cnt=0, ch;
 
#if EXCLUDE_TERMINAL_SIMULATION
    #define line_flag	0			/* results in no code */
#else
    int	line_flag = 0;				/* flag indicating terminal */
						/* i/o and therefore line */
						/* buffering */
#if !_MCC68K
    char temp[] = " ";				/* temp buffer for "write" */
#endif

    if (fd == 0)
	line_flag = 1;
#endif

    while (--nbyte != -1) {
        if ((ch = GET) == -1) {    		/* read in one char */
            errno = EIO;
            return (-1);
        }   

	if (line_flag == 1) {			/* translate \r to \n for */
	    if (ch == '\r')			/* all terminals */
		ch = '\n';
	}

#if   eoln == 1
        if (ch == '\r') continue;		/* skip carriage return */
#elif eoln == 0
        if (ch == '\r') ch = '\n';		/* replace '\r' by '\n' */
#endif

	if (line_flag) {
	    if (ch == '\b') {
		if (buffer != start_buffer) {
		    PUT ('\b');			/* write out backspace */
		    buffer--;			/* back-up buffer */
		    nbyte += 2;			/* adjust nbyte */
		    cnt--;			/* adjust cnt */
		} else {
		    nbyte++;			/* cannot go past start of */
		}				/* buffer */
	    } else {
	        PUT (ch);			/* echo character */
	        *(buffer++) = ch;
	        cnt++;
	    }
        } else {
	    *(buffer++) = ch;			/* Normal file -- do not */
	    cnt++;				/* echo */
	}

        if ((fd == 0) && ((ch == '\n') || (ch == '\r')))
            break;                      /* LF or CR terminates the read */
    }                                   /* for file descriptor 0 */
    return (cnt);
}
 
#endif /* EXCLUDE_s_read */
 

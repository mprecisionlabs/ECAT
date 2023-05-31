#include "lib_top.h"


#if EXCLUDE_setvbuf
    #pragma option -Qws			/* Prevent compiler warning */
#else
 
/****************************************************************************/
/* THIS INFORMATION IS PROPRIETARY TO                                       */
/* MICROTEC RESEARCH, INC.                                                  */
/*--------------------------------------------------------------------------*/
/* Copyright (c) 1987, 1988, 1989, 1991, 1992, 1993 Microtec Research, Inc. */
/* All rights reserved                                                      */
/****************************************************************************/
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

/************************************************************************/
/* NAME                         S E T V B U F                           */
/*                                                                      */
/* FUNCTION     setvbuf (fp, buf, mode, size) - assign user buffer to	*/
/*			file fp						*/
/*                                                                      */
/* SYNOPSIS     void setvbuf (fp, buf, mode, size)			*/
/*              FILE*   fp - file that gets new buffer                  */
/*              char*   buf - pointer to new file buffer                */
/*		int	mode - buffer mode flag				*/
/*		size_t	size - size of buffer supplied by user		*/
/*                                                                      */
/* RETURN       zero on success -- non-zero on failure			*/
/*                                                                      */
/* DESCRIPTION  If the buffer is not NULL, it is assigned to the	*/
/*		specified file.  The parameter "size" is the size	*/
/*		of the user buffer.  The parameter mode is a flag	*/
/*		indicating the buffering mode				*/
/*                                                                      */
/* MODIFICATION HISTORY                                                 */
/*	01/13/94 jpc:	Mods for support of line buffering.		*/
/*	07/15/91 jpc:	Source stolen from setbuf as a start for 	*/
/*			setvbuf						*/
/*      09/14/89 jpc:   Added ability to convert to unbuffered i/o      */
/*			by setting _IONBF flag when buf is NULL		*/
/*             -M.H. change _flags to _flag 12/17/86                    */
/*      Author - wb denniston 09/30/86                                  */
/*                                                                      */
/*      MCC86 C Library  - Copyright 1987, Microtec Research, Inc.      */
/************************************************************************/
#include <stdio.h>
#include <stdlib.h>

#ifndef LINE_BUFFER_SIZE
    #define LINE_BUFFER_SIZE    128
#endif

int setvbuf(register FILE *fp, char *buf, int mode, size_t size)
{
    if (fp->_base != NULL) return 1;	/* return error if i/o has occured */

    if (buf != NULL) {			/* if specified, buffer must be */
	if (mode == _IOLBF) {		/* large enough */
	    if (size < LINE_BUFFER_SIZE) {
		return 1;
	    }
	}
	else if (mode == _IOFBF) {
	    if (size < BUFSIZ) {	/* check for fully buffered case */
		return 1;
	    }
	}
    }

    if ((mode != _IONBF) && (mode != _IOLBF) && (mode != _IOFBF)) {
	return 1;			/* invalid "mode" value */
    }

    /* Input values are ok, proceed */

    fp->_flag &= (~(_IONBF | _IOLBF));	/* 0 buffering flags */

    if (mode == _IONBF)	{		/* set up for unbuffered i/o */	
	fp->_flag |= _IONBF;		/* flag unbuffered i/o */
	return 0;			/* done with unbuffered -- return */
    }
    else if (mode == _IOLBF)
	fp->_flag |= _IOLBF;		/* flag line buffered i/o */

    fp->_base = fp->_ptr = (unsigned char *) buf;	/* assign new buffer */

    return 0;				/* operation succeeded */
}
 
#endif /* EXCLUDE_setvbuf */
 

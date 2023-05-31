#include "lib_top.h"


#if EXCLUDE_flsbuf
    #pragma option -Qws			/* Prevent compiler warning */
#else
 
/****************************************************************************/
/* THIS INFORMATION IS PROPRIETARY TO                                       */
/* MICROTEC RESEARCH, INC.                                                  */
/*--------------------------------------------------------------------------*/
/* Copyright (c) 1987, 1988, 1989, 1991, 1992 Microtec Research, Inc.       */
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
/* NAME                     _ F L S B U F                               */
/*                                                                      */
/* FUNCTION    _flsbuf - write out I/O buffer for file.  Start a new    */
/*                      buffer with the character.                      */
/*                                                                      */
/* SYNOPSIS     _flsbuf (ch, file)                                      */
/*              int ch ;        first character of new buffer           */
/*              register FILE *file ;  pointer to buffer                */
/*                                                                      */
/* RETURN       Character if OK. EOF if buffer not full or error occurs.*/
/*                                                                      */
/* DESCRIPTION  If character device, put out one character.  Else, if   */
/* no buffer, get one from the heap, set _IOMYBUF, insert the first     */
/* character and return.  If the buffer is less than full return EOF.	*/
/*                                                                      */
/* MODIFICATION HISTORY                                                 */
/*	01/21/94 jpc: added fix to allow correct behavior when '\n' is	*/
/*		      written when buffer is full when line buffered.   */
/*		      Also previously, other changes were made to allow	*/
/*		      full-up line buffering.				*/
/*	11/15/93 jpc: added in changes for minimal line buffering.	*/
/*	11/10/93 jpc: DIFF now is unsigned				*/
/*	11/19/91 jpc: removed __STDC__ constructs			*/
/*	       - jpc  set _base to -1 for char device to indicate that	*/
/*		      i/o has occured.					*/
/*	       - jpc  changed to run w. ANSI C 06/13/90			*/
/*             - Herb modified and changed _flags to _flag 12/17/86     */
/*      Author - wb denniston 09/30/86                                  */
/*                                                                      */
/*      MCC86 C Library  - Copyright 1987, Microtec Research, Inc.      */
/************************************************************************/
#include <stdio.h>
#include <stdlib.h>

#if _SIZEOF_PTR == 2
    typedef unsigned short	DIFF;
#else
    typedef unsigned long	DIFF;
#endif

#ifndef LINE_BUFFER_SIZE
    #define LINE_BUFFER_SIZE    128
#endif

int _flsbuf (int input, register FILE *file) 
{
    char buf[1];
    unsigned int buffer_size;
    unsigned char ch = input;
    unsigned int  char_not_written;	/* flag indicated ch already written */

        /* IF CHARACTER DEVICE, PUT OUT ONE CHARACTER */
    if (file->_flag & _IONBF) {
	file->_base = (unsigned char *) -1;	/* Flag that i/o has occured */
					/* (Malloc never returns odd address) */
        file->_cnt = 0 ;		
	buf[0] = ch;
        write (file->_file, buf, 1);
        return (ch) ;
    } 

        /* IF NO BUFFER, MAKE ONE AND INSERT FIRST CHARACTER */
    buffer_size = ((file)->_flag & _IOLBF) ? LINE_BUFFER_SIZE : BUFSIZ;

    if (NULL == file->_base) {
	if (NULL == (file->_base = malloc(buffer_size)))
	    return EOF;			/* malloc failed -- return error */
        file->_flag |= _IOMYBUF;	/* tells fclose to free block */
        file->_ptr = file->_base;
        file->_cnt = buffer_size - 1;	/* count first character */
        *file->_ptr++ = ch ;
        return (ch) ;
    }

        /* IF BUFFER LESS THAN FULL, ADD CHARACTER */
    char_not_written = 1;
    if (file->_cnt >= 0) {
	*file->_ptr++ = ch;
        file->_cnt--;
	char_not_written = 0;		/* Flag character in buffer */
    }

        /* WRITE OUT _cnt CHARACTERS */
    if (write (file->_file, file->_base, 
	    (unsigned) ((DIFF)file->_ptr - (DIFF)file->_base)) < 0) {
        file->_flag |= _IOERR ;
        return (EOF) ;
    }
    file->_ptr = file->_base;
    file->_cnt = buffer_size;

        /* ALL OK. WRITE CHARACTER INTO START OF BUFFER */
    if (char_not_written) {
	if ((ch == '\n') && (file->_flag & _IOLBF)) {	/* line buffered and */
	    buf[0] = ch;				/* ch was a '\n' so */
            write (file->_file, buf, 1);		/* write it */
	} else {					/* otherwise just put */
            *file->_ptr++ = ch;				/* it in the buffer */
            file->_cnt-- ;
	}
    }
    return (ch) ;
}

#endif /* EXCLUDE_flsbuf */
 

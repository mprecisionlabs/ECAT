#include "lib_top.h"


#if EXCLUDE_filbuf
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
/* NAME                       _  F I L B U F                            */
/*                                                                      */
/* FUNCTION    _filbuf - fill up I/O buffer for file. If no buffer then */
/*                      return one character.                           */ 
/*                                                                      */
/* SYNOPSIS     int _filbuf (file)                                      */
/*              FILE    *file ; pointer to file structure               */
/*                                                                      */
/* RETURN       Character if OK. EOF if error or end of file.           */
/*                                                                      */
/* DESCRIPTION  If the error flag is set then return EOF.  If _IONBF is */
/* set then character device so only get one character.  If _base is    */
/* NULL (no buffer assigned) assign a heap buffer and set _IOMYBUF.     */
/* Fill the buffer.                                                     */
/*                                                                      */
/* MODIFICATION HISTORY                                                 */
/*	01/13/94 jpc:	Remove risc version of line buffering and	*/
/*			replaced with general purpose line buffering.	*/
/*	11/15/93 jpc:	Added simple line buffering support supplied by	*/
/*			risc group.					*/
/*	11/19/91 jpc:	removed __STDC__ constructs			*/
/*             _ M.H. change _flags to _flag 12/17/86                   */
/*	       - jpc  changed to run w. ANSI C - 06/13/90		*/
/*	       - jpc  set _base to -1 to indicate that i/o has occured  */
/*      Author - wb denniston 09/30/86                                  */
/*                                                                      */
/*      MCC86 C Library  - Copyright 1987, Microtec Research, Inc.	*/
/************************************************************************/
#include <stdio.h>
#include <stdlib.h>

#ifndef LINE_BUFFER_SIZE
    #define LINE_BUFFER_SIZE    128
#endif

int _filbuf (register FILE *file)
{
    unsigned char buf[1];
    unsigned int buffer_size;
    int	i;

        /* IF ERROR SET, RETURN EOF */
    if (file->_flag & _IOEOF) return(EOF);

        /* IF CHARACTER DEVICE RETURN ONE CHARACTER */
    if (file->_flag & _IONBF) {
        file->_cnt = 0 ;
	file->_base = (unsigned char *) -1;	/* Flag that i/o has occured */
	if (read (file->_file, buf, 1) <= 0) return (EOF);
	else return ((unsigned) buf[0]);
    }

        /* IF BUFFER NOT ASSIGNED, MAKE ONE */
    buffer_size = ((file)->_flag & _IOLBF) ? LINE_BUFFER_SIZE : BUFSIZ;

    if (file->_base == NULL) {
        file->_base = malloc(buffer_size);
        file->_flag |= _IOMYBUF;
        file->_cnt = 0;
    }

	/* FLUSH ALL LINE BUFFERRED OUTPUT STREAMS */
    if (file->_flag & _IOLBF)
	for (i=0; i<FOPEN_MAX; i++) {
	    if ((_iob[i]._flag & (_IOLBF | _IOWRT)) == (_IOLBF | _IOWRT)) {
		if (_iob[i]._base != _iob[i]._ptr) {
		    _iob[i]._cnt++;			/* increment count */
		    _flsbuf (*--_iob[i]._ptr, &_iob[i]);
	        }
	    }
	}
	
        /* FILL THE BUFFER, RESET COUNT */
    file->_cnt = read (file->_file, file->_base, buffer_size);
    if (file->_cnt-- <= 0) {
        file->_flag |= _IOEOF ;
        file->_cnt = 0 ;
        return (EOF) ;
    }

        /* RETURN FIRST CHARACTER, COUNT ALREADY DECREMENTED */
    file->_ptr = file->_base ;
    return ((unsigned) *(unsigned char *)(file->_ptr++));
}

 
#endif /* EXCLUDE_filbuf */
 

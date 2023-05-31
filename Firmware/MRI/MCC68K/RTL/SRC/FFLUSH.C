#include "lib_top.h"


#if EXCLUDE_fflush
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
/* NAME                         F F L U S H                             */
/*                                                                      */
/* FUNCTION     fflush - write the contents of file's buffer to file    */
/*                                                                      */
/* SYNOPSIS     fflush(file)                                            */
/*              FILE *file - pointer to file                            */
/*                                                                      */
/* RETURN       0 if OK. EOF for no buffer, open for READ only or not   */
/*              open.                                                   */  
/*                                                                      */
/*	11/10/93 jpc:   DIFF now unsigned				*/
/*	11/19/91 jpc:	removed __STDC__ constructs			*/
/*             _ M.H. change _flags to _flag 12/17/86                   */
/*             - Clo modified on 12/11/86                               */ 
/*      Author - wb denniston 09/30/86                                  */
/*                                                                      */
/************************************************************************/

/* Revision History:
    18-Nov-86 kjk: using HY's implementation of fflush
   End Revision History */

#include <stdio.h>

#if _SIZEOF_PTR == 2
    typedef unsigned short	DIFF;
#else
    typedef unsigned long	DIFF;
#endif
 
#ifndef LINE_BUFFER_SIZE
    #define LINE_BUFFER_SIZE    128
#endif

int fflush (FILE *file)
{
    DIFF n;

    if (((file->_flag & (_IONBF | _IOWRT)) == _IOWRT) &&
        (file->_base != NULL) && (file->_base != (unsigned char *) -1) &&
        ((n = ((DIFF)file->_ptr - (DIFF)file->_base)) > 0))
        {
        /* the file:
                is not unbuffered,
                is open for write,
                has a buffer allocated,
                has chars in the buffer which need to be flushed
        */
        file->_cnt = ((file)->_flag & _IOLBF) ? LINE_BUFFER_SIZE : BUFSIZ;
        file->_ptr = file->_base;
        if (write(file->_file, file->_base, n) != n)
            {
            file->_flag |= _IOERR;
            return(EOF);                /* error condition */
            }
        }
    return(0);                          /* success */

}
 
#endif /* EXCLUDE_fflush */
 

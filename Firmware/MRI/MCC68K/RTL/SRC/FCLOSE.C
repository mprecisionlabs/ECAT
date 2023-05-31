#include "lib_top.h"


#if EXCLUDE_fclose
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
/* NAME                         F C L O S E                             */
/*                                                                      */
/* FUNCTION     fclose - flush file buffer and close file               */
/*                                                                      */
/* SYNOPSIS     fclose(f)                                               */
/*              FILE *f - file to be closed                             */
/*                                                                      */
/* RETURN       0 if OK.  EOF if error.                                 */
/*                                                                      */
/* DESCRIPTION  If the file is write enabled the buffer is flushed. The */
/* file is closed and the file structure is reset.  If a system buffer  */
/* was allocated from the heap, it is freed.                            */
/*                                                                      */
/* MODIFICATION HISTORY                                                 */
/*	       - jpc  clear _IOMYBUF to prevent close from freeing 	*/
/*		      buffer also.					*/
/*             - M.H. change _flags to _flag 12/17/86                   */
/*      Author - wb denniston 09/30/86                                  */
/*                                                                      */
/*      MCC86 C Library  - Copyright 1987, Microtec Research, Inc.      */
/************************************************************************/

#include <stdio.h>
#include <stdlib.h>

int fclose (register FILE *file)
{
#if DUMMY				/* Used for Z80 and some others */

    return (0);

#else	/* ! DUMMY */

    register int ret = 0;

        /* IF THE FILE IS NOT OPEN RETURN EOF */
    if (! (file->_flag & (_IOREAD | _IOWRT))) return (EOF);

        /* CLEAN UP THE BUFFERS AND CLOSE FILE */
    if (file->_flag & _IOWRT) ret = fflush (file);
    if (file->_flag & _IOMYBUF)
	{
	free (file->_base);
	file->_flag &= ~_IOMYBUF;
	}

    if (close (file->_file) == EOF) ret = EOF;
    file->_base = NULL;
    file->_cnt = 0;
    file->_ptr = NULL;
    file->_flag = 0 ;
    return (ret);

#endif	/* ! DUMMY */
}
 
#endif /* EXCLUDE_fclose */
 

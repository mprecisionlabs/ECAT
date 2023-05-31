#include "lib_top.h"


#if EXCLUDE_setbuf
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
/* NAME                         S E T B U F                             */
/*                                                                      */
/* FUNCTION     setbuf (fp, buf) - assign user buffer to file fp        */
/*                                                                      */
/* SYNOPSIS     void setbuf (fp, buf)                                   */
/*              FILE*   fp - file that gets new buffer                  */
/*              char*   buf - pointer to new file buffer                */
/*                                                                      */
/* RETURN       no return                                               */
/*                                                                      */
/* DESCRIPTION  The buffer is assigned to the specified file.  If the   */
/* buffer pointer is NULL then the file is unbuffered.  The old buffer  */
/* is not freed back to the pool unless it was assigned by the system.  */
/*                                                                      */
/* MODIFICATION HISTORY                                                 */
/*	01/13/94 jpc:	some simple mods for line buffering support	*/
/*	11/19/91 jpc:	removed __STDC__ construct			*/
/*	07/09/90 jpc:	Test _base to see if i/o has occured		*/
/*	09/14/89 jpc:	Added ability to convert to unbuffered i/o	*/
/*			by setting _IONBF flag when buf is NULL		*/
/*             -M.H. change _flags to _flag 12/17/86                    */
/*      Author - wb denniston 09/30/86                                  */
/*                                                                      */
/************************************************************************/
#include <stdio.h>
#include <stdlib.h>

void setbuf(register FILE *fp, char *buf)
{
    if (fp->_base != NULL) return;	/* return if i/o has occured */

    if (buf == NULL)			/* set up for unbuffered i/o */
	{
	fp->_flag &= (~_IOLBF);		/* turn off line buffering */
	fp->_flag |= _IONBF;		/* enable unbuffered i/o */
	return;
	}

    fp->_base = fp->_ptr = (unsigned char *) buf;	/* assign new buffer */
    fp->_flag &= (~(_IOLBF | _IONBF));	/* flag as full buffering */
}


 
#endif /* EXCLUDE_setbuf */
 

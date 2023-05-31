#include "lib_top.h"


#if EXCLUDE_xfopen
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


/*	%W% %G% */

/************************************************************************/
/* NAME                         F O P E N                               */
/*                                                                      */
/* FUNCTION     fopen - open named file allowing opentype access        */
/*                                                                      */
/* SYNOPSIS     FILE *fopen(f,o)                                        */
/*              char *f - file pathname                                 */
/*              char *o - opentype r, w, or a                           */
/*                                                                      */
/* RETURN       pointer to the file structure if OK. Else NULL.         */ 
/*                                                                      */
/* DESCRIPTION  Only r, w and a allowed. A total of FOPEN_MAX files can */
/* be opened (usually 20).  The file is opened and the file handle is   */
/* put into the file structure.  The rest of the file structure is set  */
/* to 0.                                                                */
/*                                                                      */
/* MODIFICATION HISTORY                                                 */
/*		06/28/91 jpc - split from fopen so that freopen could	*/
/*			       be implemented.				*/
/*		05/01/91 jpc - modified for UNIX compatability		*/
/*		04/17/91 jpc - added handling for 29k HIF interface.	*/
/*		08/21/90 jpc - preprocessor symbol START_FILE added	*/
/*		03/26/90 jpc - fopen now returns NULL correctly when	*/
/*			       too many files are openned.		*/
/*		05/31/89 mrm - modified to make correct call to open.	*/
/*			b is not implemented yet, but			*/
/*			is accepted with no effect.			*/
/*		05/25/89 HY - allow '+' and 'b' after "r", "w", "a"	*/
/*             _ M.H. change _flags to _flag 12/17/86                   */
/*      Author - wb denniston 09/30/86                                  */
/*                                                                      */
/************************************************************************/

#include <stdio.h>
#include <stdlib.h>

#ifndef FOPEN_MAX 
#define FOPEN_MAX SYS_OPEN 
#endif

#define	O_RDONLY	0x0000
#define	O_WRONLY	0x0001
#define	O_RDWR		0x0002
#define	O_APPEND	0x0008
#define	O_CREAT		0x0200
#define	O_TRUNC		0x0400
#define O_EXCL		0x0800		/* file must not pre-exist */
					/* for compatibility w. C++ libraries */
#define	O_FORM		0x4000
#define O_BINARY	0x8000

FILE *_fopen (const char *filename, register const char *opentype, FILE *file)
{
register int	type;
	 char	openflag;
	 int	filenumber;
	 int	filehandle;

    if (strlen((char *) opentype) > 3)	/* open type not greater than 3 chars */
	    return (NULL);

    switch (*opentype)	/* Open using UNIX style flags */
    {
	case 'r':
	    openflag = _IOREAD;
	    type = O_RDONLY | O_FORM;
	    for (;;) 
	    {
		switch (*++opentype)
		{
		    case '\0':			/* End of string */
			goto call_open;
		    case '+':			/* read/write */
			openflag |= _IOWRT;
			if ((type & 3) == O_RDONLY) {
			    type += O_RDWR - O_RDONLY;
			    continue;
			    }
			return (NULL);		/* 2nd "+" */
		    case 'b':
			if (type & O_FORM) {
			    type = (type & (~O_FORM)) | O_BINARY;   /* binary */
			    continue;
			    }
		    default:
			return (NULL);		/* 2nd "b" or error condition */
		}
	    }

	case 'w':
	    openflag = _IOWRT;
	    type = O_WRONLY | O_CREAT | O_TRUNC | O_FORM;
	    for (;;)
	    {
		switch (*++opentype)
		{
		    case '\0':
			goto call_open;
		    case '+':
			openflag |= _IOREAD;
			if ((type & 3) == O_WRONLY) {
			    type += O_RDWR - O_WRONLY;
			    continue;
			    }
			return (NULL);	/* 2nd "+" */
		    case 'b':
			if (type & O_FORM) {
			    type = (type & (~O_FORM)) | O_BINARY;   /* binary */
			    continue;
			    }
		    default:
			return (NULL);	/* 2nd "b"  or error condition */
		}
	    }

	case 'a':
	    openflag = _IOWRT;
	    type = O_WRONLY | O_APPEND | O_CREAT | O_FORM;
	    for (;;)
	    {
		switch (*++opentype)
		{
		    case '\0':
			goto call_open;
		    case '+':
			openflag |= _IOREAD;
			if ((type & 3) == O_WRONLY) {
			    type += O_RDWR - O_WRONLY;
			    continue;
			    }
			return (NULL);	/* Second "+" in string */
		    case 'b':
			if (type & O_FORM) {
			     type = (type & (~O_FORM)) | O_BINARY;
			     continue;
			     }
		    default:
			return (NULL);	/* Second "b" or error condition */
		}
	    }

	default:
		return (NULL);
   }

call_open:
   if ((filehandle = open ((char *) filename, type, 0744)) < 0)
	return (NULL);

        /* SET UP FILE STRUCTURE */
    file->_base = NULL;
    file->_cnt  = 0;
    file->_flag = openflag;
    file->_ptr  = NULL;
    file->_file = filehandle ;
    return (file);
}
 
#endif /* EXCLUDE_xfopen */
 

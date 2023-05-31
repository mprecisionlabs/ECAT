#include "lib_top.h"


#if EXCLUDE_fopen
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
/*		06/28/91 jpc - fopen split to _fopen to implement	*/
/*			       freopen.					*/
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

#ifndef FOPEN_MAX 
#define FOPEN_MAX SYS_OPEN 
#endif

#define	START_FILE	0

FILE *_fopen (const char *filename, const char *opentype, FILE *file);

FILE *fopen (const char *filename, const char *opentype)
{
    register int filenumber;

    for (filenumber=START_FILE; filenumber<FOPEN_MAX; filenumber++)
        if ((&_iob[filenumber])->_flag == 0)	/* look for free ele. in _iob */
	    return _fopen (filename, opentype, &_iob[filenumber]);
    return (NULL) ;			/* No free elements in _iob structure */
}
 
#endif /* EXCLUDE_fopen */
 

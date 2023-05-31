#include "lib_top.h"


#if EXCLUDE_freopen
    #pragma option -Qws			/* Prevent compiler warning */
#else
 
/****************************************************************************/
/* THIS INFORMATION IS PROPRIETARY TO                                       */
/* MICROTEC RESEARCH, INC.                                                  */
/*--------------------------------------------------------------------------*/
/* Copyright (c) 1991, 1992 Microtec Research, Inc.                         */
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
/* NAME                         F R E O P E N				*/
/*                                                                      */
/* FUNCTION     freopen - re-open a stream to address a different file	*/
/*                                                                      */
/* SYNOPSIS     FILE *freopen(f, o, stream)				*/
/*              char *f - file pathname                                 */
/*              char *o - opentype as per ANSI				*/
/*		FILE *stream - stream which will be re-openned		*/
/*                                                                      */
/* RETURN       pointer to the file structure if OK. Else NULL.         */ 
/*                                                                      */
/* DESCRIPTION  First "stream" is closed.  Then an open () is performed	*/
/* using "f" and "o".  The file is finally completly opened when	*/
/* information is placed in the "_iob" structure.			*/
/*                                                                      */
/* MODIFICATION HISTORY                                                 */
/*	06/28/91 jpc -	Written						*/
/*                                                                      */
/************************************************************************/

#include <stdio.h>

FILE *_fopen (const char *filename, const char *opentype, FILE *file);

FILE *freopen (const char *filename, const char *opentype, FILE* stream)
{
    fclose (stream);				/* Close stream */
    return _fopen (filename, opentype, stream);	/* Re-open for "filename" */
}
 
#endif /* EXCLUDE_freopen */
 

#include "lib_top.h"


#if EXCLUDE_remove
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


/*	%W% %G%	*/

/************************************************************************/
/* NAME                   R E M O V E					*/
/*                                                                      */
/* FUNCTION     remove - make file unaccessible				*/
/*                                                                      */
/* SYNOPSIS     int remove (const char *filename);			*/
/*			filename - name of the file to be removed	*/
/*                                                                      */
/* RETURN       0 on success; non-zero on failure			*/
/*                                                                      */
/* DESCRIPTION  This routine makes the file described by "filename" to	*/
/*		be no longer accessible by that filename.  If "filename"*/
/*		is the last link to that file, then the file is deleted.*/
/*		Generally, this routine removes the directory entry for	*/
/*		"filename".						*/
/*									*/
/*		This routine calls the system function "unlink".	*/
/*                                                                      */
/* MODIFICATION HISTORY                                                 */
/*                                                                      */
/************************************************************************/

#include	<stdio.h>

extern	int unlink(const char *);

int  remove (const char *filename)
{
	return (unlink(filename));
}
 
#endif /* EXCLUDE_remove */
 

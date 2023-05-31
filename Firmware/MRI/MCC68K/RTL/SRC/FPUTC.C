#include "lib_top.h"


#if EXCLUDE_fputc
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
/* NAME                         F P U T C                               */
/*                                                                      */
/* FUNCTION     fputc - write this character to a file.                 */
/*                                                                      */
/* SYNOPSIS     fputc(ch,file)                                          */
/*              char ch ; character to write to file                    */
/*              FILE *file ; pointer to the file                        */
/*                                                                      */
/* RETURN       character if OK. -1 (EOF) if error or end of file.      */
/*                                                                      */
/* DESCRIPTION  Uses macro putc.  If no buffer exists then the character*/
/*      is written directly to the file. Else the character is written  */
/*      to a buffer that is flushed when it is full.                    */
/*                                                                      */
/* MODIFICATION HISTORY                                                 */
/*      Author - wb denniston 9/30/86                                   */
/*	11/19/91 jpc:	removed __STDC__ constructs			*/
/************************************************************************/
#include <stdio.h>

int fputc (int ch, FILE *file)
{
    return (putc(ch,file));
}
 
#endif /* EXCLUDE_fputc */
 

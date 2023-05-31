#include "lib_top.h"


#if EXCLUDE_ferror
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


/*----------------------------------------------------------------------------
    ferror.c

    28-Jul-1988 (jel)
	11/19/91 jpc:	removed __STDC__ constructs
----------------------------------------------------------------------------*/

#include <stdio.h>

int (ferror) (FILE *stream)
    {
    return ferror (stream);
    }

 
#endif /* EXCLUDE_ferror */
 

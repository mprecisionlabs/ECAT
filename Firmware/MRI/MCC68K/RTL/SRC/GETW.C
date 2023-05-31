#include "lib_top.h"


#if EXCLUDE_getw
    #pragma option -Qws			/* Prevent compiler warning */
#else
 
/************************************************************************/
/* THIS INFORMATION IS PROPRIETARY TO					*/
/* MICROTEC RESEARCH, INC.						*/
/*----------------------------------------------------------------------*/
/* Copyright (c) 1991, 1992 Microtec Research, Inc.                     */
/* All rights reserved							*/
/************************************************************************/
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

#include <stdio.h>
#include <mriext.h>
#include "op_type.h"

int getw (register FILE *file)			/* read a 2-byte integer */
{
    register int returnByte;

    returnByte = getc (file);			/* read low order byte */

#if (_LITTLE_ENDIAN || __i960_BIG_ENDIAN__)
    returnByte |= getc(file) << 8;
    return feof(file) ? EOF : returnByte;	/* return appropriate value */
#else
    return ((returnByte << 8) | getc(file));
#endif
}
 
#endif /* EXCLUDE_getw */
 

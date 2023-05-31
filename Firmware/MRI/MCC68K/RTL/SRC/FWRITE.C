#include "lib_top.h"


#if EXCLUDE_fwrite
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
/* NAME                         F W R I T E                             */
/*                                                                      */
/* FUNCTION     fwrite - write array data to a file                     */
/*                                                                      */
/* SYNOPSIS     fwrite (pc, size, count, fp)                            */
/*              char*   pc - pointer to the source array                */
/*              int     size - number of bytes per block                */
/*              int     count - number of blocks                        */
/*              FILE*   fp - destination file                           */
/*                                                                      */
/* RETURN       number of blocks written                                */
/*                                                                      */
/* DESCRIPTION  Write up to count blocks of size 'size' bytes.  If EOF  */
/* is detected stop writing.  Return the number of blockst actually     */
/* written                                                              */
/*                                                                      */
/* MODIFICATION HISTORY                                                 */
/*	11/19/91 jpc:	removed __STDC__ constructs			*/
/*	jpc    - Modified cpc to "const" -- 10/06/89			*/
/*      Millie - Modified counting logic to give a correct successful   */
/*               element count return value on 12/29/86 		*/
/*      Author - wb denniston 09/30/86                                  */
/*                                                                      */
/************************************************************************/
#include <stdio.h>

size_t fwrite (const void *pc, size_t size, size_t count, FILE *fp)
{
    register int blkcnt ;
    int outtally = count;
    register const char *cpc = pc;

    while (count--) {
        for (blkcnt = 0; blkcnt < size; blkcnt++) {
	    putc(*cpc++, fp);
            if (ferror(fp)) return (outtally - count -1);
	}
    }
    return (outtally - count -1);
}
 
#endif /* EXCLUDE_fwrite */
 

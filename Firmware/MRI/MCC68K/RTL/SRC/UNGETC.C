#include "lib_top.h"


#if EXCLUDE_ungetc
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

#include <stdio.h>

int ungetc(int c, FILE *sp)
{
	if (c == EOF)
	    return (-1);
	if ((sp->_flag & _IOREAD)==0 || sp->_ptr <= sp->_base)
	    return (-1);
	*--sp->_ptr = c;
	sp->_cnt++;
	return (c);
}
 
#endif /* EXCLUDE_ungetc */
 

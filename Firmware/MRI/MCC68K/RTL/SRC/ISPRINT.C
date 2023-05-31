#include "lib_top.h"


#if EXCLUDE_isprint
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
    isprint.c

    27-Jul-1988 (jel)
    08/11/89	 jpc	toupper and tolower changed position in file to
			allow isupper and islower to be processed as macros
    09/22/89	 jpc	split out each ctype function to individual modules.
    11/19/91	 jpc	removed __STDC__ constructs

    ANSI requires that the <ctype.h> macros be available as functions.
----------------------------------------------------------------------------*/

#include <ctype.h>

#undef isprint

int isprint(int c)
    {
    return ((_ctype_+1)[c]&(_U|_L|_N|_P|_B));
    }
 
#endif /* EXCLUDE_isprint */
 

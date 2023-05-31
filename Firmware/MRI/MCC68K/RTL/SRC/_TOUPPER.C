#include "lib_top.h"


#if EXCLUDE__toupper
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


/* %Z%  %M%  %I%  %G% */


/*----------------------------------------------------------------------------
    _toupper.c

    27-Jul-1988 (jel)
    11/19/91	 jpc	removed special __STDC__ constructs
    08/11/89	 jpc	toupper and tolower changed position in file to
			allow isupper and islower to be processed as macros
    09/22/89	 jpc	split out each ctype routine

    ANSI requires that the <ctype.h> macros be available as functions.
----------------------------------------------------------------------------*/

#include <ctype.h>

#undef _toupper

int _toupper(int c)
    {
    return ((c)+('A'-'a'));
    }

#endif /* EXCLUDE__toupper */


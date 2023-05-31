#include "lib_top.h"


#if EXCLUDE_assert
    #pragma option -Qws			/* Prevent compiler warning */
#else

/****************************************************************************/
/* THIS INFORMATION IS PROPRIETARY TO                                       */
/* MICROTEC RESEARCH, INC.                                                  */
/*--------------------------------------------------------------------------*/
/* Copyright (c) 1992 Microtec Research, Inc.				    */
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
/* NAME                      _ A S S E R T				*/ 
/*                                                                      */
/* FUNCTION     _assert - Write "assert" message to stderr.  Then call	*/
/*			  abort ();					*/
/*                                                                      */
/* SYNOPSIS     _assert (const char *exp, const char *file, int line);	*/
/*									*/
/*              const char *exp - string containing expression which	*/
/*			          was tested.				*/
/*									*/
/*		const char *file - string which contains the name of	*/
/*				   the file in which the assert		*/
/*				   occurred.				*/
/*                                                                      */
/*		int line - line number where the assert occurred.	*/
/*                                                                      */
/* RETURN       Nothing							*/
/*                                                                      */
/* DESCRIPTION                                                          */
/*                                                                      */
/* MODIFICATION HISTORY                                                 */
/*	12/22/92 jpc:	written.					*/
/*	01/27/93 jpc:	Added pre-processor code to allow handling	*/
/*			of '881 and '040.				*/
/*                                                                      */
/************************************************************************/

#include <stdio.h>
#include <assert.h>		/* Insure _ASSERT is correctly defined */
#include <stdlib.h>

static const char format[] = "Assertion failed: (%s) file %s, line %d\n";

void _ASSERT (const char *exp, const char *file, int line)
{

    fprintf(stderr, (char *) format, (char *) exp, (char *) file, line);
    abort ();

}

#endif /* EXCLUDE_assert */


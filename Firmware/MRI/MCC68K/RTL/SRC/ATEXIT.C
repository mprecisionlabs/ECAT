#include "lib_top.h"


#if EXCLUDE_atexit
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


/****************************************************************************/
/*	History:							    */
/*	07/03/90 jpc:	Removed test for func == NULL; could cause problems */
/*			on 80x86 and other families of processors	    */
/*	04/01/91 jpc:	static data now defined in csysXXX.c		    */
/*	06/26/91 jpc:	Added dummy version for z80			    */
/****************************************************************************/
/*	%W% %G%	*/

#include <stddef.h>

/*	Maximum number of functions which may be logged for
 * execution upon exit.  This value comes from the ANSI draft
 * standard for C.
 */

#define	_ATEXIT_MAX	32	/* Must modify csys* or equiv. if changed */
				/* on 68k, 88k, or SPARC */

extern void (*_atexit_stack[]) (void);
extern short	_atexit_top;		/* Init to 0 */

int atexit(void (*func)())
{
	if (_atexit_top == _ATEXIT_MAX)
		return(1);	/* Return UNsuccessful status */

	_atexit_stack[_atexit_top++] = func;
	return(0);		/* Return successful status */
}


/*
 *	Execute the functions which have been logged by atexit().
 */

void _atexit_functs ()
{
	while (_atexit_top > 0)
		(*_atexit_stack[--_atexit_top])();
}
 
#endif /* EXCLUDE_atexit */
 

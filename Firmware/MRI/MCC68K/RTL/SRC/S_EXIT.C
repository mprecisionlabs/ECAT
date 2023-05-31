#include "lib_top.h"


#if EXCLUDE_s_exit
    #pragma option -Qws			/* Prevent compiler warning */
#else
 

/************************************************************************/
/* THIS INFORMATION IS PROPRIETARY TO					*/
/* MICROTEC RESEARCH, INC.						*/
/* 2350 Mission College Blvd.						*/
/* Santa Clara, CA 95054						*/
/* USA									*/
/*----------------------------------------------------------------------*/
/* Copyright (c) Microtec Research, Inc. 1986-91, 1992.			*/
/* All rights reserved. 						*/
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

/************************************************************************
*                                                                       *
*  void exit (code) int code;						*
*                                                                       *
*  terminate C program							*
*                                                                       *
*  _exit_abort:								*
*	Flushes and closes all open files then calls _exit() using	*
*	"val" as the argument.  Does not return.			*
*									*
*	This routine is called by exit () and is the final call in the	*
*	default raise (SIGABRT) code and the abort () functions		*
*									*
*  exit:								*
*	Executes all functions specified in atexit () function calls.	*
*	If applicable, executes C++ destructors.  Then calls		*
*	_exit_abort () described above.					*
*									* 
*  									*
*  NOTE:								*
*	The call to _atexit_functs () may be removed if atexit () is	*
*	never called in the user's program.				*
*									*
*	The function _cxxfini () executes the static destructors for	*
*	C++.  If C++ is not being used, this function call may be	*
*	removed.							*
*									*
*	The "for" loop which executes fclose () on all streams may be	*
*	removed if no buffered i/o is performed.  The function fclose ()*
*	causes free () to be linked in.					*
************************************************************************/

#include <stdio.h>
#include "cxx_targ.h"

void _exit_abort (val)
int val;
{
    register int i;

    for (i=0; i < FOPEN_MAX; i++)		/* close all open streams */
        if (_iob[i]._flag & (_IOREAD|_IOWRT))	/* may be removed if no */
	    fclose (&_iob[i]);			/* buffered i/o is performed */

    _exit(val);
}

void exit (val)
int val;
{
 
#if EXCLUDE_ATEXIT
	/* No code inserted here */
#else
    _atexit_functs ();          /* execute functions saved by atexit */
#endif
                                /* may be removed if atexit is never called */
#if CXX_AVAIL
    _cxxfini ();		/* execute static destructors for c++ */
                                /* may be removed if c++ is not being used */
#endif

    _exit_abort (val);		/* finish up by closing streams and */
				/* calling _exit */
}
 
#endif /* EXCLUDE_s_exit */
 

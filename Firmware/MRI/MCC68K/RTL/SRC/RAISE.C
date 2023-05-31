#include "lib_top.h"


#if EXCLUDE_raise
    #pragma option -Qws			/* Prevent compiler warning */
#else
 

/****************************************************************************/
/* THIS INFORMATION IS PROPRIETARY TO                                       */
/* MICROTEC RESEARCH, INC.                                                  */
/*--------------------------------------------------------------------------*/
/* Copyright (c) 1991, 1992 Microtec Research, Inc.			    */
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


/************************************************************************/
/*	int raise (int sig);						*/
/*		sig - signal number					*/
/*									*/
/*	returns:  0 if successful, nonzero of not successful		*/
/*									*/
/*	In practice, this routine simply calls a routine specified	*/
/*		by the user in a signal() call, or if signal() was not	*/
/*		not called, a default routine.				*/
/*									*/
/*  History --								*/
/*	07/17/91 jpc -	Written						*/
/*	02/19/92 jpc -	added _sig_abort for raise (SIGABRT)		*/
/*	02/26/92 jpc -	_sig_abort split to separate module to allow	*/
/*			testing.					*/
/************************************************************************/
/*	%W% %G%	*/

#include <signal.h>
#include <errno.h>
#include <stdlib.h>

void _sig_abort (int var);

static void _sig_noError (int var)
{
    exit (0);
}

static void _sig_error (int var)
{
    exit (1);
}


extern void  (*_sig_functs [6]) (int);		/* array declared in csys */
extern const char _sig_trans [];

int raise (int sig)
{
register char local;
register void (* tempFunct)(int);
void (* _sig_defaults [6]) (int);

_sig_defaults[0] = _sig_noError;
_sig_defaults[1] = _sig_error,
_sig_defaults[2] = _sig_error,
_sig_defaults[3] = _sig_abort,	
_sig_defaults[4] = _sig_error,
_sig_defaults[5] = _sig_noError,

    local = _sig_trans[sig];

    if ((sig > SIGTERM) || (local == -1) || (sig < 0)) {
	errno = EBSIG;				/* bad signal number */
	return 1;				/* cannot raise signal */
	}

    if (_sig_functs[local] == SIG_DFL)
	(*_sig_defaults[local]) (sig);		/* execute default */
    else if ((tempFunct = _sig_functs[local]) != SIG_IGN) {
	_sig_functs[local] = SIG_DFL;		/* set to default */
	(* tempFunct) (sig);			/* execute signal handler */
	}

    return 0;					/* return success */
}
 
#endif /* EXCLUDE_raise */
 

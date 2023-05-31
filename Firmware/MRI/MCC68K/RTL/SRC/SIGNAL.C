#include "lib_top.h"


#if EXCLUDE_signal
    #pragma option -Qws			/* Prevent compiler warning */
#else
 

/****************************************************************************/
/* THIS INFORMATION IS PROPRIETARY TO                                       */
/* MICROTEC RESEARCH, INC.                                                  */
/*--------------------------------------------------------------------------*/
/* Copyright (c) 1991, 1992 Microtec Research, Inc.                         */
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
/*	void (*signal (int sig, void (*funct)(int))) (int);		*/
/*		func - address of function to be called when signal	*/
/*			"sig" occurs.  If func == SIG_DEFL, then the	*/
/*			default handling will occur.  If func ==	*/
/*			SIG_IGN, then the signal is ignored		*/
/*		sig - the signal number to which the signal function	*/
/*			call applies.					*/
/*									*/
/*	returns:  "func" on success, SIG_ERR on failure and "errno" is	*/
/*			set.  						*/
/*									*/
/*	History --							*/
/*		07/17/91 jpc -	Written					*/
/************************************************************************/
/*	%W% %G%	*/

#include <signal.h>
#include <errno.h>
#include <stdlib.h>

extern const char _sig_trans [];

extern void (*_sig_functs [6]) (int);		/* array declared in csys */

void (*signal (int sig, void (*func)(int)))(int)
{
register char local;

    local = _sig_trans [sig];

    if ((sig > SIGTERM) || (local == -1)) {
	errno = EBSIG;				/* set errno to bad signal */
	return SIG_ERR;
	};

    return _sig_functs[local] = func;		/* set signal function */
}
 
#endif /* EXCLUDE_signal */
 

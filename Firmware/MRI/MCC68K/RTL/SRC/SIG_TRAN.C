#include "lib_top.h"


#if EXCLUDE_sig_tran
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
/*	This module simply defines the array _sig_trans which is used	*/
/*	by signal() and raise().					*/
/*									*/
/*	History --							*/
/*		07/17/91 jpc -	Written					*/
/************************************************************************/
/*	%W% %G%	*/

#include <signal.h>
#include <errno.h>
#include <stdlib.h>

const char _sig_trans [] = {
				-1,		/* not impl. */
				-1, 		/* not impl. */
				0,	/* SIGINT */
				-1,		/* not impl. */
				1,	/* SIGILL */
				-1,		/* not impl. */
				-1,		/* not impl. */
				-1,		/* not impl. */
				2,	/* SIGFPE */
				3,	/* SIGABRT */
				-1,		/* not impl. */
				4,	/* SIGSEGV */
				-1,		/* not impl. */
				-1,		/* not impl. */
				-1,		/* not impl. */
				5 };	/* SIGTERM */

 
#endif /* EXCLUDE_sig_tran */
 

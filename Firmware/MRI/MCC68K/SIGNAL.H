
/************************************************************************/
/* Copyright (c) 1988-1991 Microtec Research, Inc.			*/
/* ALL RIGHTS RESERVED							*/
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

/*  <signal.h>   Microtec Research, Inc. ANSI C Compiler  */
/* %W% %G% */

#ifndef __SIGNAL_H
#define __SIGNAL_H	1

typedef unsigned char sig_atomic_t;

#define SIG_DFL (void (*)(int))0
#define SIG_IGN (void (*)(int))2
#define SIG_ERR (void (*)(int))4

#define SIGABRT 9    /* kill */
#define SIGFPE  8    /* floating point exception */
#define SIGILL  4    /* illegal instruction */
#define SIGINT  2    /* interrupt */
#define SIGSEGV 11   /* segmentation violation */
#define SIGTERM 15   /* software termination signal from kill */

#if __STDC__

    #if __cplusplus
	extern "C" {
    #endif

    extern void (*signal(int, void (*)(int)))(int);
    extern int raise(int);

    #if __cplusplus
	}
    #endif

#else

    extern void (*signal())();
    extern int raise();

#endif

#endif /* end __SIGNAL_H */

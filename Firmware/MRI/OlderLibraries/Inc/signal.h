
/************************************************************************/
/* Copyright (c) 1988-1989  Microtec Research, Inc.			*/
/* ALL RIGHTS RESERVED							*/
/************************************************************************/

/*  <signal.h>   Microtec Research, Inc. ANSI C Compiler  */
/* @(#)signal.h	1.2 4/13/90 */

#ifndef __SIGNAL_H
#define __SIGNAL_H

typedef unsigned char sig_atomic_t;

#define SIG_DFL (void (*)(int))0
#define SIG_IGN (void (*)(int))1
#define SIG_ERR (void (*)(int))2

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

extern void (*signal(int sig, void (*func)(int)))(int);
extern int raise(int sig);

#if __cplusplus
}
#endif

#else
extern void (*signal())();
extern int raise();
#endif

#endif /* end __SIGNAL_H */

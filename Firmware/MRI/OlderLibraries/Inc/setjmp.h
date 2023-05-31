
/************************************************************************/
/* Copyright (c) 1988-1989  Microtec Research, Inc.			*/
/* ALL RIGHTS RESERVED							*/
/************************************************************************/

/*  <setjmp.h>	Microtec Research, Inc. ANSI C Compiler  */
/* @(#)setjmp.h	1.5 4/13/90 */

#ifndef __SETJMP_H
#define __SETJMP_H

typedef int jmp_buf[13];

#if __STDC__

#if __cplusplus
extern "C" {
#endif

extern int  setjmp(jmp_buf env);
extern void longjmp(jmp_buf env, int val);

#if __cplusplus
}
#endif

#else
extern int  setjmp();
extern void longjmp();
#endif /* __STDC__ */

#endif /* end __SETJMP_H */

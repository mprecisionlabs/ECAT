
/************************************************************************/
/* Copyright (c) 1988-1989  Microtec Research, Inc.			*/
/* ALL RIGHTS RESERVED							*/
/************************************************************************/

/*  <stddef.h>   Microtec Research, Inc. ANSI C Compiler  */
/* @(#)stddef.h	1.3 4/13/90 */

#ifndef __STDDEF_H
#define __STDDEF_H

typedef int		ptrdiff_t;
typedef unsigned	size_t;
typedef short		wchar_t;
#define NULL		((void*)0)
#define offsetof(s,m)	(size_t)&(((s*)0)->m)


#endif /* end __STDDEF_H */

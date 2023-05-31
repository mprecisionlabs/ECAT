
/************************************************************************/
/* Copyright (c) 1988-1989  Microtec Research, Inc.			*/
/* ALL RIGHTS RESERVED							*/
/************************************************************************/

/*  <assert.h>   Microtec Research, Inc. ANSI C Compiler  */
/* @(#)assert.h	1.1 7/25/89 */

#undef assert

#ifdef NDEBUG

#define assert(ignore) ((void)0)

#else

#ifndef stderr
#include <stdio.h>
#endif

#define assert(exp) ((void) ((exp) ? 0 : (fprintf(stderr, \
   "Assertion failed: (" #exp ") file %s, line %d", __FILE__, __LINE__), \
   exit(1))))

#endif


/************************************************************************/
/* Copyright (c) 1988-1990  Microtec Research, Inc.			*/
/* ALL RIGHTS RESERVED							*/
/************************************************************************/

#ifndef __STDARG_H
#define __STDARG_H

/*  <stdarg.h>   Microtec Research, Inc. ANSI C Compiler  */
/* @(#)stdarg.h	1.3 4/13/90 */

typedef char *va_list;
#define va_start(ap,param) ((void)((ap)=(char*)(&param+1)))
#define va_arg(ap,type) (((type*)((ap)+=sizeof(type)))[-1])
#define va_end(ap) ((void)0)

#endif /* end __STDARG_H */

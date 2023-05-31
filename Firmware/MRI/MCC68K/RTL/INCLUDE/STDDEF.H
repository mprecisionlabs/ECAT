
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

/*  <stddef.h>   Microtec Research, Inc. ANSI C Compiler  */
/* %W% %G% */

#ifndef __STDDEF_H
#define __STDDEF_H	1

#if __STDC__

    #ifndef _PTRDIFF_T_DEFINED
	#ifdef _PTRDIFF_T
	    typedef _PTRDIFF_T ptrdiff_t;
	#else
	    typedef int ptrdiff_t;
	#endif
	#define	_PTRDIFF_T_DEFINED	1
    #endif

    #ifndef _WCHAR_T_DEFINED
	#ifdef _WCHAR_T
	    typedef _WCHAR_T wchar_t;
	#else
	    typedef unsigned char wchar_t;
	#endif
	#define	_WCHAR_T_DEFINED	1
    #endif

    #define offsetof(s,m) ((size_t)&(((s*)0)->m))

    #ifndef _SIZE_T_DEFINED
	#ifdef _sun
	    typedef int size_t;
	#else
	    typedef _SIZE_T size_t;
	#endif
	#define	_SIZE_T_DEFINED		1
    #endif

#else	/* ! __STDC__ */

    #define offsetof(s,m) ((unsigned)&(((s*)0)->m))

#endif	/* ! __STDC__ */

#if __cplusplus
    #define	NULL		(0)
#else
    #define	NULL		((void *)0)
#endif

#endif /* end __STDDEF_H */

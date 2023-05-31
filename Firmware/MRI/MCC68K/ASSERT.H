
/************************************************************************/
/* Copyright (c) 1988-1991, 1992, 1993 Microtec Research, Inc.		*/
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

/*  <assert.h>   Microtec Research, Inc. ANSI C Compiler  */
/* %W% %G% */

#undef assert

#ifdef NDEBUG

    #define assert(ignore) ((void)0)

#else

    #if __cplusplus
	extern "C" {
    #endif

	void _ASSERT (const char *, const char *, int);

    #if __cplusplus
	}
    #endif

    #define assert(exp) ((void) ((exp) ? (void) 0 : _ASSERT(#exp, __FILE__, \
	__LINE__) ))

#endif

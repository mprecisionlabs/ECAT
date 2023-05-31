
/************************************************************************/
/* Copyright (c) 1988-1991, 1992 Microtec Research, Inc.                */
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

/*  <setjmp.h>	Microtec Research, Inc. ANSI C Compiler  */
/* %W% %G% */

#ifndef __SETJMP_H
#define __SETJMP_H	1

#if 	defined(_MCC88K)
    typedef int jmp_buf[19];
#elif	defined(_MCCSP)
    typedef int jmp_buf[32];
#elif	defined(_MCC80)
    typedef int jmp_buf[6];
#elif	defined(_MCC29K)
    typedef int jmp_buf[4];
#elif	defined(_MCC960)
    typedef int jmp_buf[32];
#elif	defined(_MCCT9K)
    typedef int jmp_buf[12];
#else
    typedef int jmp_buf[13];
#endif

#if __STDC__

    #if __cplusplus
	extern "C" {
    #endif

    extern int  setjmp(jmp_buf);
    extern void longjmp(jmp_buf, int);

    #if __cplusplus
	}
    #endif

#else

    extern int  setjmp();
    extern void longjmp();

#endif /* __STDC__ */

#endif /* end __SETJMP_H */

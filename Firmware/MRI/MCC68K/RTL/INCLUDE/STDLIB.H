
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
/*  <stdlib.h>	Microtec Research, Inc. ANSI C Compiler  */
/* %W% %G% */


#ifndef __STDLIB_H
#define __STDLIB_H	1

#if __STDC__

    #ifndef _WCHAR_T_DEFINED
	#ifdef _sun
	    typedef unsigned short wchar_t;
	#else
	    typedef _WCHAR_T wchar_t;
	#endif
	#define	_WCHAR_T_DEFINED	1
    #endif

    #ifndef	_SIZE_T_DEFINED
	#ifdef _sun
	    typedef int size_t;
	#else
	    typedef _SIZE_T size_t;
	#endif
	#define	_SIZE_T_DEFINED		1
    #endif

#endif /* __STDC__ */

typedef struct div_t {int  quot, rem;} div_t;
typedef struct ldiv_t {long quot, rem;} ldiv_t;

#define EXIT_FAILURE	1
#define EXIT_SUCCESS	0
#define RAND_MAX	0x7FFF
#define MB_CUR_MAX	1

#if __cplusplus
    #define	NULL		(0)
#else
    #define	NULL		((void *)0)
#endif

#if __STDC__

    #if __cplusplus
	extern "C" {
    #endif

    extern double    atof(const char *);
    extern int       atoi(const char *);
    extern long      atol(const char *);
    extern double    strtod(const char *, char **);
    extern long      strtol(const char *, char **, int);
    extern unsigned  long strtoul(const char *, char **, int);
    extern int       rand(void);
    extern void      srand(unsigned);
    extern void     *calloc(size_t, size_t);
    extern void      free(void *);
    extern void     *malloc(size_t);
    extern void     *realloc(void *, size_t);

    #ifdef _sun
	extern int	abort(void);
    #else
	extern void	abort(void);
    #endif

    extern int       atexit(void (*)(void));
    extern void      exit(int);
    extern char     *getenv(const char *);
    extern int       system(const char *);
    extern void     *bsearch(const void *, const void *, size_t,
			size_t, int (*)(const void *, const void *));
    extern void      qsort(void *, size_t, size_t,
			int (*)(const void *, const void *));
    extern int       abs(int);
    extern div_t     div(int, int);
    extern long      labs(long);
    extern ldiv_t    ldiv(long, long);
    extern int       mblen(const char *, size_t);
    extern int       mbtowc(wchar_t *, const char *, size_t);
    extern int       wctomb(char *, wchar_t);
    extern size_t    mbstowcs(wchar_t *, const char *, size_t);
    extern size_t    wcstombs(char *, const wchar_t *, size_t);

    #if __cplusplus
	}
    #endif

#else /* __STDC__ */

    extern double    atof();
    extern int       atoi();
    extern long      atol();
    extern double    strtod();
    extern long      strtol();
    extern unsigned  long strtoul();
    extern int       rand();
    extern void      srand();
    extern void     *calloc();
    extern void      free();
    extern void     *malloc();
    extern void     *realloc();

    #ifdef _sun
	extern int	abort();
    #else
	extern void	abort();
    #endif

    extern int       atexit();
    extern void      exit();
    extern char     *getenv();
    extern int       system();
    extern void     *bsearch();
    extern void      qsort();
    extern int       abs();
    extern div_t     div();
    extern long      labs();
    extern ldiv_t    ldiv();
    extern int       mblen();
    extern int       mbtowc();
    extern int       wctomb();
    extern int	     mbstowcs();
    extern int	     wcstombs();

#endif	/* ! __STDC__ */

#endif /* end __STDLIB_H */
